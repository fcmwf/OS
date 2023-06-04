#include "../../include/myPrintk.h"
#include "../../include/mem.h"
unsigned long pMemStart;  // 可用的内存的起始地址
unsigned long pMemSize;  // 可用的大小
unsigned long pMemHandler;
void memTest(unsigned long start, unsigned long grainSize){
	// TODO
	/*功能：检测算法
		这一个函数对应实验讲解ppt中的第一大功能-内存检测。
		本函数的功能是检测从start开始有多大的内存可用，具体算法参照ppt检测算法的文字描述
	注意点三个：
	1、覆盖写入和读出就是往指针指向的位置写和读，不要想复杂。
	  (至于为什么这种检测内存的方法可行大家要自己想一下)
	2、开始的地址要大于1M，需要做一个if判断。
	3、grainsize不能太小，也要做一个if判断
	*/
	unsigned short *p,*q;
	short data_ori;
	int flag ;
	pMemStart = 0;
	pMemSize  = 0;
	flag = 0;
	if(grainSize < 8*sizeof(unsigned long)) grainSize = 8*sizeof(unsigned long);
	if(start<=0x10000)
		start  = 0x10001;
	for(p=(unsigned short*)start; ;p+=grainSize/sizeof(unsigned short)){
		q = p + grainSize/sizeof(unsigned short) - 1;
		//头两个字节检查
		data_ori = *p;
		*p = 0xAA55;
		if(*p!=0xAA55) flag = 1;
		*p = 0x55AA;
		if(*p!=0x55AA) flag = 1;
		*p = data_ori;
		//末两个字节检查
		data_ori = *q;
		*q = 0x55AA;
		if(*q!=0x55AA) flag = 1;
		*q = 0x55AA;
		if(*q!=0x55AA) flag = 1;
		*q = 0x55AA;
		if(flag&&!pMemSize) continue;
		if(flag&&pMemSize)  break;
		if(!flag&&!pMemSize) pMemStart = (unsigned long)p;
		pMemSize = (unsigned long)p - pMemStart + grainSize; //maybe bug!
	}
	myPrintk(0x7,"MemStart: %x  \n", pMemStart);
	myPrintk(0x7,"MemSize:  %x  \n", pMemSize);
}

extern unsigned long _end;
void pMemInit(void){
	unsigned long _end_addr = (unsigned long) &_end;
	memTest(0x100000,0x1000);
	myPrintk(0x7,"_end:  %x  \n", _end_addr);
	if (pMemStart <= _end_addr) {
		pMemSize -= _end_addr - pMemStart;
		pMemStart = _end_addr;
	}
	//对齐
	pMemStart = 0xFFFFFFFC & pMemStart;
	pMemSize  = 0xFFFFFFFC & pMemSize;
	// 此处选择不同的内存管理算法
	pMemHandler = dPartitionInit(pMemStart,pMemSize);
}
