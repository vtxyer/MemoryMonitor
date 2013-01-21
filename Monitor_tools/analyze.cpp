#include <iostream>
#include <csignal>

#include "analyze.h"
#include <set>
using namespace std;


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
	unsigned int round = 0;
	unsigned long cr3_list[30];
	unsigned long result[10];
	struct guest_pagetable_walk gw;
	

	signal(SIGBUS, handler);

	if(argc<3){
		printf("%s domID os_type(linux->0 windows->1)\n", argv[0]);
		exit(1);
	}
	domID = atoi(argv[1]);
	os_type = atoi(argv[2]);

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

//	cr3 = (addr_t)strtoul(argv[1], NULL, 16);
//	fscanf(stdin, "%lx %lx %lx %lx %lx %lx", &cr3_list[0], &cr3_list[1], &cr3_list[2], &cr3_list[3], &cr3_list[4], &cr3_list[5]);
//	fscanf(stdin, "%lx %lx", &cr3_list[0], &cr3_list[1]);
//	list_size = 1;


	while(1){
		get_cr3_hypercall(cr3_list, list_size, fd);
	
//		list_size = 5; //limit to size 5

		system_map_wks.clear();
		system_map_swap.clear();
		system_map_total_valid.clear();
		total_change_page = each_change_page = result[0] = result[1] = result[2] = 0;;

		walk_cr3_list(data_map, cr3_list, list_size, os_type, round, gw);
		each_change_page = check_cr3_list(data_map, cr3_list, list_size);
		calculate_all_page(data_map, os_type, result);

		printf("Invalid Memory:%lu[M] Valid Memory:%lu[M] Total valid Memory:%lu[M] map size:%lu[M] round %d\n\n", 
					result[0]/256, result[1]/256, result[2]/256, data_map[cr3_list[0]].h.size()/(1024*1024), round);
		round++;
		retrieve_list(data_map, round);
		sleep(2);		
	}

	xc_interface_close(xch1);
	xc_interface_close(xch2);
	xc_interface_close(xch3);
	xc_interface_close(xch4);

	return 0;
}

