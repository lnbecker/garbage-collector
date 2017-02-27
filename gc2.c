#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

void gc();
void mark(void *p);
int isMarked(int *b);
int *thisBlock(void *p);
void sweep();
void *stackBottom();

void gc(){
	int *stackTop;
	int *bottomAddress=(int *) stackBottom();
	int **rootScanner=&stackTop;
	while(rootScanner<bottomAddress){
		mark(*rootScanner);
		rootScanner=rootScanner+1;
	}
	sweep();
}


void mark(void *p){
	int *heapStart = firstBlock();
	int *heapEnd = lastBlock();
	if((int *)p<heapStart || (int *)p>heapEnd){ //if p is not in heap
		return;
	}
	int *b = thisBlock(p);
	if(isAllocated(b) && isMarked(b)==0){
		*b=*b+2; //mark the block
		int *p2=b+1; 
		while(p2<nextBlock(b)){ //for each pointer p2 in b
			mark(p2); //mark p2
			p2=p2+1; //go to next pointer
		}
	}
}

int isMarked(int *b){
	if(*b % 4 == 3)
		return 1;
	else
		return 0;
}

int *thisBlock(void *p){
	int *blockScanner = firstBlock();
	while(blockScanner!=lastBlock()){
		if((int *)p>blockScanner && (int *)p<nextBlock(blockScanner)){
			return blockScanner; //returns header to block
		}
		else{
			blockScanner=nextBlock(blockScanner);
		}
	}
}

void sweep(){
	int *ptr=firstBlock();
	while(ptr != lastBlock()){
		if(isMarked(ptr)==0){ //if ptr is not marked
			myfree(ptr+1); //deallocate ptr
		}
		else{
			*ptr=*ptr-2; //unmark ptr
		}
		ptr = nextBlock(ptr);
	}
	coalesce();
}




void *stackBottom() {
	unsigned long bottom;
    FILE *statfp = fopen("/proc/self/stat", "r");
    fscanf(statfp,
           "%*d %*s %*c %*d %*d %*d %*d %*d %*u "
           "%*u %*u %*u %*u %*u %*u %*d %*d "
           "%*d %*d %*d %*d %*u %*u %*d "
           "%*u %*u %*u %lu", &bottom);
    fclose(statfp);
	return (void *) bottom;
}

