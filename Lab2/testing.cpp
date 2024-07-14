#include<bits/stdc++.h>
#include<sys/wait.h>
using namespace std;

int main()
{
    
    int a = 4;
    while (a > 0)
    {
        int ret_val = fork();
        if(ret_val == 0)
        {
            cout<<a<<endl;
        }
        else
        {
            wait(NULL);
        }
        // cout<<a<<endl;
        a--;
    }
    
    

    return 0;
}