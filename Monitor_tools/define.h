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

#define CHANGE_LIMIT 2
#define MAX_ROUND_INTERVAL 999
#define ADDR_MASK 0x0000ffffffffffff
#define RECENT_CR3_SIZE 50

#define EM_WAIT_INVALID 20
#define EM_WAIT_SWAP_OUT 5
#define EM_WAIT_RESTORING 50

typedef unsigned long addr_t;
typedef unsigned long mfn_t;
typedef char byte;
typedef map<unsigned long, struct hash_table> DATAMAP;
typedef map<unsigned long, unsigned long> SYSTEM_MAP;

struct mapData
{
	/*
	 * bit 0 -> present bit, bit 1~7 -> change times
	 * */
	byte present_times;

	/*
	 * valid_bit==1 -> [63:63]:bool for change_times add, [62:28]:paddr, [0:0]:huge_bit
	 * valid_bit==0 -> [63:63]:bool for change_times add, [62:28]:paddr, [27:1]:swap_paddr, [0:0]huge_bit
	 * */
	unsigned long paddr;
};
typedef struct mapData mapData;
typedef map<unsigned long, mapData> HASHMAP;

struct hash_table
{
	map<unsigned long, mapData> h; //bit 0=>valid_bit, 1~8 => counter
	unsigned long cr3;
	unsigned long change_page, total_valid_pages;
	unsigned long activity_page[2];//0->invalid, 1->valid
	unsigned int round;
	unsigned long start_round, end_round;
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

	unsigned long l4offset;            /* Guest's level 4 entry */
	unsigned long l3offset;            /* Guest's level 3 entry */
	unsigned long l2offset;            /* Guest's level 2 entry */
	unsigned long l1offset;            /* Guest's level 1 entry (or fabrication) */
};

class Sampled_process_node
{
public:
	void add_share_relation(unsigned long target_cr3){
		if(share_node.count(target_cr3) == 0 ){
			share_node.insert(std::pair<unsigned long, unsigned long>(target_cr3, 1));
		}
		else{
			unsigned long size = share_node[target_cr3];
			size += 1;
			share_node.insert(std::pair<unsigned long, unsigned long>(target_cr3, size));
		}
	};

private:
	map<unsigned long, unsigned long> share_node;
};
class Sampled_data
{
public:
	void set_value(unsigned long _total, unsigned long _change_times){
		total_bottleneck_size = _total;
		total_change_times = _change_times;
	};
	void add_process(unsigned long _cr3){
		Sampled_process_node process_node;
		process_node_ptr.insert(std::pair<unsigned long, Sampled_process_node>(_cr3, process_node));
	};
	void add_share(unsigned long cr3_1, unsigned long cr3_2){
		Sampled_process_node &process_node = process_node_ptr[cr3_1];
		process_node.add_share_relation(cr3_2);
	};

private:
	unsigned long total_bottleneck_size;	
	unsigned long total_change_times;
	map<unsigned long, Sampled_process_node> process_node_ptr;
};


/*global variable*/
extern int domID;
extern int fd;
extern unsigned int round;
extern SYSTEM_MAP system_map_wks;
extern SYSTEM_MAP system_map_swap;
extern map<unsigned int, Sampled_data> sample_result;
extern xc_interface *xch4, *xch3, *xch2, *xch1;
extern sigjmp_buf sigbuf;

extern int set_extra_page;


/* BitManage */
//1~7 bits represent change number
int add_change_number(byte &value);
unsigned long get_change_number(byte value);
unsigned long get_bit(unsigned long entry, int num, int position);
void clear_bit(unsigned long &addr, int start, int end);
int page_size_flag (uint64_t entry);
int entry_present(uint64_t entry);
int get_access_bit(uint64_t entry);
int get_huge_bit(uint64_t entry);
unsigned long get_paddr(uint64_t addr);
unsigned long get_swap_id(uint64_t addr);
void save_paddr(unsigned long &addr, unsigned long val);
void save_swap_paddr(unsigned long &addr, unsigned long val);
void save_huge_bit(unsigned long &addr, unsigned long val);
unsigned long get_vaddr(unsigned long l1offset, unsigned long l2offset, unsigned long l3offset, unsigned long l4offset);
int entry_valid(unsigned long entry);
int pte_entry_valid(unsigned long entry);
void set_change_bit(unsigned long &entry, bool val);
bool change_bit_set(unsigned long entry);



/* Hypercall */
extern "C"{
int init_hypercall(int recent_cr3_size, int fd);
void get_cr3_hypercall(unsigned long *cr3_list, int &list_size, int fd);
int lock_gfn_hypercall(unsigned long gpa, int fd);
int init_em_hypercall(unsigned long *buff, int fd);
int add_free_list_hypercall(unsigned long size, unsigned long *host, unsigned long *list, int fd);
int start_mapping_hypercall(unsigned long gpa, unsigned long *no_lock_list, int fd);
}


/*Communate*/
int set_free_list(char *ip, int port, unsigned long host_id, unsigned long host_cr3, int fd);


/* Page Walk */
void* map_page(unsigned long pa_base, int level, struct guest_pagetable_walk *gw);
int check_cr3(DATAMAP &list, unsigned long cr3);
unsigned long check_cr3_list(DATAMAP &list, unsigned long *cr3_list, int list_size);
/*
 * valid_bit=0 => in swap
 * valid_bit=1 => is valid
 * */
int compare_swap(struct hash_table *table, struct guest_pagetable_walk *gw, unsigned long offset, char valid_bit, char huge_bit);
unsigned long page_walk_ia32e(addr_t dtb, struct hash_table *table, struct guest_pagetable_walk &gw);
int walk_cr3_list(DATAMAP &list, unsigned long *cr3_list, int list_size,  unsigned int round, struct guest_pagetable_walk &gw);
int retrieve_list(DATAMAP &list, unsigned int round);
struct page_data{unsigned long owner; unsigned long max_change_times;};
unsigned long calculate_all_page(DATAMAP &list, unsigned long *result);

