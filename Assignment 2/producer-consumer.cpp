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
vector<struct Semaphore> sync_thread(5, Semaphore(0));
vector<struct Semaphore> sync_parent(5, Semaphore(0));

pthread_mutex_t total_mutex;
pthread_mutex_t print_mutex;

vector<pthread_cond_t> condvars(5);
pthread_cond_t parent_cv;

int min_cycle[3] = {1, 1, 1};	//To store minimum cycle for which data is present in P
int generated_num = 1;
int inputs[5];

vector<produce> overflow[3];
int consume[2] = {0,0}; //To store extra that consumer must consume
int buffer_size;


bool parent_signal[5] = {0,0,0,0,0}; //To check if threads have accepted input
bool thread_recieved_signal[5] = {0,0,0,0,0}; //To check if threads used the dummy signal sent by parent
bool parent_sent_signal = 0; //To check if parent sent false signal yet

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

	int num;

	while(1){

		sync_thread[my_id].down();

		pthread_mutex_lock(&total_mutex);

		num = inputs[my_id];
		
		cycle++;

		for(int i = 0; i<num; i++){
			overflow[my_id].push_back(produce(cycle, generated_num+i));
		}

		parent_signal[my_id] = 1;

		num = overflow[my_id].size();

		generated_num += num;
		

		pthread_mutex_unlock(&total_mutex);
	
		for(int i = 0; i<num; i++){

			empty_count.down();

			pthread_mutex_lock(&total_mutex);

			//To handle dummy signal
			if(consume[0] == 0 && consume[1] == 0 && parent_sent_signal == 1 && buffer.size() >= buffer_size){
				
				thread_recieved_signal[my_id] = 1;
				pthread_mutex_unlock(&total_mutex);
				break;
			}
			
			//To ensure lower cycle data is put into buffer first
			if( ((min_cycle[my_id] <= min_cycle[(my_id+1)%3]) || !overflow[(my_id+1)%3].size()) && ((min_cycle[my_id] <= min_cycle[(my_id+2)%3]) || !overflow[(my_id+2)%3].size())){

				buffer.push_back(overflow[my_id].front().num);

				overflow[my_id].erase(overflow[my_id].begin());

				min_cycle[my_id] = overflow[my_id][0].cycle;

				filled_count.up();

			}

			else{
				i--;
				empty_count.up();
				
			}

			pthread_mutex_unlock(&total_mutex);	


		}

		pthread_cond_signal(&parent_cv);

		sync_parent[my_id].up();
	}

}

void *consumer(void *t){

	long my_id = (long)t;

	int num;

	while(1){
		
		sync_thread[my_id].down();

		pthread_mutex_lock(&total_mutex);

		num = inputs[my_id];	
		consume[my_id-3] += num;
		num = consume[my_id-3];

		parent_signal[my_id] = 1; 

		pthread_mutex_unlock(&total_mutex);

		for(int i =0; i<num; i++){

			filled_count.down();

			pthread_mutex_lock(&total_mutex);

			//To handle dummy signal
			if(overflow[0].size() == 0 && overflow[1].size() == 0 && overflow[2].size() == 0 && parent_sent_signal == 1 && buffer.size() == 0){
				
				thread_recieved_signal[my_id] = 1;
				pthread_mutex_unlock(&total_mutex);
				break;
			}

			buffer.pop_back();

			empty_count.up();

			consume[my_id-3]--;

			pthread_mutex_unlock(&total_mutex);
		}

		pthread_cond_signal(&parent_cv);

		sync_parent[my_id].up();

	}

}

bool producers_completed(){

	if(overflow[0].size() == 0 && overflow[1].size() ==0 && overflow[2].size() ==0 && parent_signal[0] == 1 && parent_signal[1] == 1 && parent_signal[2] == 1){
		return 1;
	}

	return 0;

}

bool consumers_completed(){

	if(consume[0] == 0 && consume[1] == 0 && parent_signal[3] == 1 && parent_signal[4] == 1){
		return 1;
	}

	return 0;

}

int main(){

	pthread_t threads[5];

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


	while(1){


		//Accepting inpus
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

			inputs[i] = n;
		}

		//Syncing threads with parent		
		for(int i = 0; i<5; i++){
			
			sync_thread[i].up();
		}
		

		pthread_mutex_lock(&total_mutex);
		
		//To check for premature/complete completion of threads
		while(!producers_completed() && !consumers_completed()){
			pthread_cond_wait(&parent_cv, &total_mutex);
		}
		
		for(int i =0; i<5; i++){
			parent_signal[i] = 0;
		}

		parent_sent_signal = 1;

		//Dummy signals
		filled_count.up();
		filled_count.up();
		empty_count.up();
		empty_count.up();
		empty_count.up();

		pthread_mutex_unlock(&total_mutex);

		//Wair for completion of all threads
		for(int i = 0; i<5; i++){
			sync_parent[i].down();
		}

		//Negate dummy signal that wasn't received
		for(int i = 0; i<5; i++){

			if(i<3 && thread_recieved_signal[i] == 0){
				empty_count.down();
			}
			else if(i>=3 && thread_recieved_signal[i] == 0){
				filled_count.down();
			}
			thread_recieved_signal[i] = 0;

		}



		parent_sent_signal = 0;

		//Display buffer and overflow
		display();

	}	


	return 0;

}