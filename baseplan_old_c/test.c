#include<stdlib.h>
#include<stdio.h>
#include<assert.h>


int main(int argc, char *argv[])
{
	char str[50];
	strcpy(str,argv[1]);
	printf("str:%s,len=%d\n", str, wcslen(str)); 
	printf("str:%s,len=%d\n", str, strlen(str)); 
}
