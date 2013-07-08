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

int fd, domID;
extern "C"{
int init_hypercall(int recent_cr3_size, int fd){
	int ret, i;

	//Init
	privcmd_hypercall_t hyper1 = { 
		__HYPERVISOR_vt_op, 
		{ 1, domID, recent_cr3_size, 0, 0}
	};
	ret = ioctl(fd, IOCTL_PRIVCMD_HYPERCALL, &hyper1);

	return ret;
}

void get_cr3_hypercall(unsigned long *cr3_list, int &list_size, int fd){
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

int main(int argc, char **argv){
	int list_size, ret;
	unsigned long cr3_list[300];

	domID = atoi(argv[1]);

	fd = open("/proc/xen/privcmd", O_RDWR);  
	if (fd < 0) {  
		perror("open");  
		exit(1);  
	}

	ret = init_hypercall(300, fd);
	while(1){
		get_cr3_hypercall(cr3_list, list_size, fd);
		printf("cr3 change times in one second: %d\n", list_size);
		list_size = 0;
		sleep(1);
	}


	return 0;
}
