#include "define.h"

void* map_page(unsigned long pa_base, int level, struct guest_pagetable_walk *gw)
{	
	pa_base >>= 12;
	pa_base &= ADDR_MASK;
	switch(level){
		case 1:
			gw->l1mfn = pa_base;
			return xc_map_foreign_range( xch1, domID, XC_PAGE_SIZE, PROT_READ, pa_base);
			break;
		case 2:
			gw->l2mfn = pa_base;
			return xc_map_foreign_range( xch2, domID, XC_PAGE_SIZE, PROT_READ, pa_base);
			break;
		case 3:
			gw->l3mfn = pa_base;
			return xc_map_foreign_range( xch3, domID, XC_PAGE_SIZE, PROT_READ, pa_base);
			break;
		case 4:
			gw->l4mfn = pa_base;
			return xc_map_foreign_range( xch4, domID, XC_PAGE_SIZE, PROT_READ, pa_base);
			break;
	}
}
int check_cr3(DATAMAP &list, unsigned long cr3)
{
	DATAMAP::iterator it;
	it = list.find(cr3);
	if(it==list.end()){		
		return 0;
	}
	else{
		return 1;
	}

}
unsigned long check_cr3_list(DATAMAP &list, unsigned long *cr3_list, int list_size)
{
	unsigned long cr3, total_change_page;	
	int i, ret;

	total_change_page = 0;
	for(i=0; i<list_size; i++){
		cr3 = cr3_list[i];
		ret = check_cr3(list, cr3);
		if(ret == 0){
			struct hash_table tmp;
			tmp.cr3 = cr3;
			tmp.change_page = 0;
			tmp.start_round = round;
			tmp.end_round = round;
			list.insert(DATAMAP::value_type(cr3, tmp));
		}
		else{
			struct hash_table &h = list[cr3];
			h.end_round = round;
			h.check = 1;
			total_change_page += h.change_page;	
		}
	}

	return total_change_page;
}

/*
 * valid_bit=0 => in swap
 * valid_bit=1 => is valid
 * */
int compare_swap(struct hash_table *table, struct guest_pagetable_walk *gw, unsigned long offset, char valid_bit, char huge_bit)
{
	unsigned long vkey, paddr;
	char val, tmp;
	unsigned long entry_size = 8;
	int ret = 0;
	map<unsigned long, mapData>::iterator it;


	vkey = gw->va;

	/*!!!!!! NOTE !!!!!!!
	 * I assume bit 13~48 also represent swap file offset
	 * */
	if(huge_bit == 0 )
		paddr = ((gw->l1e) & ADDR_MASK)>>(unsigned long)12;
	else
		paddr = ((gw->l2e) & ADDR_MASK)>>(unsigned long)12;


	/*insert into each process map*/
	it = table->h.find(vkey);
	if(it == table->h.end()){
		mapData mapVal;
		mapVal.present_times = valid_bit;

		if(valid_bit == 1){
			save_paddr(mapVal.paddr, paddr);
		}
		else{
			save_swap_paddr(mapVal.paddr, paddr);
		}
		save_huge_bit(mapVal.paddr, huge_bit);

		table->h.insert(map<unsigned long, mapData>::value_type(vkey, mapVal));
		ret = 0;
	}
	else{
		char &val_ref = table->h[vkey].present_times;	

		if(valid_bit == 1){
			save_paddr(table->h[vkey].paddr, paddr);
		}
		else{
			save_swap_paddr(table->h[vkey].paddr, paddr);
		}
		save_huge_bit(table->h[vkey].paddr, huge_bit);


		val = val_ref&1;
		//non-swap to swap bit
		if( val==1 && valid_bit==0 ){	
			//			fprintf(stderr, "bit:%x non2s va:%lx\n", val_ref, gw->va);
			add_change_number(val_ref);
			tmp = get_change_number(val_ref);			
			if(tmp >= CHANGE_LIMIT){
				(table->change_page)++;
			}
			ret = 1;
		}
		//swap to non-swap bit
		else if(val==0 && valid_bit==1){
			//			fprintf(stderr, "bit:%x s2non va:%lx\n", val_ref, gw->va);
			add_change_number(val_ref);			
			ret = 0;		
		}

		if(val!=valid_bit){
			val_ref &= 0xfe;
			val_ref |= valid_bit; 
		}
	}
	return ret;
}


unsigned long page_walk_ia32e(addr_t dtb, struct hash_table *table, struct guest_pagetable_walk &gw)
{
	unsigned long count=0, total=0;	
	unsigned long *l1p, *l2p, *l3p, *l4p;

	uint64_t pml4e = 0, pdpte = 0, pde = 0, pte = 0, access=0;
	unsigned long l1offset, l2offset, l3offset, l4offset;
	int l1num, l2num, l3num, l4num;
	int flag;
	unsigned int hugepage_counter = 0;


	table->check = table->change_page = table->activity_page[0] = table->activity_page[1] =
		table->total_valid_pages = 0;
	l1num = 512;
	l2num = 512;
	l3num = 512;
	l4num = 512;
	//	l4num = 511;
	//l4num = 1;
	memset(&gw, 0, sizeof(struct guest_pagetable_walk));	


	if(!sigsetjmp(sigbuf, 1)){
		l4p = (unsigned long *)map_page(dtb, 4, &gw);
		if(l4p == NULL){
			printf("cr3 map error\n");
			return -1;
		}
		for(l4offset=0; l4offset<l4num; l4offset++)
		{
			gw.l4e = l4p[l4offset];
			if( !entry_valid(gw.l4e)){
				continue;
			}
			l3p = (unsigned long *)map_page(gw.l4e, 3, &gw);
			if(l3p == NULL){
				continue;
			}
			for(l3offset=0; l3offset<l3num; l3offset++)
			{
				gw.l3e = l3p[l3offset];
				if( !entry_valid(gw.l3e)){
					continue;
				}
				if(page_size_flag(gw.l3e)) //1GB huge page
				{
					//hugepage_counter++;
					continue;
				}
				l2p = (unsigned long *)map_page(gw.l3e, 2, &gw);
				if(l2p == NULL){
					continue;
				}
				for(l2offset=0; l2offset<l2num; l2offset++)
				{
					gw.l2e = l2p[l2offset];
					if( !entry_valid(gw.l2e)){
						continue;
					}

					if( page_size_flag(gw.l2e) ) //2MB huge page
					{

						//for ignore already map page => huge page only see User space
						if( get_bit( gw.l2e, 1, 2) ){
							total += 512;
							hugepage_counter++;					
							gw.va = get_vaddr(0, l2offset, l3offset, l4offset);
							if( !pte_entry_valid(gw.l2e)){
								printf("huge swap\n");
								compare_swap(table, &gw, l2offset, 0, 1);								
							}
							else{
								compare_swap(table, &gw, l2offset, 1, 1);
							}
						}

						continue;
					}

					l1p = (unsigned long *)map_page(gw.l2e, 1, &gw);
					if(l1p == NULL){
						continue;
					}
					for(l1offset=0; l1offset<l1num; l1offset++)
					{
						gw.l1e = l1p[l1offset];
						gw.va = get_vaddr(l1offset, l2offset, l3offset, l4offset);

						/*Even gw.l1e==0 also continue or there will some problem with share memory*/
						if( gw.va == 0 )
							continue;

						if( !pte_entry_valid(gw.l1e))
						{
							count++;
							int ret;
							ret = compare_swap(table, &gw, l1offset, 0, 0);
						}
						else
						{
							int ret;
							//       						total_valid_calculate( (((gw.l1e)>>12)&ADDR_MASK ), table->total_valid_pages);
							(table->total_valid_pages)++;  						
							ret = compare_swap(table, &gw, l1offset, 1, 0);
							total++;
						}
					}	 
					munmap(l1p, XC_PAGE_SIZE);
					l1p = NULL;
				}	
				munmap(l2p, XC_PAGE_SIZE);
				l2p = NULL;
			}	
			munmap(l3p, XC_PAGE_SIZE);
			l3p = NULL;
		}	
		munmap(l4p, XC_PAGE_SIZE);
		l4p = NULL;
	}
	else{
		/*bus error*/
		table->h.clear();		
		if(l1p != NULL)
			munmap(l1p, XC_PAGE_SIZE);
		if(l2p != NULL)
			munmap(l2p, XC_PAGE_SIZE);
		if(l3p != NULL)
			munmap(l3p, XC_PAGE_SIZE);
		if(l4p != NULL)
			munmap(l4p, XC_PAGE_SIZE);		
		goto BUSERR;
	}

BUSERR: 

	//	if(table->non2s != 0)
	//		printf("###All_pages:%lu swap_page:%lu non2s:%lu s2non:%lu total_page: %lu hugepage_counter:%u cr3:%lx###\n", 
	//				total, count, table->non2s, table->s2non, table->total_valid_pages, hugepage_counter, dtb);
	return count;
}


int walk_cr3_list(DATAMAP &list, unsigned long *cr3_list, int list_size,  unsigned int round, struct guest_pagetable_walk &gw)
{
	unsigned long cr3;
	for(int i=0; i<list_size; i++)
	{
		cr3 = cr3_list[i];
		//If there are no struct for this cr3, map will automated assigned new struct
		struct hash_table &h = list[cr3];
		h.cr3 = cr3;
		h.round = round;
		if(cr3 == 0x187000)
			continue;
		page_walk_ia32e(cr3, &h, gw);
	}
}
int retrieve_list(DATAMAP &list, unsigned int round)
{
	DATAMAP::iterator it = list.begin();
	int retrieve_cr3_number = 0, interval;
	unsigned int max_int = -1;
	while(it != list.end())
	{
		struct hash_table &h = it->second;
		if(round > h.round){
			interval = round - h.round;
		}
		else{
			interval = max_int - h.round + round;	
		}

		if(interval >= MAX_ROUND_INTERVAL){
			DATAMAP::iterator erase_node = it;
			it++;
			list.erase(erase_node);
			retrieve_cr3_number++;
		}
		else{
			it++;
		}
	}

	return retrieve_cr3_number;
}

unsigned long calculate_all_page(DATAMAP &list, unsigned long *result)
{
	DATAMAP::iterator it = list.begin();
	unsigned long check_cr3_num = 0;
	unsigned long total_change_times = 0;
	SYSTEM_MAP *system_map;
	map<unsigned long, struct page_data>page_owner;
	Sampled_data sample_data;

	sample_result.insert(pair<unsigned int, Sampled_data>(round, sample_data));

	while(it != list.end())
	{
		unsigned long change_times;
		struct hash_table &h = it->second;
		unsigned long cr3 = it->first;
		check_cr3_num++;

		if(h.check == 1){
			sample_result[round].add_process(cr3);
			HASHMAP::iterator hashIt = h.h.begin();
			while(hashIt != h.h.end()){
				unsigned long paddr;
				byte valid_bit = (hashIt->second.present_times) & 1;
				byte val_ref = hashIt->second.present_times;
				char huge_bit = get_huge_bit(hashIt->second.paddr);

				if(valid_bit == 0){
					system_map = &system_map_swap;
					paddr = get_swap_id(hashIt->second.paddr);
				}
				else{
					system_map = &system_map_wks;
					paddr = get_paddr(hashIt->second.paddr);
				}

				change_times = get_change_number(val_ref);
				if(  (change_times >= CHANGE_LIMIT && valid_bit == 0) || valid_bit == 1 ){
					//			if(  ( valid_bit == 0 ){
					/*check if paddr already stored in system_map*/
					if(system_map->count(paddr) > 0){
						if(page_owner[paddr].max_change_times < change_times){
							page_owner[paddr].max_change_times = change_times;
						}
						unsigned long owner = page_owner[paddr].owner;
						(system_map->at(paddr))++;
						sample_result[round].add_share(owner, cr3);
					}
					else{
						struct page_data page_data;
						page_data.owner = cr3;
						page_data.max_change_times = change_times;

						page_owner.insert(pair<unsigned long, struct page_data>( paddr, page_data));
						system_map->insert(pair<unsigned long, byte>(paddr, 1));
						if(huge_bit == 0){
							(h.activity_page)[valid_bit]++;
						}
						else{
							(h.activity_page)[valid_bit] += 512;
						}
						total_change_times += change_times;
					}
				}
				hashIt++;
				}

				result[0] += h.activity_page[0];
				result[1] += h.activity_page[1];
				result[2] += h.total_valid_pages;
			}
			it++;
		}
		sample_result[round].set_value( result[0], total_change_times);

		page_owner.clear();
		return check_cr3_num;
	}

