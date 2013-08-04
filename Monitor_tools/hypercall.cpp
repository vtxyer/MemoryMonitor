#include "define.h"

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
int init_em_hypercall(unsigned long *buff, int fd){
	int ret, i;
	//Init
	privcmd_hypercall_t hyper1 = { 
		__HYPERVISOR_vt_op, 
		{ 9, domID, 0, (__u64)buff, 0}
	};
	ret = ioctl(fd, IOCTL_PRIVCMD_HYPERCALL, &hyper1);
	return ret;
}
int add_free_list_hypercall(unsigned long size, unsigned long *host, unsigned long *list, int fd){
	int ret, i;
	//Init
	privcmd_hypercall_t hyper1 = { 
		__HYPERVISOR_vt_op, 
		{ 5, domID, size, (unsigned long)host, (unsigned long)list}
	};
	ret = ioctl(fd, IOCTL_PRIVCMD_HYPERCALL, &hyper1);
	return ret;
}
int map_gfn_hypercall(unsigned long gfn, unsigned long *no_lock_list, int fd){
	int ret, i;
	//Init
	privcmd_hypercall_t hyper1 = { 
		__HYPERVISOR_vt_op, 
		{ 4, domID, gfn, (__u64)no_lock_list, 0}
	};
	ret = ioctl(fd, IOCTL_PRIVCMD_HYPERCALL, &hyper1);
	return ret;
}
}

