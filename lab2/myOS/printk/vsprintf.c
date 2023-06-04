/*
 * 识别格式化字符串的核心代码写在本文件中
 * 可以从网上移植代码
 */ 
#include <stdarg.h>  

void int_char(char*str, int num){
    int a,b;
    int data[100];
    int i = 0;
    a = num;
    do{
        b = a % 10;
        a = (a - b)/10;
        data[i] = b + 48;
        i++;
    } while (a > 0);
    i--;
    while(i>=0){
        *str = data[i];
        i--;
        str++;
    }
    *str = '\0';
    return;
}

int vsprintf(char *buf, const char *fmt, va_list argptr){
	va_list _end;
    int num;
    char *char_num;
	while(*fmt){
		if((*fmt == '%')&&(*(fmt+1) == 'd')){
			num = va_arg(argptr, int);
            int_char(char_num,num);
            while ((*char_num) != '\0'){
                *buf = *char_num;
                buf++;
                char_num++;
            }
			fmt = fmt + 2;
		}
		else {
			*buf = *fmt;
			buf++;
			fmt++;
		}
	}
}