#ifndef _CONTEXT_
#define _CONTEXT_

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

	current_context -> code(current_context -> args);

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

#endif

