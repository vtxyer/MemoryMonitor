#include "define.h"
#include <poll.h>


void *receive_event(void*)
{
	struct pollfd poll_fd;
	int port, rc;

	poll_fd.fd = xc_evtchn_fd(xch_event);
	poll_fd.events = POLLIN | POLLERR;

	while(1){
		rc = poll(&poll_fd, 1, -1);
		if(rc == -1){
			fprintf(stderr, "poll exitted with an error");
		}
		if(rc == 1){
			pthread_mutex_lock(&monitor_flag_lock);
			monitor_flag = 2;
			pthread_mutex_unlock(&monitor_flag_lock);

			port = xc_evtchn_pending(xch_event);
			if (port == -1) {
				perror("failed to read port from evtchn");
				exit(EXIT_FAILURE);
			}
			printf("port %d\n", port);
			rc = xc_evtchn_unmask(xch_event, port);
			if (rc == -1) {
				perror("failed to write port to evtchn");
			}	
		}
	}
}

void *sample_usage(void *)
{
	int ret;
	unsigned long touched_num;

	xch_event = xc_evtchn_open(NULL, 0);
	if(xch_event == NULL){
		fprintf(stderr, "xch alloc error\n");
		exit(1);
	}
	ret = xc_evtchn_bind_virq(xch_event, 20);
	if(ret == -1){
		fprintf(stderr, "bind virq error\n");
	}

	ret = pthread_create(&event_thread, NULL, receive_event, NULL);

	while(1){
		lock_pages_hypercall(LOCK_PAGES, hypercall_fd);
		sleep(USAGE_SLEEP);
		pthread_mutex_lock(&monitor_flag_lock);
		if(monitor_flag > 0)
			monitor_flag--;
		pthread_mutex_unlock(&monitor_flag_lock);
	}
}


