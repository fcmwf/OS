#include "io.h"

void init8253(void){
	outb(0x43, 0x34);
	outb(0x40, ( (int)11932) & 0x00FF);
	outb(0x40, ( ((int)11932)>>8) & 0x00FF);
	return;
}
