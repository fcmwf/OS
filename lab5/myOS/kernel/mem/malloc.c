#include "../../include/mem.h"

unsigned long Mymalloc(unsigned long size){
    return dPartitionAlloc(pMemHandler,size);
}

unsigned long free(unsigned long start){
    return dPartitionFree(pMemHandler,start);
}