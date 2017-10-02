#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

int tally[20];
int sum = 0;
pthread_mutex_t lock;

//Function to print out the vote count 
void printTally(){
	int i = 0;
	while(i<20){
		if (i%5 == 0 && i > 0)
			printf("	");
		printf("%d ", tally[i]);
		i++;
	}
	printf("\n");
}

//Vote counting function tun by thread
void *myThreadFun(void *vargp)
{
	int threadnumber = (int)vargp;
	threadnumber++;
	FILE *fptr;
	char filename[30];
	char suffix[] = ".txt";
	char *line = NULL;
	size_t len = 0;
	size_t pos = 0;
	ssize_t read;
	int count_l = 0;
	snprintf(filename, sizeof(filename), "%d%s", threadnumber, suffix);
	fptr = fopen(filename, "r");
	if (fptr == NULL)
		exit(EXIT_FAILURE);
	while((read = getline(&line, &len, fptr)) != -1) {
		//Ignoring empty lines
		if(isspace(line[0])){
			count_l = 0;
			continue;
		}
		//Ignoring district numbers
		if(count_l == 0){
			count_l++;
			continue;
		}
		else {
			char *token,  *tofree;
			tofree = line;
			int count = 0;
			int party = 0,votes = 0;
			while( (token = strsep(&line, " ")) != NULL){
				int a = atoi(token);
				if (count == 0){
					party = a;
				}else {
					votes = a;
				}
				count++;
			}
			party--;
			pthread_mutex_lock(&lock);
			//------ START CRITICAL SECTION --------
			int pre = sum/50;
			tally[party] = tally[party] + votes;
			sum = sum + votes;
			int post = sum/50;
			if(post > pre)
				printTally();
			//------ END CRITICAL SECTION ----------
			pthread_mutex_unlock(&lock);
		}
		count_l++;
		}
		
	
	fclose(fptr);
	if(line)
		free(line);
	return NULL;
}
  
int main()
{
	int j =0;
	while(j<20) {
	tally[j] = 0;
	j++;
	}
	int n = 10;
	int i = 0;
    pthread_t tid[n];
    while(i<n){
    	pthread_create(&tid[i], NULL, myThreadFun, (void *)(long)i);
    	i++;
    }
    i =0;
    while(i<n){
    	pthread_join(tid[i], NULL);
    	i++;
    }
    exit(0);
}
