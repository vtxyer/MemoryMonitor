#include "define.h"
#include <set>
#include <fstream>
#include <iostream>
#include <csignal>
using namespace std;

fstream fin;
int domID;
int fd;
int set_extra_page;
unsigned int round;
SYSTEM_MAP system_map_wks;
SYSTEM_MAP system_map_swap;
map<unsigned int, Sampled_data> sample_result;
xc_interface *xch4, *xch3, *xch2, *xch1;
sigjmp_buf sigbuf;


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



int main(int argc, char *argv[])  
{ 
	int ret, i, list_size;  
	unsigned long cr3, value, os_type, total_change_page, each_change_page;
	unsigned long offset = 0;
	struct hash_table global_hash;
	DATAMAP data_map;
	unsigned long cr3_list[RECENT_CR3_SIZE];
	unsigned long result[10];
	struct guest_pagetable_walk gw;
	
	set_extra_page = 0;

	signal(SIGBUS, handler);
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


	int ttt=0; 

//	list_size = file_cr3(cr3_list);

	while(1){
	 	ttt++;
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
					result[0]/256, result[1]/256, round);

		walk_cr3_list(data_map, cr3_list, list_size, round, gw);


		round++;
		retrieve_list(data_map, round);

//		if(ttt==2)
//			break;

		sleep(2);		
	}

	xc_interface_close(xch1);
	xc_interface_close(xch2);
	xc_interface_close(xch3);
	xc_interface_close(xch4);

	return 0;
}


