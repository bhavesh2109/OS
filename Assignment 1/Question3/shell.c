#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

int run(char **command, char *path) {
	pid_t pid, wpid;
		int status;
		pid = fork();
		if(pid == 0) {
			execvp(path, command);
			exit(EXIT_FAILURE);
		} else if (pid < 0) {
		} else {
			do{																	//Written from documentation, stackOverflow
				wpid = waitpid(pid, &status, WUNTRACED);
			}while(!WIFEXITED(status) && !WIFSIGNALED(status));
		}
		return 1;
}

int main() {
	int status;
	char user_input[1000];
	bool flag = true;
	do {
		// User input and prompt generator
		cout << "CS347(M)$ ";
		fgets(user_input, 1000, stdin);
		user_input[strlen(user_input) - 1] = '\0';

		//Name and Roll number functionality

		if(strcmp(user_input, "name") == 0){
			printf("Name: Dhanvi\n");
			status = 1;
			continue;
		}

		if(strcmp(user_input, "roll") == 0){
			printf("Roll No.: 15D100009\n");
			status = 1;
			continue;
		}

		if(strcmp(user_input, "exit") == 0){
			flag = false;
			continue;
		}

		//String processing (splitting)
		
		char *c = strdup(user_input);
		char *tok = c, *end = c;
		vector<char*> v;
		while(tok != NULL) {
			strsep(&end, " ");
			v.push_back(tok);
			tok = end;
		}
		v.push_back(NULL);
		char *command[v.size()];
		for(int i=0; i<v.size(); i++) {
			command[i] = v[i];
		}
		//Creating of child process
		const char* prefix = "/bin/";
		char* path;
		int length = strlen(command[0])+6;
		path = (char*)malloc(length);
		strcpy(path, prefix);
		strcat(path, command[0]);
		status = run(command, path);
		free(c);
	}while(status && flag);
	return 0;
}
