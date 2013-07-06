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
#include <iostream>
using namespace std;
int fd;

extern "C"{
int lock_hypercall(int domID, unsigned long gfn){
	int ret, i;
	//Init
	privcmd_hypercall_t hyper1 = { 
		__HYPERVISOR_vt_op, 
		{ 1, domID, gfn, 0, 0}
	};
	ret = ioctl(fd, IOCTL_PRIVCMD_HYPERCALL, &hyper1);

	return ret;
}
}

int main(int argc, char **argv){
	int domID;
	unsigned long gfn;

	fd = open("/proc/xen/privcmd", O_RDWR);  
	if (fd < 0) {  
		perror("open");  
		exit(1);  
	}

	printf("input DomID\n");
	scanf("%d", &domID);

	printf("input lock gfn\n");
	scanf("%lx", &gfn);

	lock_hypercall( domID, gfn);

	return 0;
}
