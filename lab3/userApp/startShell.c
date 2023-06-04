#include "io.h"
#include "myPrintk.h"
#include "uart.h"
#include "vga.h"
#include "i8253.h"
#include "i8259A.h"
#include "tick.h"
#include "wallClock.h"
#define command_num 2
typedef struct myCommand {
    char name[80];
    char help_content[200];
    int (*func)(int argc, char (*argv)[8]);
}myCommand; 
int func_cmd(int argc, char (*argv)[8]);
int func_help(int argc, char (*argv)[8]);
int find_command(char *str);
void usd_strcpy(char*str1, char*str2);
int usd_strcmp(char*str1, char*str2);
void split(char*buf,int *argc,char argv[8][8]);

myCommand command[command_num] = {
    {"cmd", "List all command\n", func_cmd},
    {"help", "Usage: help [command]\nDisplay info about [command]\n", func_help},
    // add more commands if needed4
};
void usd_strcpy(char*str1, char*str2){  //Assign the value of str1 to str2
    while(*str1){
        *str2 = *str1;
        str1++;
        str2++;
        }
    *str2 = '\0';
}
int usd_strcmp(char*str1, char*str2){   //Compare str1 and str2 return 1 or 0 or-1;
    while(*str1&&*str2){
        if(*str1!=*str2)
            break;
        str1++;
        str2++;
        }
    if(*str1>*str2) return 1;
    else if(*str1==*str2) return 0;
    else return -1;
}
int func_cmd(int argc, char (*argv)[8]){
    for(int i=0;i<command_num;i++)
        myPrintk(0x07,"%s\n",command[i].name);
}
int func_help(int argc, char (*argv)[8]){
    int num;
    num = find_command(argv[1]);
    myPrintk(0x07,"%s\n",command[num].help_content);
}
void split(char*buf,int *argc,char argv[8][8]){
    int i=0,j=0;
    char* p=buf;
    while(*p){
        if((*p=='[')||(*p==']')){
            argv[i][j] = '\0';
            i++;
            j=0;
        }
        else {
            argv[i][j] = *p;
            j++;
        }
        p++;
    }
    argv[i][j] = '\0';
    *argc = i+1;
}
int find_command(char *str){
    for(int i=0;i<command_num;i++)
        if(!(usd_strcmp(str,command[i].name))) return i;
    return -1;
}
void startShell(void){
//我们通过串口来实现数据的输入
char BUF[256]; //输入缓存区
int BUF_len=0;	//输入缓存区的长度
	int argc;
    char argv[8][8];
    do{
        BUF_len=0; 
        myPrintk(0x07,"MyOS>>\0");
        while((BUF[BUF_len]=uart_get_char())!='\r'){
            myPrintk(0x07,"%c",BUF[BUF_len]);
            BUF_len++;  //BUF数组的长度加
        }
        BUF[BUF_len] = '\0';
        myPrintk(0x07,"\n");

        split(BUF,&argc,argv);
        int number;
        number = find_command(argv[0]);
        command[number].func(argc,argv);
    }while(1);

}

