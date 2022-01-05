#include <stdio.h>
#include "context.h"

context c1;
context c2;
context c3;
context c4;
context c5;
context c6;


void* do_work1(void* args){
	while(1){
		//printf("work1\n");
	}
}

void* do_work2(void* args){
	while(1){
		printf("work2\n");
	}
}

void* do_work3(void* args){
	while(1){
		//printf("work3\n");
	}
}


int main(){
	FCFSScheduler* sched = get_fcfs_scheduler(3);
	context_init(&c1, 1024, do_work1, NULL);
	context_init(&c2, 1024, do_work2, NULL);
	context_init(&c3, 1024, do_work3, NULL);
	
	add_context_to_fcfs_scheduler(sched, &c1);
	add_context_to_fcfs_scheduler(sched, &c2);
	add_context_to_fcfs_scheduler(sched, &c3);
	start_fcfs_scheduler(sched);

	run_schedule(sched);
	
	//switch_current(&c3);
	join_fcfs_scheduler(sched);
	std::cout << "exit\n";

	return 0;
}