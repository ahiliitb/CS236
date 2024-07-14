#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int argc, char* argv[])
{
    int childoneid;
    if (argc != 5)
    {
        printf("Incorrect number of arguments\n");
    }
    int child1 = fork();
    if(child1 == 0)
    {
        execlp(argv[1], argv[1] ,argv[2], NULL);
        kill(getpid(), SIGKILL);
    }
    else
    {
        wait(NULL);
    }

    int child2 = fork();
    if(child2 == 0)
    {
        execlp(argv[3], argv[3],argv[4], NULL);
    }
    else
    {
        wait(NULL);
    }
    


    return 0;
}