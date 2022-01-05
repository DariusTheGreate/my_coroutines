#include <stdio.h>
#include "context.h"

context c1;
context c2;

int* dangerous_data;
int data_processed = 0;

void* do_dangerous_work(void* args){
	while(1){
		data_processed++;
		if(dangerous_data[data_processed] == 15)
			switch_current(&c2);//throw an exception and go into exceptions handler
		//std::cout << "boo\n";	
		printf("boo\n");
	}
}

void* exception_of_dangerous_work(void* args){
	printf("EXCEPTION AAAA!! data data_processed is --> %d\n", data_processed);
	exit(0);
}

int main(){
	context_init(&c1, 1024, do_dangerous_work, NULL);
	
	context_init(&c2, 1024, exception_of_dangerous_work, NULL);
	
	switch_current(&c1);
	//std::cout << "exit\n";

	//free(dangerous_data);
	return 0;
}