#include<stdio.h>
#include<string.h>
#define MAXLEN 64
typedef struct helloworld{
    unsigned int number;
    char name[MAXLEN];
}HelloWorld;

int main(){
    HelloWorld Helloworld;
    Helloworld.number = 2018218794;
    strcpy(Helloworld.name,"wenjaihao");
    printf("%u %s",Helloworld.number,Helloworld.name);
    return 0;
}