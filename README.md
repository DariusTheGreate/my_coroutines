## My coroutines, written in C

### Architecture:
 - there is context structure, that holds rbp, rsp registers and function pointer to executable code.
 - also you can use is via scheduler structures (but its half broken for now)

### Usage:
- [x] as coroutines:

```
context c1;
context c2;

void* foo(void* args){
	int i = 0;
	while(1){
		i++;
		if((i)%6==0)
			switch_current(&c2);
		//std::cout << "foo\n";	
		printf("foo\n");
	}
}

void* boo(void* args){
	int i = 0;
	while(1){
		i++;
		if((i)%6==0)
			switch_current(&c1);
		//std::cout << "boo\n";	
		printf("boo\n");
	}
}

int main(){
	context_init(&c1, 1024, foo, NULL);
	context_init(&c2, 1024, boo, NULL);
	switch_current(&c1);
	//std::cout << "exit\n";
	return 0;
}

```
- [x] as exceptions:

```
context c1;
context c2;

int* dangerous_data;
int data_processed = 0;

void* do_dangerous_work(void* args){
	while(1){
		data_processed++;
		if(dangerous_data[data_processed] != 15)
			switch_current(&c2);//throw an exception and go into exceptions handler
		std::cout << "boo\n";	
		printf("boo\n");
	}
}

void* exception_of_dangerous_work(void* args){
	printf("EXCEPTION AAAA!! data data_processed is --> %d\n", data_processed);
	exit(0);
}

int main(){
    dangerous_data = (int*) malloc(10 * sizeof(int));
    for(int i = 0; i < 10; ++i){
        dangerous_data[i] = 15;
    }
	context_init(&c1, 1024, do_dangerous_work, NULL);
	
	context_init(&c2, 1024, exception_of_dangerous_work, NULL);
	
	switch_current(&c1);
	//std::cout << "exit\n";

	free(dangerous_data);
	return 0;
}

```
- [ ] as scheduling entity

```
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
}

```
