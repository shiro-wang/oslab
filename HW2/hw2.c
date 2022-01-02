#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
int total[1024] ={0};
int max_buffer[4]={0};
int min_buffer[4]={0};
int total_max = 0;
int total_min = RAND_MAX;
int maxb = 0;
int minb = 0;
sem_t max_buffer_full;
sem_t min_buffer_full;
sem_t max_buffer_mutex;
sem_t min_buffer_mutex;
void producer(void *arg){
	int num = *(int*)arg;
	int min = RAND_MAX;
	int max = 0;
	for(int i=0;i<256;i++){
		if(max < total[i+num*256]){
			max = total[i+num*256];
		}
		if(min > total[i+num*256]){
			min = total[i+num*256];
		}
	}
	printf("Temporary max = %d, and min = %d at %d\n",max,min,num);
	sem_wait(&max_buffer_mutex);
	printf("Producer: put %d into max_buffer at %d\n",max,num);
	max_buffer[++maxb] = max;
	sem_post(&max_buffer_full);
	sem_post(&max_buffer_mutex);

	sem_wait(&min_buffer_mutex);
	min_buffer[++minb] = min;
	printf("Producer: put %d into min_buffer at %d\n",min,num);
	sem_post(&min_buffer_full);
	sem_post(&min_buffer_mutex);
	pthread_exit(NULL);
}
void consumer(void *arg){
	int num = *(int*)arg;
	if(num == 0){
		sem_wait(&max_buffer_full);
		sem_wait(&max_buffer_mutex);
		//printf("max: %d maxbu:%d\n",total_max , max_buffer[maxb]);
		if(total_max < max_buffer[maxb]){
			total_max = max_buffer[maxb];
			printf("Consumer: updated! maximum = %d\n",total_max);
		}
		maxb -= 1;
		sem_post(&max_buffer_mutex);
	}else{
		sem_wait(&min_buffer_full);
		sem_wait(&min_buffer_mutex);
		if(total_min > min_buffer[minb]){
			total_min = min_buffer[minb];
			printf("Consumer: updated! miniimum = %d\n",total_min);
		}
		minb -= 1;
		sem_post(&min_buffer_mutex);
	}
	pthread_exit(NULL);
}
int main(){
	sem_destroy(&max_buffer_full);
	sem_destroy(&min_buffer_full);
	sem_destroy(&max_buffer_mutex);
	sem_destroy(&min_buffer_mutex);
	srand(time(NULL));
	//printf("%d\n",RAND_MAX);
	for(int i=0;i<1024;i++){
		total[i] = rand();
	}
	sem_init(&max_buffer_full, 0, 0);
	sem_init(&min_buffer_full, 0, 0);
	sem_init(&max_buffer_mutex, 0, 1);
	sem_init(&min_buffer_mutex, 0, 1);
	int t0=0, t1=1, t2=2, t3=3, t4=0, t5=1;
	pthread_t id[6];
	pthread_create(&id[0],NULL,(void*)producer,&t0);
	pthread_create(&id[1],NULL,(void*)producer,&t1);
	pthread_create(&id[2],NULL,(void*)producer,&t2);
	pthread_create(&id[3],NULL,(void*)producer,&t3);
	pthread_create(&id[4],NULL,(void*)consumer,&t4);
	pthread_create(&id[5],NULL,(void*)consumer,&t5);
	for(int i=0;i<6;i++){
		pthread_join(id[i],NULL);
	}
	printf("\nmax is %d, min is %d\n",total_max, total_min);
	sem_destroy(&max_buffer_full);
	sem_destroy(&min_buffer_full);
	sem_destroy(&max_buffer_mutex);
	sem_destroy(&min_buffer_mutex);
	pthread_exit(NULL);
}
