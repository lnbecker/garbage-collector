//Lauren Becker

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>


int *heap;
int *sentinel;
int *currentBlock=firstBlock();

void  initmemory(int size){
	int numBytes=size+12;
	int remainder;
	if(numBytes%8!=0){
		remainder=8-(numBytes%8);
		numBytes=numBytes+remainder;
	}
	int *begin=(int *)malloc(numBytes);
	heap=begin+1;
	int *end = begin+(numBytes/4);
	sentinel=begin+((numBytes/4)-1);
	*heap=numBytes-8; 
	*sentinel=5;
}



void *myalloc(int length){
	currentBlock=firstBlock();
	int newSize, remainder;
	while(currentBlock!=lastBlock()){
		if(isAllocated(currentBlock)==1){ //if block is allocated
			currentBlock=nextBlock(currentBlock); //move to next block
		}
		else{ //block is not allocated
			if(*currentBlock-4>=length){ //if block user space size is big enough
				if(((length+4)%8)!=0){ //if size of userspace+header is not a multiple of 8
					remainder=(length+4)%8;
					newSize=length+4+(8-remainder); //make it multiple of 8 so next user space is multiple
					if(*currentBlock>=newSize){ //we officially know it's big enough
						if(nextBlock(currentBlock)==currentBlock+(newSize/4)){//Takes up all open slot
							*currentBlock=newSize+1;
							return currentBlock+1; //don't change next header, return user space
						}
						else{
							*(currentBlock+(newSize/4))=(*currentBlock)-newSize; //make next header the old size-size just allocated (do before changing currentBlock size)
							*currentBlock=newSize+1; //new size, odd=allocated
							return currentBlock+1; //return pointer to newly allocated user space
						}
					} //else it's not big enough and we need to move onto next block
				}
				else{ //the malloc is size is good as is, change headers
					newSize=length+4;
					if(nextBlock(currentBlock)==currentBlock+(newSize/4)){  //Takes up all open slot
						*currentBlock=newSize+1;
						return (currentBlock+1); //don't change next header
					}
					else{
						*(currentBlock+(newSize/4))=(*currentBlock)-newSize; //set next block header
						*currentBlock=newSize+1; //new size, odd=allocated
						return currentBlock+1; //return pointer to newly allocated user space
					}
				}
			}
			else{ //currentBlock is not large enough
				currentBlock=nextBlock(currentBlock); //move to next block
			}
		}
	} //after this, currentBlock=sentinal
	
	return NULL; //didn't find space

}


void  myfree(void *ptr){
	int *other=ptr-4;
	if(isAllocated(other))
		*other=(*other)-1;

}




void  coalesce(){
	int *p1=heap;
	int *p2=nextBlock(p1);
	while(p1!=sentinel){
		if(isAllocated(p1)==0 && isAllocated(p2)==0){
			*p1=(*p1)+(*p2);
		}
		else{
			p1=nextBlock(p1);
		}
		p2=nextBlock(p1);
	}
}




void  printallocation(){
	int n=0;
	int *currentBlock=heap;
	int size;
	while(currentBlock!=sentinel){
		printf("Block %d: ", n);
		size=*currentBlock;
		if(isAllocated(currentBlock)==1){
			size=size-1;
		}
		printf(" Size: %d bytes", size);
		if(isAllocated(currentBlock)==1){
			printf("       allocated");
		}
		if(isAllocated(currentBlock)==0){
			printf("       unallocated");
		}
		n++;
		currentBlock=nextBlock(currentBlock);
		printf("\n");
	}
	
}


//NEXT FOUR


void *firstBlock(){
	return heap;

}



void *lastBlock(){
	return sentinel;
}



int isAllocated(int *p){ //pass in address of block or header?
	if((*p)%2 != 0){
		return 1;
	}
	else{
		return 0;
	}
}



int *nextBlock(int *p){
	int size = *p;
	if (size % 2 != 0){
		size=size-1;
	}
	int *nextBlockaddr=p+(size/4);
	return nextBlockaddr;

}
