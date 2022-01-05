#include <stdio.h>
#include "context.h"

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