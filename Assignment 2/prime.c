#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>


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


void merge(int *prime1,int *prime2,int *sorted_array,int n)
{ 

    int i=0;
    int L=0;
    int R=0;
    while(i<(n-5))
    {   
        if (prime1[L]==0||prime2[R]==0) break;

        if (prime1[L]>=prime2[R])
        {
            sorted_array[i+5]=prime2[R];
            R++;i++;
        }
        else if (prime1[L]<prime2[R])
        {
            sorted_array[i+5]=prime1[L];
            L++;i++;
        }
    }
    if (prime1[L]==0)
    {
        while (i<n)
        {   
            if (prime2[R]==0) break;
            sorted_array[i+5]=prime2[R];
            R++;i++;
        }
    }
    else if (prime2[R]==0)
    {
        while(i<n)
        {
            if (prime1[L]==0) break;
            sorted_array[i+5]=prime1[L];
            L++;i++;
        }
    }
    


}


int main()
{

    pid_t pid;
    int c1=0;int c2=0;
    int pipe_p_c1[2],pipe_c1_p[2],pipe_p_c2[2],pipe_c2_p[2];
    int n;
    printf("Enter N ");
    scanf("%d",&n);
    int prime1[n],prime2[n],sorted_array[n]; 

    int a=0;
    int b=0;

    if(pipe(pipe_p_c1)<0||pipe(pipe_c1_p)<0) 
    {
        printf("Error in creating a pipe\n");
        exit(1);
    }
    if(pipe(pipe_p_c2)<0||pipe(pipe_c2_p)<0) 
    {
        printf("Error in creating a pipe\n");
        exit(1);
    }
    for (int i=0;i<n;i++)
    {
        prime1[i]=0;
        prime2[i]=0;
        sorted_array[i]=0;
    } 

    if (n==2){sorted_array[0]=2;} 
    
    else if(n==3||n==4){sorted_array[0]=2;sorted_array[1]=3;}
    else if(n==5||n==6){sorted_array[0]=2;sorted_array[1]=3;sorted_array[2]=5;}
    else if(n>=7&&n<11){sorted_array[0]=2;sorted_array[1]=3;sorted_array[2]=5;sorted_array[3]=7;}
    else if(n>=11){sorted_array[0]=2;sorted_array[1]=3;sorted_array[2]=5;sorted_array[3]=7;sorted_array[4]=11;}




    pid=fork(); c1 = (int) pid;

    if (c1!=0) 
    {
        pid=fork();c2 = (int) pid;
    }

    if ((c1<0)||(c2<0)) 
    {
        printf("Error in forking\n");
        exit(1);
    }

 /*   
    if (c1!=0&&c2!=0) printf("I am parent,My pid is  %d \n",getpid());
    if (c1==0&&c2==0) printf ("I am child1,My pid is %d \n",getpid());
    if (c1!=0&&c2==0) printf("I am child2,My pid is %d \n",getpid());
    
*/
//parent loop
    if(c1!=0&&c2!=0)
    {  
        close(pipe_p_c1[0]);
        close(pipe_p_c2[0]);
        close(pipe_c1_p[1]);
        close(pipe_c2_p[1]);

        bool pipe_number=0;
        for(int j=13;j<=n;j=j+2)
        {  
            if((j%2!=0)&&(j%3!=0)&&(j%5!=0)&&(j%7!=0)&&(j%11!=0))
            { 
                if (pipe_number==0)
                {   
                    write(pipe_p_c1[1],&j,sizeof(int));
                    pipe_number=1;
                }

               else if (pipe_number==1)
                {   
                    write(pipe_p_c2[1],&j,sizeof(int));
                    pipe_number=0;
                }
            
            }

        }
        int term=0;
        write(pipe_p_c1[1],&term,sizeof(int));
        close(pipe_p_c1[1]);

        write(pipe_p_c2[1],&term,sizeof(int));
        close(pipe_p_c2[1]);

        wait(NULL);
        wait(NULL);

        read(pipe_c1_p[0],prime1,n*sizeof(int));
        read(pipe_c2_p[0],prime2,n*sizeof(int)); 

        close(pipe_c1_p[0]);
        close(pipe_c2_p[0]); 


        merge(prime1,prime2,sorted_array,n); 
    




        for(int i=0;i<n;i++)
        {    
            if (sorted_array[i]==0) break;
            printf("%d ",sorted_array[i]);

        }


        exit(0);


       

    }

    if (c1==0&&c2==0)
    {   close(pipe_p_c2[0]);
        close(pipe_p_c2[1]);
        close(pipe_c2_p[0]);
        close(pipe_c2_p[1]);

        close(pipe_p_c1[1]);
        close(pipe_c1_p[0]);
        int array_index=0;
//      printf("Entering Child1\n");

        while(1)
        {   
            read(pipe_p_c1[0],&a,sizeof(int));
            if(a==0) break;

            if (is_prime(a))
            {
                prime1[array_index]=a;
                array_index++;
            }

        }
//        printf("Exiting Loop1\n");
        close(pipe_p_c1[0]);
        write(pipe_c1_p[1],prime1,n*sizeof(int));
        close(pipe_c1_p[1]);
        exit(0);

//        printf("Exiting Child1\n");

    }

    if (c1!=0&&c2==0)
    {   close(pipe_p_c1[0]);
        close(pipe_p_c1[1]);
        close(pipe_c1_p[0]);
        close(pipe_c1_p[1]);

        close(pipe_p_c2[1]);
        close(pipe_c2_p[0]);

        int array_index=0;
//        printf("Entering Child2\n");

        while(1)
        {   
            read(pipe_p_c2[0],&b,sizeof(int));
            if(b==0) break;

            if (is_prime(b))
            {
                prime2[array_index]=b;
                array_index++;
            }

        }
//        printf("Exiting Loop2\n");
        close(pipe_p_c2[0]);
        write(pipe_c2_p[1],prime2,n*sizeof(int));
        close(pipe_c2_p[1]);
        exit(0);



//       printf("Exiting Child2\n");

    }



}