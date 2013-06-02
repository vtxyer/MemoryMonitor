#include <iostream>
#include <csignal>

#include "analyze.h"
#include <set>
#include <fstream>
using namespace std;

fstream fin;

void handler(int sig){
//	printf("signal bus error\n");
	siglongjmp(sigbuf, 1);
}


int main(int argc, char *argv[])  
{ 
	int fd, ret, i, list_size;  
	unsigned long cr3, value, os_type, total_change_page, each_change_page;
	unsigned long offset = 0;
	struct hash_table global_hash;
	DATAMAP data_map;
	unsigned long global_round = 0;
	unsigned long cr3_list[RECENT_CR3_SIZE];
	double result[10];
	struct guest_pagetable_walk gw;
	

	signal(SIGBUS, handler);

	if(argc<2){
		printf("%s domID\n", argv[0]);
		exit(1);
	}
	domID = atoi(argv[1]);

	xch1 =  xc_interface_open(0,0,0);
	xch2 =  xc_interface_open(0,0,0);
	xch3 =  xc_interface_open(0,0,0);
	xch4 =  xc_interface_open(0,0,0);
	if(xch1 == NULL || xch2 == NULL ||xch3 == NULL ||xch4 == NULL){
		fprintf(stderr, "xch alloc error\n");
		exit(1);
	}
	fd = open("/proc/xen/privcmd", O_RDWR);  
	if (fd < 0) {  
		perror("open");  
		exit(1);  
	}

	ret = init_hypercall(RECENT_CR3_SIZE, fd);
	if(ret == -1){
		printf("Init environment error\n");
		return -1;
	}
	while(1){
		get_cr3_hypercall(cr3_list, list_size, fd);

//		cr3_list[0] = 0x3bb2c000;
//		list_size = 1; //limit to size 5

		system_map_wks.clear();
		system_map_swap.clear();
	//	system_map_total_valid.clear();
		total_change_page = each_change_page = result[0] = result[1] = result[2] = 0;;

		each_change_page = check_cr3_list(data_map, cr3_list, list_size);
		calculate_all_page(data_map, result);
		printf("InvalidMemory:%lu[M] ValidMemory:%lu[M] Round %d\n\n", 
					result[0]/256, result[1]/256, global_round);
		walk_cr3_list(data_map, cr3_list, list_size, global_round, gw);


		global_round++;
		retrieve_list(data_map, global_round);
		sleep(5);		
	}

	xc_interface_close(xch1);
	xc_interface_close(xch2);
	xc_interface_close(xch3);
	xc_interface_close(xch4);

	return 0;
}


