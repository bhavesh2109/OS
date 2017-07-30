#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>

void prints(char *string){

	for(int i = 0; string[i]!= '\0'; i++){

		printf("%c", string[i]);
	}

	printf("\n");

}


int main(){

	printf("CS347(M)$\n");

	pid_t cpid;

	while(1){
		
		cpid = fork();

		if(cpid == 0){

			char args[100], path[] = "/bin/";

			scanf("%[^\n]%*c", args);	//args accepts arguments to be run


			//Exit code needs to be checked
			if(!strcmp(args, "exit")){

				printf("Here\n");
				break;
			}
			else if(!strcmp(args, "name")){

				printf("Samir Wadhwa\nDhanvi Sreenivasan\nBhavesh Noob\n");
				continue;
			}
			else if(!strcmp(args, "roll")){
				printf("150100024\n150100007\n15D100009\n");
				continue;
			}

			strcat(path, args);	//makes the path /bin/"args"

			//Error Handling and run
			int a = execl(path,args, NULL);	//a checks for an error

			printf("%d\n", a);	
		}


		int status;
		wait(&status);
	}

	return 0;
}