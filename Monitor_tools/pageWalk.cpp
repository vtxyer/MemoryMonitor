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
		if( it->second.pte_data.empty() ){
			return 0;
		}
		else
			return 1;
	}

}
void check_cr3_list(DATAMAP &list, unsigned long *cr3_list, int list_size)
{
	unsigned long cr3;	
	int i, ret;

	for(i=0; i<list_size; i++){
		cr3 = cr3_list[i];
		ret = check_cr3(list, cr3);
		if(ret == 0){
			struct hash_table tmp;
			tmp.start_round = round;
			tmp.end_round = round;
			tmp.check = 0;
			list.insert(DATAMAP::value_type(cr3, tmp));
		}
		else{
			struct hash_table &h = list[cr3];
			h.end_round = round;
			h.check = 1;
		}
	}
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
	it = table->pte_data.find(vkey);
	if(it == table->pte_data.end()){
		mapData mapVal;
		mapVal.present_times = valid_bit;

		if(valid_bit == 1){
			save_paddr(mapVal.paddr, paddr);
		}
		else{
			save_swap_paddr(mapVal.paddr, paddr);
		}
		save_huge_bit(mapVal.paddr, huge_bit);
		set_change_bit(mapVal.paddr, true);

		table->pte_data.insert(map<unsigned long, mapData>::value_type(vkey, mapVal));
		ret = 0;
	}
	else{
		char &val_ref = table->pte_data[vkey].present_times;	
		unsigned long &paddr_val = table->pte_data[vkey].paddr;

		if(valid_bit == 1){
			save_paddr(paddr_val, paddr);
		}
		else{
			save_swap_paddr(paddr_val, paddr);
		}
		save_huge_bit(paddr_val, huge_bit);


		val = val_ref&1;
		//non-swap to swap bit
		if( val==1 && valid_bit==0 ){	
			//			fprintf(stderr, "bit:%x non2s va:%lx\n", val_ref, gw->va);
			add_change_number(val_ref);
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
			set_change_bit(paddr_val, true);
		}
		else{
			set_change_bit(paddr_val, false);
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


	table->check = table->activity_page[0] = table->activity_page[1] = 0;
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

						/*
						 * The page is freed, we don't have to count into
						 * But there are some problem with shared memory if we didn't count it??????????
						 */
						map<unsigned long, mapData>::iterator tmp_it;
						tmp_it = table->pte_data.find(gw.va);
						unsigned long paddr = get_paddr(tmp_it->second.paddr);
						if(gw.l1e == 0 && paddr != 0 ){
							table->pte_data.erase( tmp_it );
							continue;
						}

						if( !pte_entry_valid(gw.l1e))
						{
							count++;
							int ret;
							ret = compare_swap(table, &gw, l1offset, 0, 0);
						}
						else
						{
							int ret;
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
		table->pte_data.clear();



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

	return count;

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
		if(cr3 == 0x187000)
			continue;
		page_walk_ia32e(cr3, &h, gw);
	}
}

/*retrieve cr3 list for long time no used*/
int retrieve_list(DATAMAP &list)
{
	DATAMAP::iterator it = list.begin();
	int retrieve_cr3_number = 0, interval;
	unsigned int max_int = -1;


	while(it != list.end())
	{
		struct hash_table &h = it->second;
		if(round > h.end_round){
			interval = round - h.end_round;
		}
		else{
			interval = max_int - h.end_round + round;	
		}

		if(interval >= MAX_ROUND_INTERVAL){
			h.pte_data.clear();
			retrieve_cr3_number++;
		}
		it++;
	}

	return retrieve_cr3_number;
}

static void estimate_bottleneck_set(SHARED_TREE *system_map, int valid_bit, 
		sharedID_t sharedID, struct hash_table *h, int huge_bit, unsigned long &total_change_times, unsigned long cr3,
		bool is_change_times_add)
{


	/*The page is shared with each other*/
	if(system_map->count(sharedID) > 0){
//		system_map->at(sharedID).add_share_relation(cr3);
	}
	/*Page is not shared*/
	else{
		Shared_page_node shared_node;
//		system_map->insert(pair<sharedID_t, Shared_page_node>(sharedID, shared_node));
//		system_map->at(sharedID).add_share_relation(cr3);
		if(is_change_times_add)		
			total_change_times += 1;
		if(huge_bit == 0){
			(h->activity_page)[valid_bit]++;
		}
		else{
			(h->activity_page)[valid_bit] += 512;
		}
	}
}
unsigned long calculate_all_page(DATAMAP &list, unsigned long *result)
{
	DATAMAP::iterator it = list.begin();
	unsigned long check_cr3_num = 0;
	unsigned long total_change_times = 0;
	Sampled_data sample_data;
	sharedID_t sharedID;

	sample_result.insert(pair<round_t, Sampled_data>(round, sample_data));
	while(it != list.end())
	{
		struct hash_table &h = it->second;
		unsigned long cr3 = it->first;
		addr_t paddr;
		check_cr3_num++;

		HASHMAP::iterator hashIt = h.pte_data.begin();
		(h.activity_page)[0] = (h.activity_page)[1] = (h.activity_page)[2] = 0;

		if(h.check == 1){

		while(hashIt != h.pte_data.end()){	
			byte valid_bit = (hashIt->second.present_times) & 1;
			byte val_ref = hashIt->second.present_times;
			unsigned int change_times = get_change_number(val_ref);
			char huge_bit = get_huge_bit(hashIt->second.paddr);
			bool is_change_times_add = false;

			/*Is sharedID ok when valid_bit = 1 ????????*/
			sharedID = get_swap_id(hashIt->second.paddr);
			paddr = get_paddr(hashIt->second.paddr);
	
			/* 
			 * The data have not been valid and change times already add 1, 
			 * so it need to decrease 1
			 * */
			if( h.check == 1 && is_change_bit_set(hashIt->second.paddr) ){
				is_change_times_add = true;
			}
			set_change_bit(hashIt->second.paddr, false);

			if(change_times >= CHANGE_LIMIT ){
				estimate_bottleneck_set( &(sample_result[round].shared_tree), valid_bit, 
						sharedID, &h, huge_bit, total_change_times, cr3,
						is_change_times_add);
			}

			/*Only for estimating working set size for this round*/
			if(valid_bit){
				if(system_map_wks.count(paddr) == 0){
					h.activity_page[valid_bit]++;
					system_map_wks[paddr] = 1;
				}
				else{
					system_map_wks[paddr]++;
				}
			}

			hashIt++;
		}//end walk each page for CR3

		}//end if h.check

		sample_result[round].cr3_info[cr3] = h.activity_page[0];
		result[0] += h.activity_page[0];
		result[1] += h.activity_page[1];
		it++;
	}
	sample_result[round].set_value( result[0] );
	global_total_change_times += total_change_times;
	system_map_wks.clear();

	return check_cr3_num;
}
