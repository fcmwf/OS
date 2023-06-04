/* 声明入口点 */
.globl start
/* 定义Multiboot header */
	magic_ITEM_NAME    = 0x1BADB002           /* Multiboot magic number */
	flags_ITEM_NAME    = 0x0                  /* Multiboot flags */
	checksum_ITEM_NAME = -(0x1BADB002 + 0x0)  /* Checksum (magic + flags + checksum must equal 0) */


.section  ".lab1"
.align 4
    .long   magic_ITEM_NAME 
    .long   flags_ITEM_NAME
    .long   checksum_ITEM_NAME

.text
.code32
start:
    /* VGA 输出 "PB21051012" */
    movl $0x2F422F50, 0xB8000
    movl $0x2F312F32, 0xB8004
    movl $0x2F352F30, 0xB8008
    movl $0x2F302F31, 0xB800C
    movl $0x2F322F31, 0xB8010

    /* 串口输出 "LXH" */
    movw $0x3F8, %dx     /* 把串口地址 0x3F8 装入 DX 寄存器 */
    movb $0x4C, %al      /* 把 'L' 存入 AL 寄存器 */
    outb %al, %dx        /* 输出 AL 寄存器的值到 DX 寄存器指定的串口端口 */
    movb $0x58, %al      /* 把 'X' 存入 AL 寄存器 */
    outb %al, %dx        /* 输出 AL 寄存器的值到 DX 寄存器指定的串口端口 */
    movb $0x48, %al      /* 把 'H' 存入 AL 寄存器 */
    outb %al, %dx        /* 输出 AL 寄存器的值到 DX 寄存器指定的串口端口 */

    /* 停机 */
    hlt

/* 声明代码结束点 */
end: