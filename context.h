#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

enum context_state{
	ctx_init,
	ctx_exeq,
	ctx_dead
};

typedef void* (*functor) (void*);

struct context{
	void* rbp;
	void* rsp;
	void* base;

	context_state state;

	functor code;
	void* args;
};

context* current_context = NULL;

void context_init(context* ctx, size_t stack_size, functor f, void* args_){
	ctx -> base = malloc(stack_size);
	ctx -> code = f;
	ctx -> args = args_;

	ctx -> state = ctx_init;

	ctx->rsp = (int*)(ctx -> base) + stack_size - 4;

	ctx -> rbp = ctx -> rsp;
}

void start_current_ctx(){
	current_context -> state = ctx_exeq;

	current_context -> code(NULL);

	current_context -> state = ctx_dead;
	//std::cout << "the end\n";
}

void switch_current(context* to){
	if (current_context)
    	asm ("movq %%rsp, %0" "\n\t"
    		 "movq %%rbp, %1"
    			: "=r"(current_context->rsp) , "=r"(current_context->rbp)
    			: );

	current_context = to;
  
  	asm("movq %0, %%rsp" "\n\t"
  		"movq %1, %%rbp"
      		:
      		:"r"(current_context->rsp), "r"(current_context->rbp));

	if (current_context->state == ctx_init)
		start_current_ctx();
	std::cout << "swap\n";
}

struct FCFSScheduler{
	context** contexts;//pointer to context stored here AND in a place he comes from!
	uint32_t capacity;
	uint32_t amount;
	pthread_t* scheduler_thread;
	int32_t curr;
};

void* scheduler_switch(void* args);

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
	pthread_create((sched -> scheduler_thread), NULL, scheduler_switch, (void*)(sched));
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

//take thread-caller registers and change teir IP to another context;
//args: sched, rbp, rsp, base;
void* scheduler_switch(void* args){
	FCFSScheduler* sched = (FCFSScheduler*)args;
	printf("its okay in scheduler\n");
	int32_t ready[sched-> capacity];
	while(1){
		sleep(3);
		//USE MUTEX HERE
		uint32_t count = calc_amount_of_fcfs_sched(sched);
		printf("%d, %d\n", count, sched -> curr);
		
		if(count == 0)
			break;
		
		for(uint32_t i = 0; i < sched -> capacity; ++i){
			if((sched -> contexts[i] != NULL) && ready[i]){
				sched -> curr = i;
				ready[i] = 0;
				printf("i -> %d\n", i);
				//sched -> contexts[i] = NULL;
				break;
				//switch_current(sched -> contexts[i]);
			}
		}
	}

	return NULL;
}

void run_schedule(FCFSScheduler* sched){
	switch_current(sched -> contexts[sched -> curr]);	
}