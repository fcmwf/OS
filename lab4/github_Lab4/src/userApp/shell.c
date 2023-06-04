//shell.c --- malloc version
#include "../myOS/userInterface.h"

#define NULL (void*)0

int getCmdline(unsigned char *buf, int limit){
    unsigned char *ptr = buf;
    int n = 0;
    while (n < limit) {
        *ptr = uart_get_char();        
        if (*ptr == 0xd) {
            *ptr++ = '\n';
            *ptr = '\0';
            uart_put_char('\r');
            uart_put_char('\n');
            return n+2;
        }
        uart_put_char(*ptr);
        ptr++;
        n++;
    }     
    
    return n;
}

typedef struct command {
    unsigned char *cmd;
    int (*func)(int argc, unsigned char **argv);
    void (*help_func)(void);
    unsigned char *desc;
    struct command *next;
} command;

#define cmd_size sizeof(struct command)

struct command *ourCmds = NULL;

int listCmds(int argc, unsigned char **argv){
    struct command *tmpCmd = ourCmds;
    myPrintf(0x7, "list all registered commands:\n");
    myPrintf(0x7, "command name: description\n");

    while (tmpCmd != NULL) {
        myPrintf(0x7,"% 12s: %s\n",tmpCmd->cmd, tmpCmd->desc);
        tmpCmd = tmpCmd->next;
    }
    return 0;
}

void addNewCmd(unsigned char *cmd, 
		int (*func)(int argc, unsigned char **argv), 
		void (*help_func)(void), 
		unsigned char* desc) {
    //TODO
    command *newCmd = (command *)MYmalloc(sizeof(command));
    //myPrintf(0x7,"addr=%x\n",newCmd);
    char *cmd_new = (char *)MYmalloc(myStrlen(cmd) + 1);
    char *desc_new = (char *)MYmalloc(myStrlen(desc) + 1);
    if (!newCmd || !cmd_new || !desc_new)
        return;

    newCmd->cmd = cmd_new;
    newCmd->desc = desc_new;
    newCmd->func = func;
    newCmd->help_func = help_func;
    myStrcpy(newCmd->cmd, cmd);
    myStrcpy(newCmd->desc, desc);

    // insert to the head of list
    newCmd->next = ourCmds;
    ourCmds = newCmd;
}

void help_help(void){
    myPrintf(0x7,"USAGE: help [cmd]\n\n");
}

int help(int argc, unsigned char **argv){
    int i;
    struct command *tmpCmd = ourCmds;
    help_help();

    if (argc==1) return listCmds(argc,argv);
    if (argc>2) return 1;
    
    while (tmpCmd != NULL) {            
        if (myStrcmp(argv[1],tmpCmd->cmd)==0) {
            if (tmpCmd->help_func!=NULL)
                tmpCmd->help_func();
            else myPrintf(0x7,"%s\n",tmpCmd->desc);
            break;
        }
        tmpCmd = tmpCmd->next;
    }
    return 0;
}

struct command *findCmd(unsigned char *cmd){
        struct command * tmpCmd = ourCmds;
	int found = 0;
        while (tmpCmd != NULL) {  //at lease 2 cmds            
            if (myStrcmp(cmd,tmpCmd->cmd)==0){
		    found=1;
		    break;
	    }
            tmpCmd = tmpCmd->next;
        }
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
    struct command *tmpCmd;
    //myPrintf(0x7,"StartShell:\n");     
    
    while(1) {
        myPrintf(0x3,"Student >:");
        getCmdline(&cmdline[0],100);
        myPrintf(0x7,cmdline);

        argc = split2Words(cmdline,&argv[0],10); 
        if (argc == 0) continue;

	    tmpCmd = findCmd(argv[0]);

        if (tmpCmd)   
	        tmpCmd->func(argc, argv);
	    else
            myPrintf(0x7,"UNKOWN command: %s\n",argv[0]);
    }
}
