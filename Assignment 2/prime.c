#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
using namespace std;


bool is_prime(int n)
{   bool flag=true;
	for (int i=5;i<=(sqrt(n)+1);i=i+2)
    {
		if (n%i==0)
        {
		    flag=false;
		    return flag;
        }

    }
    return flag;
}


int main()
{

	pid_t pid;
	int c1,c2=0;
	int pipe_p_c1[2],pipe_c1_p[2],pipe_p_c2[2],pipe_c2_p[2];
	int n=1002;
    int prime1[n],prime2[n]; 

    int a=0;
    int b=0;

	pipe(pipe_p_c1);
    pipe(pipe_c1_p);

    for (int i=0;i<n;i++)
    {
        prime1[i]=0;
        prime2[i]=0;
    }



	pid=fork(); c1 = (int) pid;

    if (c1!=0) 
    {
    	pid=fork();c2 = (int) pid;
    }

    if ((c1<0)||(c2<0)) 
    {
    	printf("Error");
    }

    /*
    if (c1!=0&&c2!=0) printf("I am parent,My pid is  %d \n",getpid());
    if (c1==0&&c2==0) printf ("I am child1,My pid is %d \n",getpid());
    if (c1!=0&&c2==0) printf("I am child2,My pid is %d \n",getpid());
    */

    //c2==0 indicates a child process and c2!=0 indicates parent
    if(c1!=0)
    {   close(pipe_p_c1[0]);
        close(pipe_p_c2[0]);

        bool pipe_number=0;
    	for(int j=11;j<(n+1);j++)
    	{  
    		if((j%2!=0)&&(j%3!=0)&&(j%5!=0)&&(j%7!=0)&&(j%11!=0))
    		{   
                if (pipe_number==0)
                {
                    write(pipe_p_c1[1],&j,sizeof(int));
                    pipe_number=1;
                    continue;
                }

               if (pipe_number==1)
                {
                    write(pipe_p_c2[1],&j,sizeof(int));
                    pipe_number=0;
                    continue;
                }
			
    		}

    	}
    	int i=0;
    	write(pipe_p_c1[1],&i,sizeof(int));
    	close(pipe_p_c1[1]);

        write(pipe_p_c2[1],&i,sizeof(int));
        close(pipe_p_c2[1]);

        wait(NULL);
        close(pipe_c1_p[1]);
        close(pipe_c2_p[1]);
        close(pipe_c1_p[0]);
        close(pipe_c2_p[0]);


/*        for(int i=0;prime1[i]!=0;i++)
        {
            cout<<prime1[i]<<endl;
        }
*/        

    }

    if (c1==0&&c2==0)
    {   close(pipe_p_c2[0]);
        close(pipe_p_c2[1]);
        close(pipe_c2_p[0]);
        close(pipe_c2_p[1]);

        close(pipe_p_c1[1]);
        close(pipe_c1_p[0]);
        int array_index=0;
    	printf("Entering Child1\n");

    	while(1)
    	{   read(pipe_p_c1[0],&a,sizeof(int));
        	if(a==0) break;
            cout<<a<<endl;

            if (is_prime(a))
            {
                prime1[array_index]=a;
                array_index++;
                cout<<a<<endl;
            }

    	}
        printf("Exiting Loop1\n");
        close(pipe_p_c1[0]);
        write(pipe_c1_p[1],prime1,n*sizeof(int));
        close(pipe_c1_p[1]);

        printf("Exiting Child1\n");

    }

    if (c1!=0&&c2==0)
    {   close(pipe_p_c1[0]);
        close(pipe_p_c1[1]);
        close(pipe_c1_p[0]);
        close(pipe_c1_p[1]);

        close(pipe_p_c2[1]);
        close(pipe_c2_p[0]);
        int array_index=0;
        printf("Entering Child2\n");

        while(1)
        {   read(pipe_p_c1[0],&a,sizeof(int));
            if(b==0) break;
            cout<<a<<endl;

            if (is_prime(b))
            {
                prime1[array_index]=b;
                array_index++;
                cout<<b<<endl;
            }

        }
        printf("Exiting Loop2\n");
        close(pipe_p_c2[0]);
        write(pipe_c2_p[1],prime2,n*sizeof(int));
        close(pipe_c2_p[1]);

        printf("Exiting Child2\n");

    }



}