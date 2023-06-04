#include "io.h"

void init8259A(void){
	outb(0x21, 0xFF);
	outb(0xA1, 0xFF);

	outb(0x20, 0x11);
	outb(0x21, 0x20);
	outb(0x21, 0x4);
	outb(0x21, 0x3);

	outb(0xA0, 0x11);
	outb(0xA1, 0x28);
	outb(0xA1, 0x02);
	outb(0xA1, 0x01);
	return;
}
