// #include "../../include/myPrintk.h"
#include "../../include/myPrintk.h"

#define FALSE 0
#define TRUE  1
//dPartition 是整个动态分区内存的数据结构
typedef struct dPartition{
	unsigned long size;
	unsigned long firstFreeStart; 
} dPartition;	//共占8个字节

#define dPartition_size ((unsigned long)0x8)

void showdPartition(struct dPartition *dp){
	myPrintk(0x5,"dPartition(start=0x%x, size=0x%x, firstFreeStart=0x%x)\n", dp, dp->size,dp->firstFreeStart);
}

// EMB 是每一个block的数据结构，userdata可以暂时不用管。
typedef struct EMB{
	unsigned long size;
	unsigned long is_allocated;
	union {
		unsigned long nextStart;    // if free: pointer to next block
         unsigned long userData;		// if allocated, belongs to user
	};	   
	unsigned long preStart;                        
} EMB;	//共占D个字节

#define EMB_size ((unsigned long)0x10)

void showEMB(struct EMB * emb){
	myPrintk(0x3,"EMB(start=0x%x, size=0x%x, nextStart=0x%x)\n", emb, emb->size, emb->nextStart);
}

unsigned long dPartitionInit(unsigned long start, unsigned long totalSize){
	// TODO
	/*功能：初始化内存。
	1. 在地址start处，首先是要有dPartition结构体表示整个数据结构(也即句柄)。
	2. 然后，一整块的EMB被分配（以后使用内存会逐渐拆分），在内存中紧紧跟在dP后面，然后dP的firstFreeStart指向EMB。
	3. 返回start首地址(也即句柄)。
	注意有两个地方的大小问题：
		第一个是由于内存肯定要有一个EMB和一个dPartition，totalSize肯定要比这两个加起来大。
		第二个注意EMB的size属性不是totalsize，因为dPartition和EMB自身都需要要占空间。
	*/
	// dPartition* dp =(dPartition*)start;
	// dp->firstFreeStart = start+dPartition_size;
	// dp->size = totalSize - dPartition_size;
	// EMB *block = (EMB *)dp->firstFreeStart;
	// block->size = totalSize - dPartition_size - EMB_size;
	// block->nextStart = 0;
	// block->is_allocated = TRUE;
	// block->preStart = start;
	// myPrintk(0x3,"addr:EMB(start=0x%x, size=0x%x, allocated=0x%x,nextStart=0x%x,prestart=0x%x)\n", block, &block->size, &block->is_allocated,&block->nextStart,&block->preStart);
	//myPrintk(0x7,"start=%x,size=%x\n",start,totalSize);
	dPartition dp  = {totalSize - dPartition_size, start+dPartition_size};
	*(dPartition*)start = dp;
	EMB		   emb = {totalSize - dPartition_size - EMB_size,FALSE,0,start};
	*(EMB*)(start + dPartition_size) =  emb;
	//myPrintk(0x3,"addr:EMB(start=0x%x, size=0x%x, allocated=0x%x,nextStart=0x%x,prestart=0x%x)\n", start + dPartition_size, (*(EMB*)(start + dPartition_size)).size, (*(EMB*)(start + dPartition_size)).is_allocated,(*(EMB*)(start + dPartition_size)).nextStart,(*(EMB*)(start + dPartition_size)).preStart);
	return start;
}


void dPartitionWalkByAddr(unsigned long dp){
	// TODO
	/*功能：本函数遍历输出EMB 方便调试
	1. 先打印dP的信息，可调用上面的showdPartition。
	2. 然后按地址的大小遍历EMB，对于每一个EMB，可以调用上面的showEMB输出其信息
	*/
	showdPartition((dPartition*)dp);
	unsigned long p;
	for(p=(*(dPartition*)dp).firstFreeStart; p ; p=(*(EMB*)p).nextStart){
		showEMB((EMB*)p);
	}
}


//=================firstfit, order: address, low-->high=====================
/**
 * return value: addr (without overhead, can directly used by user)
**/

unsigned long dPartitionAllocFirstFit(unsigned long dp, unsigned long size){
	// TODO
	/*功能：分配一个空间
	1. 使用firstfit的算法分配空间，
	2. 成功分配返回首地址，不成功返回0
	3. 从空闲内存块组成的链表中拿出一块供我们来分配空间(如果提供给分配空间的内存块空间大于size，我们还将把剩余部分放回链表中)，并维护相应的空闲链表以及句柄
	注意的地方：
		1.EMB类型的数据的存在本身就占用了一定的空间。
	*/
	unsigned long p = (*(dPartition*)dp).firstFreeStart;
	while(p){
		if(((EMB*)p)->is_allocated==FALSE && ((EMB*)p)->size >= (size+EMB_size))
			break;
		p = ((EMB*)p)->nextStart;
	}
	if(!p)
		return 0x0; //空间不足
	else if( (!((EMB*)p)->nextStart) &&((EMB*)p)->size>=size+EMB_size){
		//位于链表末尾
		unsigned long remained_mem_tail = (*(EMB*)p).size-size-EMB_size;
		EMB* pre = (EMB*)p;
		EMB* next;
		EMB  new_emb_tail = {remained_mem_tail, FALSE,0,p};
		next  = (EMB*)((char*)pre + size + EMB_size);
		*next = new_emb_tail;
		pre->is_allocated = TRUE; 
		pre->nextStart = (unsigned long)next;
		pre->size = size;
	}
	else{
		//位于链表中间
		EMB* pre = (EMB*)p;
		EMB* next;
		next  = (EMB*)((char*)pre + size + EMB_size);
		unsigned long remained_mem_middle = pre->size - size - EMB_size;
		EMB new_emb_middle = {remained_mem_middle,FALSE,pre->nextStart,(unsigned long)pre};
		*next = new_emb_middle;
		pre->is_allocated = TRUE;
		((EMB*)pre->nextStart)->preStart = (unsigned long)next;
		pre->nextStart = (unsigned long)next;
		pre->size = size;
	}
	return p+EMB_size;
	//位于中间和尾端的情况部分好像可以合并？不过我懒得合并了...
}

unsigned long find_start(unsigned long dp,unsigned long start){
	unsigned long emb = ((dPartition*)dp)->firstFreeStart;
	while(emb){
		if(emb == start) return 1;
		emb = ((EMB*)emb)->nextStart;
	}
	return 0;
}

unsigned long dPartitionFreeFirstFit(unsigned long dp, unsigned long start){
	// TODO
	/*功能：释放一个空间
	1. 按照对应的fit的算法释放空间
	2. 注意检查要释放的start~end这个范围是否在dp有效分配范围内
		返回1 没问题
		返回0 error
	3. 需要考虑两个空闲且相邻的内存块的合并
	*/
	EMB* pre;
	EMB* middle;
	EMB* next;  
	start -= sizeof(EMB);
	if(!find_start(dp,start)) return 0;  //未找到start对应的EMB块，结束程序
	middle = (EMB*)start;
	middle->is_allocated = FALSE;
	pre  = (EMB*)(middle->preStart);
	next = (EMB*)(middle->nextStart);
	//前后链表均可合并
	if((unsigned long)pre!=dp&&(unsigned long)next){
		if(!pre->is_allocated&&!next->is_allocated){
			pre->size += EMB_size*2 + middle->size + next->size;
			pre->nextStart = next->nextStart;
			if( next->nextStart)
				((EMB*)next->nextStart)->preStart = (unsigned long)pre;
			return 1;
		}
	}

	//判断与上一链表是否能合并并处理
	if((unsigned long)pre!=dp)
		if(pre->is_allocated == FALSE){
			pre->size += middle->size + EMB_size;
			pre->nextStart = middle->nextStart; //当middle为尾端的时候也成立
			if(middle->nextStart)
				((EMB*)(middle->nextStart))->preStart = (unsigned long)pre;
		}
	//判断与下一节点能否合并
	if(middle->nextStart){
		next = (EMB*)(middle->nextStart);
		if(next->is_allocated == FALSE){
			middle->size += next->size + EMB_size;
			middle->nextStart = next->nextStart;
			if(((EMB*)next->nextStart)->nextStart)
				( (EMB*)(((EMB*)next->nextStart)->nextStart))->preStart = (unsigned long)middle;
		}
	}
	return 1;
}

// 进行封装，此处默认firstfit分配算法，当然也可以使用其他fit，不限制。
unsigned long dPartitionAlloc(unsigned long dp, unsigned long size){
	return dPartitionAllocFirstFit(dp,size);
}

unsigned long dPartitionFree(unsigned long	 dp, unsigned long start){
	return dPartitionFreeFirstFit(dp,start);
}
