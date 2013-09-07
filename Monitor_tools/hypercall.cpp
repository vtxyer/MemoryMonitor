#include "define.h"

extern "C"{
int init_hypercall(int recent_cr3_size, int fd){
	int ret, i;
	unsigned long buff[1];
	buff[0] = LOCK_PAGES_THRESHOLD;
	//Init
	privcmd_hypercall_t hyper1 = { 
		__HYPERVISOR_vt_op, 
		{ 1, domID, recent_cr3_size, (unsigned long)buff, 0}
	};
	ret = ioctl(fd, IOCTL_PRIVCMD_HYPERCALL, &hyper1);

	return ret;
}
int free_hypercall(int fd){
	int ret, i;
	//Free
	privcmd_hypercall_t hyper1 = { 
		__HYPERVISOR_vt_op, 
		{ 2, domID, 0, 0, 0}
	};
	ret = ioctl(fd, IOCTL_PRIVCMD_HYPERCALL, &hyper1);
	return ret;
}
int lock_pages_hypercall(int parts_num, int fd){
	int ret, i;
	privcmd_hypercall_t hyper1 = { 
		__HYPERVISOR_vt_op, 
		{ 4, domID, parts_num, 0, 0}
	};
	ret = ioctl(fd, IOCTL_PRIVCMD_HYPERCALL, &hyper1);

	return ret;
}
unsigned long get_pagesNum_hypercall(int fd){
	privcmd_hypercall_t hyper1 = { 
		__HYPERVISOR_vt_op, 
		{ 1, domID, 0, 0, 0}
	};
	return ioctl(fd, IOCTL_PRIVCMD_HYPERCALL, &hyper1);
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

