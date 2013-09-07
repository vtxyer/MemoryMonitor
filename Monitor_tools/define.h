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
#include <pthread.h>
}
#include <map>
#include <list>
#include <csignal>
using namespace std;

#define CHANGE_LIMIT 1
#define MAX_ROUND_INTERVAL 50 
#define USAGE_SLEEP 30
#define SAMPLE_INTERVAL 3
#define LOCK_PAGES	200 
#define LOCK_PAGES_THRESHOLD 120
#define HIGH_USAGE_STILL_MONITOR_ROUND 2

#define RECENT_CR3_SIZE 300
#define ADDR_MASK 0x0000ffffffffffff


typedef unsigned long addr_t;
typedef unsigned long mfn_t;
typedef char byte;
typedef unsigned long cr3_t;
typedef unsigned long sharedID_t;
typedef unsigned int round_t;
typedef map<unsigned long, struct hash_table> DATAMAP;
typedef map<unsigned long, unsigned long> SYSTEM_MAP;
typedef list<cr3_t> CR3_LIST;

struct mapData
{
	/*
	 * bit 0 -> present bit, bit 1~7 -> change times
	 * */
	byte present_times;
	/*
	 * valid_bit==1 -> [63:63]:bool for is_change_times_change, [62:28]:paddr, [0:0]:huge_bit
	 * valid_bit==0 -> [63:63]:bool for is_change_times_change, [62:28]:paddr, [27:1]:swap_paddr, [0:0]huge_bit
	 * */
	byte pte_node[8];
//	unsigned long paddr;
};
typedef struct mapData mapData;
typedef map<unsigned long, mapData> HASHMAP;

struct hash_table
{
	map<unsigned long, mapData> pte_data; //bit 0=>valid_bit, 1~8 => counter
	unsigned long activity_page[2];//0->invalid, 1->valid
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
};

class Shared_page_node
{
public:
	void add_share_relation(cr3_t target_cr3){
		shared_cr3_list.push_back(target_cr3);
	};
	unsigned long is_page_counted(CR3_LIST &scr3_l){
		CR3_LIST::iterator sIt = scr3_l.begin();
		CR3_LIST::iterator tIt = shared_cr3_list.begin();
		unsigned long rnum = 0;
		while( sIt != scr3_l.end() ) {
			cr3_t check_cr3 = *sIt;
			while(tIt != shared_cr3_list.end()){
				if(check_cr3 == *tIt){
					rnum++;
//					shared_cr3_list.erase(tIt);	
				}
				tIt++;
			}
			sIt++;
		}
		return rnum;
	}
	/*shared cr3 list*/
	CR3_LIST shared_cr3_list;
};

typedef map<sharedID_t, Shared_page_node> SHARED_TREE;
/*valud is cr3 bottleneck page numbers*/
typedef map<cr3_t, unsigned long> CR3_INFO;
class Sampled_data
{
public:
	void set_value(unsigned long _total, unsigned long _swap_count){
		total_bottleneck_pages = _total;
		swap_count_this_times = _swap_count;
	};
	void get_value(unsigned long &_total, unsigned long &_swap_count){
		_total = total_bottleneck_pages;
		_swap_count = swap_count_this_times;
	};
	SHARED_TREE shared_tree;
	CR3_INFO cr3_info;
	map<byte, unsigned int> swap_count_tims_to_num;
private:
	unsigned long total_bottleneck_pages;
	unsigned long swap_count_this_times;
};


/*global variable*/
extern int domID;
extern round_t round;
extern SYSTEM_MAP system_map_wks;
extern map<round_t, Sampled_data> sample_result;
extern xc_interface *xch4, *xch3, *xch2, *xch1, *xch_event;
extern sigjmp_buf sigbuf;
extern unsigned long global_total_change_times;
extern pthread_t event_thread, mem_thread;
extern pthread_mutex_t monitor_flag_lock;
extern int monitor_flag;
extern int hypercall_fd;

extern unsigned long reduce_tot_swap_count;
extern pthread_cond_t cond;

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
void save_paddr(unsigned long *addr, unsigned long val);
void save_swap_paddr(unsigned long *addr, unsigned long val);
void save_huge_bit(unsigned long *addr, unsigned long val);
unsigned long get_vaddr(unsigned long l1offset, unsigned long l2offset, unsigned long l3offset, unsigned long l4offset);
int entry_valid(unsigned long entry);
int pte_entry_valid(unsigned long entry);
void set_change_bit(unsigned long *entry, bool val);
bool is_change_bit_set(unsigned long entry);



/* Hypercall */
extern "C"{
int init_hypercall(int recent_cr3_size, int fd);
int free_hypercall(int fd);
int lock_pages_hypercall(int parts_num, int fd);
void get_cr3_hypercall(unsigned long *cr3_list, int &list_size, int fd);
}


/*high memory usage dection*/
void *sample_usage(void *);

/* Page Walk */
void* map_page(unsigned long pa_base, int level, struct guest_pagetable_walk *gw);
int check_cr3(DATAMAP &list, unsigned long cr3);
void check_cr3_list(DATAMAP &list, unsigned long *cr3_list, int list_size);
/*
 * valid_bit=0 => in swap
 * valid_bit=1 => is valid
 * */
int compare_swap(struct hash_table *table, struct guest_pagetable_walk *gw, unsigned long offset, char valid_bit, char huge_bit);
unsigned long page_walk_ia32e(addr_t dtb, struct hash_table *table, struct guest_pagetable_walk &gw);
int walk_cr3_list(DATAMAP &list, unsigned long *cr3_list, int list_size,  unsigned int round, struct guest_pagetable_walk &gw);
int retrieve_list(DATAMAP &list);
struct page_data{unsigned long owner; unsigned long max_change_times;};
unsigned long calculate_all_page(DATAMAP &list, unsigned long *result);

/*Output*/
void estimate_output(DATAMAP &cr3_data);
