//shell.c --- malloc version
#include "../myOS/userInterface.h"
#define cmd_size sizeof(struct cmd)
#define NULL (void*)0
struct cmd *ourCmds = NULL;

typedef struct cmd {
    unsigned char cmd[20+1];
    int (*func)(int argc, unsigned char **argv);
    void (*help_func)(void);
    unsigned char description[100+1];  
    struct cmd * nextCmd ;
} cmd;

int listCmds(int argc, unsigned char **argv){
    struct cmd *tmpCmd_2 = ourCmds;
    myPrintf(0x7, "list all registered commands:\n");
    myPrintf(0x7, "command name: description\n");
    while (tmpCmd_2 != NULL) {
        myPrintf(0x7,"% 12s: %s\n",tmpCmd_2->cmd, tmpCmd_2->description);
        tmpCmd_2 = tmpCmd_2->nextCmd;
    }
    return 0;
}


void addNewCmd(	unsigned char *myCmd, 
		        int (*func)(int argc, unsigned char **argv), 
		        void (*help_func)(void), 
		        unsigned char* description){
	// TODO
    /*功能：增加命令
    1. 使用malloc创建一个cmd的结构体，新增命令。
    2. 同时还需要维护一个表头为ourCmds的链表。
    */
    cmd *newCmd = (cmd *)malloc_1(sizeof(struct cmd));
    //myPrintf(0x7,"sizeof(newCmd)=%d\n",sizeof(cmd));
    //myPrintf(0x7,"addr=%x\n",newCmd);
    if (!newCmd)
        return;
    newCmd->func = func;
    newCmd->help_func = help_func;
    int i;
    i = strcpy_1(myCmd,newCmd->cmd);
    newCmd->cmd[i]='\0';
    i = strcpy_1(description,newCmd->description);
    newCmd->description[i]='\0';
    // insert to the head of list
    newCmd->nextCmd = ourCmds;
    ourCmds = newCmd;
    //myPrintf(0x7,"addcmd:%s:%x\n",newCmd->cmd,newCmd->func);
    //myPrintf(0x7,"cmd:%s,des:%s\n",newCmd->cmd,newCmd->description);
    // myPrintf(0x7,"this is a test\n");
    // struct cmd *tmpCmd = ourCmds;
    // while (tmpCmd != NULL) {
    //     myPrintf(0x7,"% 12s: %s\n",tmpCmd->cmd, tmpCmd->description);
    //     tmpCmd = tmpCmd->nextCmd;
    // }
    // myPrintf(0x7,"test over\n");
}

void help_help(void){
    myPrintf(0x7,"USAGE: help [cmd]\n\n");
}

int help(int argc, unsigned char **argv){
    int i;
    struct cmd *tmpCmd = ourCmds;
    help_help();

    if (argc==1) return listCmds(argc,argv);
    if (argc>2) return 1;
    
    while (tmpCmd != NULL) {            
        if (strcmp_1(argv[1],tmpCmd->cmd)==0) {
            if (tmpCmd->help_func!=NULL)
                tmpCmd->help_func();
            else myPrintf(0x7,"%s\n",tmpCmd->description);
            break;
        }
        tmpCmd = tmpCmd->nextCmd;
    }
    return 0;
}

struct cmd *findCmd(unsigned char *cmd){
        struct cmd * tmpCmd = ourCmds;
	int found = 0;
        while (tmpCmd != NULL) {  //at lease 2 cmds            
            if (strcmp_1(cmd,tmpCmd->cmd)==0){
		    found=1;
		    break;
	    }
            tmpCmd = tmpCmd->nextCmd;
        }
    //if(found)myPrintf(0x7,"findcmd:%s,%x,%x,%s\n",tmpCmd->cmd,tmpCmd->func,tmpCmd->help_func,tmpCmd->description);
	return found?tmpCmd:NULL;
}

int split2Words(unsigned char *cmdline, unsigned char **argv, int limit){
    unsigned char c, *ptr = cmdline;
    int argc=0;    
    int inAWord=0;

    while ( c = *ptr ) {  // not '\0'
        if (argc >= limit) {
            myPrintf(0x7,"cmdline is tooooo long\n");
            break;
        }
        switch (c) {
            case ' ':  *ptr = '\0'; inAWord = 0; break; //skip white space
            case '\n': *ptr = '\0'; inAWord = 0; break; //end of cmdline?
            default:  //a word
             if (!inAWord) *(argv + argc++) = ptr;
             inAWord = 1;             
             break;
        }   
        ptr++;     
    }
    return argc;
}

void initShell(void){
    addNewCmd("cmd\0",listCmds,NULL,"list all registered commands\0");
    addNewCmd("help\0",help,help_help,"help [cmd]\0");
}

unsigned char cmdline[100];
void startShell(void){    
    unsigned char *argv[10];  //max 10
    int argc;    
    struct cmd *tmpCmd;
    int BUF_len;
    // myPrintf(0x7,"StartShell:\n");     
    // myPrintf(0x7,"this is a test\n");
    // struct cmd *tmpCmd_1 = ourCmds;
    // while (tmpCmd_1 != NULL) {
    //     myPrintf(0x7,"% 12s: %s\n",tmpCmd_1->cmd, tmpCmd_1->description);
    //     tmpCmd_1 = tmpCmd_1->nextCmd;
    // }
    // myPrintf(0x7,"test over\n");
    //myPrintf(0x7,"&func:listCmds=%x\n",&listCmds);
    //myPrintf(0x7,"&func:help=%x\n",&help);
    while(1) {
        myPrintf(0x3,"Student >:");
        BUF_len = 0;
        while((cmdline[BUF_len]=uart_get_char())!='\r'){
            myPrintf(0x07,"%c",cmdline[BUF_len]);
            BUF_len++;  //BUF数组的长度加
        }
        cmdline[BUF_len] = '\0';
        myPrintf(0x7,"%s\n",cmdline);
        argc = split2Words(cmdline,&argv[0],10); 
        if (argc == 0) continue;

	    tmpCmd = findCmd(argv[0]);
        if (tmpCmd){   
            //myPrintf(0x7,"func=%x\n",tmpCmd->func);
	        (tmpCmd->func)(argc, argv);
        }
	    else
            myPrintf(0x7,"UNKOWN command: %s\n",argv[0]);
        myPrintf(0x7,"program over\n");
    }
}