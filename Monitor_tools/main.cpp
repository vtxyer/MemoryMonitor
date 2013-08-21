#include "define.h"
#include <set>
#include <fstream>
#include <iostream>
#include <csignal>
using namespace std;

fstream fin;
int domID;
round_t round;
SYSTEM_MAP system_map_wks;
map<round_t, Sampled_data> sample_result;
xc_interface *xch4, *xch3, *xch2, *xch1, *xch_event;
sigjmp_buf sigbuf;
unsigned long global_total_change_times;
pthread_mutex_t monitor_flag_lock;
pthread_t event_thread, mem_thread;
int monitor_flag;
int hypercall_fd;

unsigned long reduce_tot_swap_count;

round_t END_ROUND = 9000;	


void handler(int sig){
//	printf("signal bus error\n");
	siglongjmp(sigbuf, 1);
}


int file_cr3(unsigned long *cr3_list){
	fin.open("bb", ios::in);
	int num;
	num = 0;
	string str;
	while(getline(fin, str)){
		cr3_list[num] = strtol(str.c_str(), NULL, 16);
		num++;
	}

	return num;
}

void refund()
{
	xc_interface_close(xch1);
	xc_interface_close(xch2);
	xc_interface_close(xch3);
	xc_interface_close(xch4);
	pthread_cancel(mem_thread);
	pthread_cancel(event_thread);
	pthread_join(mem_thread, NULL);
	pthread_join(event_thread, NULL);
	printf("Stop monitor\n");
	exit(1);
}
void over_handler(int sig){
	END_ROUND = 0;
	monitor_flag = 2;
}


void calculate_size(DATAMAP data_map)
{
	unsigned long size = 0;
	DATAMAP::iterator it = data_map.begin();
	while(it != data_map.end()){
		unsigned long p_size;
		unsigned long p_num = it->second.pte_data.size();
		p_size = (16 * p_num);
		size += p_size;
		printf("sizeof(struct mapData):%lu pte_data.size:%lu p_size:%lu[M]\n", 
					sizeof(struct mapData), it->second.pte_data.size(), p_size/256);
		it++;
	}
	printf("total size: %lu[M]\n", size/256);
}
int remove_redundant(cr3_t *cr3_list, int list_size)
{
	cr3_t new_cr3_list[9999];
	int new_size = 0;
	for(int i=0; i<list_size; i++){
		cr3_t now_cr3 = cr3_list[i];
		if(now_cr3 == 0 ){
			continue;
		}
		bool dflag = false;
		for(int k=i+1; k<list_size; k++){
			if(now_cr3 == cr3_list[k]){
				cr3_list[k] = 0;
				dflag = true;
			}
		}
		new_cr3_list[new_size] = now_cr3;
		new_size++;
	}
	for(int i=0; i<list_size; i++){
		if(i < new_size){
			cr3_list[i] = new_cr3_list[i];
		}
		else	
			cr3_list[i] = 0;
	}
	return new_size;
}


int main(int argc, char *argv[])  
{ 
	int ret, i, list_size;  
	unsigned long cr3, value, os_type, total_change_page;
	unsigned long offset = 0;
	struct hash_table global_hash;
	DATAMAP data_map;
	unsigned long cr3_list[RECENT_CR3_SIZE];
	unsigned long result[10];
	struct guest_pagetable_walk gw;
	
	global_total_change_times = 0;
	reduce_tot_swap_count = 0;

	signal(SIGBUS, handler);
	signal(SIGINT, over_handler);

	if(argc<2){
		printf("%s domID\n", argv[0]);
		exit(1);
	}
	domID = atoi(argv[1]);
	round = 0;
	xch1 =  xc_interface_open(0,0,0);
	xch2 =  xc_interface_open(0,0,0);
	xch3 =  xc_interface_open(0,0,0);
	xch4 =  xc_interface_open(0,0,0);
	if(xch1 == NULL || xch2 == NULL ||xch3 == NULL ||xch4 == NULL){
		fprintf(stderr, "xch alloc error\n");
		exit(1);
	}
	hypercall_fd = open("/proc/xen/privcmd", O_RDWR);  
	if (hypercall_fd < 0) {  
		perror("open");  
		exit(1);  
	}

	ret = init_hypercall(RECENT_CR3_SIZE, hypercall_fd);
	if(ret == -1){
		printf("Init environment error\n");
		return -1;
	}

	/***********************Sample memory usage thread*******************************/
	pthread_mutex_t monitor_flag_lock = PTHREAD_MUTEX_INITIALIZER;
	monitor_flag = 0;
	pthread_create(&mem_thread, NULL, sample_usage, NULL);
	/***********************Sample memory usage thread*******************************/

	int ttt=0; 
	unsigned long tmp_max_mem = 0;
//	list_size = file_cr3(cr3_list);


//	while(1){
	while(END_ROUND-- > 0){
	 	ttt++;
		get_cr3_hypercall(cr3_list, list_size, hypercall_fd);
	    list_size = remove_redundant(cr3_list, list_size);	

//		cr3_list[0] = 0x3bb2c000;
//		list_size = 1; //limit to size 5

		system_map_wks.clear();
		total_change_page = result[0] = result[1] = result[2] = 0;;


		check_cr3_list(data_map, cr3_list, list_size);

		calculate_all_page(data_map, result);


		if(tmp_max_mem < result[0]){
			tmp_max_mem = result[0];
		}
//		calculate_size(data_map);
//		printf("Max:%lu[M] ChangeTimes:%lu BottleneckMemory:%lu[M] ValidMemory:%lu[M] Round %d\n\n", 
//					tmp_max_mem/256, global_total_change_times, result[0]/256, result[1]/256, round);
		printf("Max:%lu[M] ChangeTimes:%lu BottleneckMemory:%lu[M] Round %d\n\n", 
					tmp_max_mem/256, global_total_change_times, result[0]/256, round);

		walk_cr3_list(data_map, cr3_list, list_size, round, gw);


		round++;
		retrieve_list(data_map);

		do{
			sleep(SAMPLE_INTERVAL);		
//		}while(monitor_flag == 0);
		}while(0);
	}

	estimate_output(data_map);	

	refund();
	return 0;
}


