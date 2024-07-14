#include<bits/stdc++.h>
#include <sys/wait.h>

using namespace std;

int main()
{
    int x; cin>>x;
    while(x--)
    {
        if(x == 1)chdir("..");
        else
        {
            int fork_ret = fork();
            if(fork_ret == 0)
            {
                execlp("ls", "ls", NULL);
            }
            else
            {
                int wstatus;
                wait(&wstatus);
                kill(fork_ret, SIGKILL);
            }
        }
    }
    // chdir("Lab2");
    // system("ls");
    
    

    return 0;
}