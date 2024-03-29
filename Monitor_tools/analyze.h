extern "C"{
#include <stdio.h>  
#include <stdlib.h>  
#include <errno.h>  
#include <sys/ioctl.h>  
#include <sys/types.h>  
#include <fcntl.h>  
#include <string.h>  
#include <xenctrl.h>  
#include <xen/sys/privcmd.h> 
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <xs.h>
#include <setjmp.h>
}
#include <map>
using namespace std;

#define CHANGE_LIMIT 1
#define MAX_ROUND_INTERVAL 20
#define ADDR_MASK 0x0000ffffffffffff
#define RECENT_CR3_SIZE 50
#define DECAY_FACTOR 0.9

typedef unsigned long addr_t;
typedef unsigned long mfn_t;
typedef char byte;
typedef map<unsigned long, struct hash_table> DATAMAP;
typedef map<unsigned long, unsigned long> SYSTEM_MAP;

/*globall variable*/
int domID;
SYSTEM_MAP system_map_wks;
SYSTEM_MAP system_map_swap;
//SYSTEM_MAP system_map_total_valid;
xc_interface *xch4, *xch3, *xch2, *xch1;
sigjmp_buf sigbuf;


/*struct*/
struct mapData
{
	byte present_times;
	short weight;
	/*
	 * valid_bit==1 -> [63:28]:paddr, [0:0]:huge_bit
	 * valid_bit==0 -> [63:28]:paddr, [27:1]:swap_paddr, [0:0]huge_bit
	 * */
	unsigned long paddr;
};
typedef struct mapData mapData;
typedef map<unsigned long, mapData> HASHMAP;
struct hash_table
{
	map<unsigned long, mapData> h; //bit 0=>valid_bit, 1~8 => counter
	unsigned long cr3;
	unsigned long count;
	unsigned long change_page, total_valid_pages;
	double activity_page[2];//0->invalid, 1->valid
	unsigned long round;
	byte check;
};
struct guest_pagetable_walk
{
	unsigned long va;
	unsigned long l4e;            /* Guest's level 4 entry */
	unsigned long l3e;            /* Guest's level 3 entry */
	unsigned long l2e;            /* Guest's level 2 entry */
	unsigned long l1e;            /* Guest's level 1 entry (or fabrication) */
	mfn_t l4mfn;                /* MFN that the level 4 entry was in */
	mfn_t l3mfn;                /* MFN that the level 3 entry was in */
	mfn_t l2mfn;                /* MFN that the level 2 entry was in */
	mfn_t l1mfn;                /* MFN that the level 1 entry was in */
};



//1~7 bits represent change number
int add_change_number(byte &value)
{
	byte number;
	number = (value>>1) & 0x7f;
	if(number<0x7f)
	{
		number += 1;
		number <<= 1;
		number |= (value&1);
		value = number;
	}
}
byte get_change_number(byte value)
{
	byte number;
	number = (value>>1) & 0x7f;
	return number;
} 
unsigned long get_bit(unsigned long entry, int num, int position)
{
	unsigned long mask = 0;
	unsigned long i, value;
	for(i=0; i<num; i++){
		mask<<=(unsigned long)1;
		mask+=1;
	}
	value = (entry&(mask<<(unsigned long)position));
	value >>= (unsigned long)position;
	return value;
}
void clear_bit(unsigned long &addr, int start, int end)
{
	unsigned long mask = 0;
	unsigned long tmp = -1;
	for(int i=start; i<=end; i++){
		mask<<=1;
		mask|=1;
	}
	mask <<= start;
	mask ^= tmp;
	addr &= mask;
}
int page_size_flag (uint64_t entry)
{
	return get_bit(entry, 1, 7);
}
int entry_present(uint64_t entry){
	return get_bit(entry, 1, 0);
}
int get_access_bit(uint64_t entry)
{
	return get_bit(entry, 1, 5);
}
int get_huge_bit(uint64_t entry)
{
	return get_bit(entry, 1, 0);
}
unsigned long get_paddr(uint64_t addr)
{
	return get_bit(addr, 36, 28);
}
unsigned long get_swap_id(uint64_t addr)
{
	return get_bit(addr, 63, 1);
}
void save_paddr(unsigned long &addr, unsigned long val)
{
	clear_bit(addr, 28, 63);
	addr |= (val<<28);
}
void save_swap_paddr(unsigned long &addr, unsigned long val)
{
	clear_bit(addr, 1, 27);
	addr |= ((val&0xeffffff)<<1);
}
void save_huge_bit(unsigned long &addr, unsigned long val)
{
	clear_bit(addr, 0, 0);
	addr |= (val & 0x1);
}
unsigned long get_vaddr(unsigned long l1offset, unsigned long l2offset, unsigned long l3offset, unsigned long l4offset)
{
	unsigned long va = 0, tmp;

	tmp = 0;
	tmp |= (l1offset<<12);
	va |= tmp;
	tmp = 0;
	tmp |= (l2offset<<21);
	va |= tmp;tmp = 0;
	tmp |= (l3offset<<30);
	va |= tmp;tmp = 0;
	tmp |= (l4offset<<39);
	va |= tmp;

	return va;
}
int entry_valid(unsigned long entry)
{
	int flag = entry & 0xfff;
	uint32_t gflags, mflags, iflags, rc = 0;

	//	if( (entry&1) == 0 || (get_access_bit(entry) == 0)){
	if( (entry&1) == 0 ){
		return 0; //invalid
	}
	else
		return 1; //valid
}
int pte_entry_valid(unsigned long entry)
{
	return entry_present(entry);
}
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



extern "C"
{
	int init_hypercall(int recent_cr3_size, int fd)
	{
		int ret, i;

		//Init
		privcmd_hypercall_t hyper1 = { 
			__HYPERVISOR_vt_op, 
			{ 1, domID, recent_cr3_size, 0, 0}
		};
		ret = ioctl(fd, IOCTL_PRIVCMD_HYPERCALL, &hyper1);

		return ret;
	}

	void get_cr3_hypercall(unsigned long *cr3_list, int &list_size, int fd)
	{
		int ret, i;

		for(i=0; i<list_size; i++)
			cr3_list[i] = 0;

		//Get cr3 from Hypervisor
		privcmd_hypercall_t hyper1 = { 
			__HYPERVISOR_vt_op, 
			{ 3, domID, 0, (__u64)cr3_list, 0}
		};
		ret = ioctl(fd, IOCTL_PRIVCMD_HYPERCALL, &hyper1);

		list_size = cr3_list[0];
		for(i=1; i<=list_size; i++){
			cr3_list[i-1] = cr3_list[i];
		}
		cr3_list[i+1] = 0;
	}
}

/*void total_valid_calculate(unsigned long frame, unsigned long &total_value)
  {
  if(system_map_total_valid.count(frame) == 0){
  system_map_total_valid[frame] = 1;
  total_value++;
  }
  }*/



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

	if(it == table->h.end()){ //not exist
		mapData mapVal;
		mapVal.present_times = valid_bit;
		mapVal.weight = 1;

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
	else{  //already exist
		char &val_ref = table->h[vkey].present_times;	
		table->h[vkey].weight = 1;

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
			add_change_number(val_ref);
			tmp = get_change_number(val_ref);			
			if(tmp >= CHANGE_LIMIT){
				(table->change_page)++;
			}
			ret = 1;
		}
		//swap to non-swap bit
		else if(val==0 && valid_bit==1){
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


	table->count = table->change_page = table->activity_page[0] = table->activity_page[1] =
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

						/*Even gw.l1e==0 also continue*/
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
	table->count = count;
	return count;
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



/*main function*/
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
			list.insert(DATAMAP::value_type(cr3, tmp));
		}
		else{
			struct hash_table &h = list[cr3];
			h.check = 1;
			total_change_page += h.change_page;	
		}
	}
	return total_change_page;
}
unsigned long calculate_all_page(DATAMAP &list, double *result)
{
	DATAMAP::iterator it = list.begin();
	unsigned long check_cr3_num = 0;
	SYSTEM_MAP *system_map;


	while(it != list.end())
	{
		struct hash_table &h = it->second;
		unsigned long cr3 = it->first;
		check_cr3_num++;

		if(h.check == 1){
			HASHMAP::iterator hashIt = h.h.begin();
			while(hashIt != h.h.end()){
				unsigned long paddr;
				byte valid_bit = (hashIt->second.present_times) & 1;
				byte val_ref = hashIt->second.present_times;
				char huge_bit = get_huge_bit(hashIt->second.paddr);
				double weight = hashIt->second.weight;

				if(valid_bit == 0){
					system_map = &system_map_swap;
					paddr = get_swap_id(hashIt->second.paddr);
				}
				else{
					system_map = &system_map_wks;
					paddr = get_paddr(hashIt->second.paddr);
				}


				if(  (get_change_number(val_ref) >= CHANGE_LIMIT && valid_bit == 0) || valid_bit == 1 ){
					/*check if paddr already stored in system_map*/
					if(system_map->count(paddr) > 0){
						(system_map->at(paddr))++;
					}
					else{
						system_map->insert(pair<unsigned long, byte>(paddr, 1));
						if(huge_bit == 0){
							(h.activity_page)[valid_bit] += 1*weight;
						}
						else{
							(h.activity_page)[valid_bit] += 512*weight;
						}
					}
				}

				hashIt->second.weight *= DECAY_FACTOR;
				hashIt++;
			}

			result[0] += h.activity_page[0];
			result[1] += h.activity_page[1];
			result[2] += h.total_valid_pages;
		}

		h.check = 0;
		it++;
	}
	return check_cr3_num;
}
int walk_cr3_list(DATAMAP &list, unsigned long *cr3_list, int list_size,  unsigned long round, struct guest_pagetable_walk &gw)
{
	unsigned long cr3;
	for(int i=0; i<list_size; i++)
	{
		cr3 = cr3_list[i];
		//If there are no struct for this cr3, map will automated assigned new struct
		struct hash_table &h = list[cr3];
		h.cr3 = cr3;
		h.check = 0;
		h.round = round;
		if(cr3 == 0x187000) //skip windows kernel
			continue;
		page_walk_ia32e(cr3, &h, gw);
	}
}
int retrieve_list(DATAMAP &list, unsigned long round)
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

