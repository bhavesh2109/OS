#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
using namespace std;

int main() {
	bool flag = true;
	while(flag) {
		// User input and prompt generator
		
		char user_input[1000];
		cout << "CS347(M)$ ";
		fgets(user_input, 1000, stdin);
		user_input[strlen(user_input) - 1] = '\0';

		//Name and Roll number functionality

		if(strcmp(user_input, "name") == 0)
			printf("Name: Dhanvi\n");

		if(strcmp(user_input, "roll") == 0)
			printf("Roll No.: 15D100009\n");

		if(strcmp(user_input, "exit") == 0)
			flag = false;

	}
	return 0;
}