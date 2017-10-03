#include"semaphore.h"
#include<stdlib.h>
#include<unistd.h>
#include<iostream>
#include<vector>
#include<pthread.h>

using namespace std;

struct produce{

	int cycle;
	int num;

	produce(int inp1, int inp2){
		cycle = inp1;
		num = inp2;
	}

};

vector<int> buffer;

Semaphore empty_count(0);
Semaphore filled_count = Semaphore(0);

pthread_mutex_t min_mutex;
pthread_mutex_t buffer_mutex;
pthread_mutex_t print_mutex;
pthread_mutex_t sync_mutex[5];
pthread_mutex_t input_mutex;
pthread_mutex_t consume_mutex;

vector<pthread_cond_t> condvars(5);

int min_cycle[3] = {1, 1, 1};	//To store minimum cycle for which data is present in P
int generated_num = 1, thread_count = 5; 
int inputs[5];

vector<produce> overflow[3];
int consume[2] = {0,0}; //To store extra that consumer must consume

void display(){

	pthread_mutex_lock(&print_mutex);

	cout<<"Displaying buffer: ";

	for(int i = 0; i<buffer.size(); i++){

		cout<<buffer[i]<<" ";
	}

	cout<<endl;

	cout<<"Displaying overflow: ";

	for(int j = 0; j<3; j++){
	for(int i = 0; i<overflow[j].size(); i++){

		cout<<overflow[j][i].num<<" ";

	}}

	cout<<endl;

	pthread_mutex_unlock(&print_mutex);

}

void *producer(void *t){

	long my_id = (long)t;
	int cycle = 0;

	//close(fd[my_id][1]); //To close writing end of pipe

	int num;

	//while(read(fd[my_id][0], &num, sizeof(num))> 0){

	while(1){

		pthread_mutex_lock(&sync_mutex[my_id]);
		pthread_cond_wait(&condvars[my_id], &sync_mutex[my_id]);

		pthread_mutex_lock(&input_mutex);

		num = inputs[my_id];
		
		cycle++;

		//EDIT (Remove gen_mutex)
		//Generating the new 

		for(int i = 0; i<num; i++){
			overflow[my_id].push_back(produce(cycle, generated_num+i));
		}

		generated_num += num;


		pthread_mutex_lock(&print_mutex);
		cout<<"Producer "<<my_id<<" created numbers"<<endl;
		pthread_mutex_unlock(&print_mutex);

		pthread_mutex_unlock(&input_mutex);

		for(int i = 0; i<num; i++){

			empty_count.down();

			pthread_mutex_lock(&min_mutex);

			if(min_cycle[my_id] <= min_cycle[(my_id+1)%3] && min_cycle[my_id] <= min_cycle[(my_id+2)%3]){

				pthread_mutex_lock(&buffer_mutex);


				pthread_mutex_lock(&print_mutex);
				cout<<"Producer: "<<my_id<<" writing "<<overflow[my_id].front().num<<endl;
				pthread_mutex_unlock(&print_mutex);

				buffer.push_back(overflow[my_id].front().num);

				overflow[my_id].erase(overflow[my_id].begin());

				min_cycle[my_id] = overflow[my_id][0].cycle;

				pthread_mutex_unlock(&buffer_mutex);

				filled_count.up();

				pthread_mutex_lock(&print_mutex);
				cout<<"Producer "<<my_id<<" done"<<endl;
				pthread_mutex_unlock(&print_mutex);


			}	

			pthread_mutex_unlock(&min_mutex);

		}

		pthread_mutex_unlock(&sync_mutex[my_id]);

	}

}

void *consumer(void *t){

	long my_id = (long)t;

	int num;

	//while(read(fd[my_id][0], &num, sizeof(num))> 0){

	while(1){
		pthread_mutex_lock(&sync_mutex[my_id]);
		pthread_cond_wait(&condvars[my_id], &sync_mutex[my_id]);

		pthread_mutex_lock(&print_mutex);
		cout<<"Consumer "<<my_id<<" running"<<endl;
		pthread_mutex_unlock(&print_mutex);

		pthread_mutex_lock(&input_mutex);
		pthread_mutex_lock(&consume_mutex);

		num = inputs[my_id];	
		consume[my_id-2] += num;
		num = consume[my_id-2];

		pthread_mutex_unlock(&consume_mutex);
		pthread_mutex_unlock(&input_mutex);

		for(int i =0; i<num; i++){

			filled_count.down();

			pthread_mutex_lock(&buffer_mutex);

			pthread_mutex_lock(&print_mutex);
			cout<<"Consumer "<<my_id<<" in buffer"<<endl;
			pthread_mutex_unlock(&print_mutex);

			buffer.pop_back();

			pthread_mutex_unlock(&buffer_mutex);

			empty_count.up();

			pthread_mutex_lock(&consume_mutex);
			consume[my_id-2]--;
			pthread_mutex_unlock(&consume_mutex);

			pthread_mutex_lock(&print_mutex);
			cout<<"Consumer "<<my_id<<" done"<<endl;
			pthread_mutex_unlock(&print_mutex);

		}

		pthread_mutex_unlock(&sync_mutex[my_id]);

	}

}

int main(){

	pthread_t threads[5];

	int buffer_size;

	cin>>buffer_size;

	empty_count = Semaphore(buffer_size);

	//intitializing condition variables

	for(int i = 0; i< 5; i++){

		pthread_cond_init(&condvars[i], NULL);

		if(i<3){
			pthread_create(&threads[i], NULL, producer, (void*)i);
		}
		else{
			pthread_create(&threads[i], NULL, consumer, (void*)i);
		}
	}


	for(int i = 0; i<5; i++){

		pthread_mutex_lock(&sync_mutex[i]);

	}

	while(1){

		pthread_mutex_lock(&print_mutex);
		cout<<"Input: ";
		pthread_mutex_unlock(&print_mutex);
		
		int n;

		for(int i = 0; i<5; i++){

			cin>>n;

			if(n == -1){

				display();
				return 0;
			}

			//write(fd[i][1], &n, sizeof(n));

			inputs[i] = n;

			//cout<<"Signalled "<<i<<endl;
			//Remember to add code to synchronise for each cycle
		}

		for(int i = 0; i<5; i++){

			pthread_cond_signal(&condvars[i]);
			pthread_mutex_unlock(&sync_mutex[i]);
			pthread_mutex_lock(&print_mutex);
			cout<<"Thread "<<i<<" signalled"<<endl;
			pthread_mutex_unlock(&print_mutex);
		}

		for(int i = 0; i<5; i++){
			pthread_mutex_lock(&sync_mutex[i]);
		}

		display();

	}	


	return 0;

}