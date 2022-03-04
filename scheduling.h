#ifndef _SCHEDULERS_
#define _SCHEDULERS_

#include "context.h"

#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

extern context sched_ctx;

struct context;

struct FCFSScheduler{
	context** contexts;//pointer to context stored here AND in a place he comes from!
	uint32_t capacity;
	uint32_t amount;
	pthread_t* scheduler_thread;
	int32_t curr;
};

void* scheduler_switch(void* args);
void* scheduler_switch_starter(void* args);


FCFSScheduler* get_fcfs_scheduler(uint32_t capacity_in){
	FCFSScheduler* sched = (FCFSScheduler*)malloc(sizeof(FCFSScheduler));
	sched -> capacity = capacity_in;
	sched -> amount = 0;
	sched -> curr = 0;

	sched -> scheduler_thread = (pthread_t*)malloc(sizeof(pthread_t));
	sched -> contexts = (context**)malloc(sizeof(context*) * capacity_in);
	
	return sched;
}

void start_fcfs_scheduler(FCFSScheduler* sched){
	printf("its okay in starter\n");
	pthread_create((sched -> scheduler_thread), NULL, scheduler_switch_starter, (void*)(sched));
}

void join_fcfs_scheduler(FCFSScheduler* sched){
	pthread_join(*(sched->scheduler_thread), NULL);
}

void add_context_to_fcfs_scheduler(FCFSScheduler* sched, context* ctx){
	if(sched -> capacity == sched -> amount)
		return;
	sched -> contexts[sched -> amount] = ctx;
	sched -> amount++;
}


uint32_t calc_amount_of_fcfs_sched(FCFSScheduler* sched){
	if(!sched)
		return 0;

	uint32_t len = 0;
	for(uint32_t i = 0; i < sched -> capacity; ++i){
		if(sched -> contexts[i] != NULL){
			len++;
		}
	}

	return len;
}

void kill_context(FCFSScheduler* sched, int num){
    context* dead_ctx = sched -> contexts[num];
    sched -> contexts[num] = NULL;
}

void* scheduler_switch_starter(void* args){
    switch_current(&sched_ctx);
    return NULL;//will never return
}

void* scheduler_switch(void* args){
    printf("address is, %p", args);
	FCFSScheduler* sched = (FCFSScheduler*)args;
	printf("its okay in scheduler\n");

	while(1){
		//sleep(3);
		uint32_t count = calc_amount_of_fcfs_sched(sched);
		printf("%d, %d\n", count, sched -> curr);
       
		if(count == 0 || (sched -> curr) >= count)
			break;
		
		if((sched -> contexts[sched -> curr] != NULL)){
			printf("i -> %d\n", sched -> curr);

            context* choosen_ctx = sched -> contexts[sched -> curr];
            switch_current(choosen_ctx);
            
            if(sched -> curr < count)
                sched -> curr += 1;
            else
                sched -> curr = 0;
		}
        
   }

	return NULL;
}

void run_schedule(FCFSScheduler* sched){
	switch_current(sched -> contexts[sched -> curr]);	
}

#endif

