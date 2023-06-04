/*
 * 本文件实现vga的相关功能，清屏和屏幕输出
 * clear_screen和append2screen必须按照如下借口实现
 * 可以增加其他函数供clear_screen和append2screen调用
 */
extern void outb (unsigned short int port_to, unsigned char value);
extern unsigned char inb(unsigned short int port_from);
//VGA字符界面规格：25行80列
//VGA显存初始地址为0xB8000

short cur_line=0;
short cur_column=0;//当前光标位置
char * vga_init_p = (char *)0xB8000;


void update_cursor(void){//通过当前行值cur_cline与列值cur_column回写光标
	//use global variable cur_line and cur_column tp update the current cursor position
	short cur_pos;
	cur_pos = 80*cur_line + cur_column;
	//here, we will use the inb and outb to interact with port = 0x3D4 and port = 0x3D5 
	outb(0x3D4,0x0E);
	outb(0x3D5, (unsigned char)((cur_pos>>8)&0x00FF));  //高八位
	outb(0x3D4,0x0F);		
	outb(0x3D5, (unsigned char)((cur_pos)&0x00FF));	 //低八位
	return;
}

  short get_cursor_position(void){//获得当前光标，计算出cur_line和cur_column的值
	 short high_pos, low_pos,cur_pos;
	outb(0x3D4,0x0E);
	high_pos = inb(0x3D5);
	outb(0x3D4,0x0F);
	low_pos  = inb(0x3D5);
	cur_pos  = (low_pos&0x00FF)|((high_pos<<8)&0xFF00);
	cur_line = cur_pos / 80;
	cur_column   = cur_pos - cur_line*80; 
	return cur_pos;
}


void clear_screen() {   //清屏
	char space = ' ';
	int i;
	for(i=0; i<25*80; i++)
		*(vga_init_p + 2*i) = (space);
	cur_line = 0;
	cur_column = 0;
	update_cursor();
	return;
}

void scroll(int color) {    //滚屏
	int i,j;
	char space = ' ';
	for(i=0;i<24;i++)
		for(j=0;j<80;j++)
			*(vga_init_p+2*(i*80+j)) = *(vga_init_p+2*((i+1)*80+j));
	for(i=24,j=0;j<80;j++)
		*(vga_init_p + 2*(i*80 + j)) =  ((color << 8) & 0xff00) | space;
	cur_column = 0;
	cur_line   = 24;
	update_cursor();
	return;
}

void append2screen(char *str,int color){ 
	short *p;
	short pos;
	while(*str){
		if((*str == '\n')|(*str == '\r')){
			if(cur_line >= 24)
				scroll(color);
			else {
				cur_line += 1;
				cur_column = 0;
				update_cursor();
			}
			str++;
		}
		else{
			pos = get_cursor_position();
			p =(short*)( vga_init_p + 2*pos);
			*p = ((color<<8)&0xFF00)|(*str);
			cur_column +=1;
			update_cursor();
			if(cur_line >= 25)
				scroll(color);
			str++;
		}
	}
	return;
}