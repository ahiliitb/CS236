#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<unistd.h>
#include<signal.h>

int main(int argc, char* argv[])
{
    int ret = fork();
    if(ret == 0)
    {
        printf("\n4");
        printf("ahil");
        sleep(5);
    }
    else 
    {
        int wstatus;
        wait(NULL);
        int x = waitpid(ret, &wstatus, WNOHANG); //particular pid wait
        printf("\n%d", x);
        printf("\n%d", wstatus);
    }

    return 0;
}