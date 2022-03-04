#include <stdio.h>
#include "context.h"
#include "scheduling.h"

context c1;
context c2;
context c3;
context c4;
context c5;
context c6;


context sched_ctx;
FCFSScheduler* global_ptr_to_curr_scheduler;

void* do_work1(void* args){
	while(1){
		printf("work1\n");
        printf("work1\n");
        switch_current(&sched_ctx);
	}
}

void* do_work2(void* args){
    int i = 0;
	while(i < 10){
        i++;
		printf("work2\n");
	}
    switch_current(&sched_ctx);
    return 0;
}

void* do_work3(void* args){
	int suicide_time = 10;
    int i = 0;
    while(1){
        i++;
		printf("work3\n");
        if(i == suicide_time){
           // kill_context(global_ptr_to_curr_scheduler, 2);
            switch_current(&sched_ctx);
        }
	}
}


int main(){
	FCFSScheduler* sched = get_fcfs_scheduler(3);
    global_ptr_to_curr_scheduler = sched;
	context_init(&c1, 1024, do_work1, NULL);
	context_init(&c2, 1024, do_work2, NULL);
	context_init(&c3, 1024, do_work3, NULL);
	context_init(&sched_ctx, 1024, scheduler_switch, global_ptr_to_curr_scheduler);

	add_context_to_fcfs_scheduler(sched, &c1);
	add_context_to_fcfs_scheduler(sched, &c2);
	add_context_to_fcfs_scheduler(sched, &c3);
	start_fcfs_scheduler(sched);

	//run_schedule(sched);
    
    while(1){
        sleep(1);
	    //printf("hui\n");
    }
    printf("main work ends here\n");

	//switch_current(&c3);
	join_fcfs_scheduler(sched);
	std::cout << "exit\n";

	return 0;
}
