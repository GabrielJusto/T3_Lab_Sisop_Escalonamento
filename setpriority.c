#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/sched.h>



char* buffer = NULL;
int bufferIndex = 0;
int bufferSize = 0;
int nThreads = 0;
int running = 0;
pthread_mutex_t mutex;

void *run(void *data)
{

	/*Garante que todas as threads comecem ao mesmo tempo*/
	running ++;
	while(running < nThreads){}

	intptr_t id = (intptr_t) data;
	while(bufferIndex < bufferSize)
	{
		pthread_mutex_lock(&mutex);
		buffer[bufferIndex] = 'A' + id;
		bufferIndex ++;
		pthread_mutex_unlock(&mutex);

	}
	return 0;
}

void print_sched(int policy)
{
	int priority_min, priority_max;

	switch(policy){
		case SCHED_DEADLINE:
			printf("SCHED_DEADLINE");
			break;
		case SCHED_FIFO:
			printf("SCHED_FIFO");
			break;
		case SCHED_RR:
			printf("SCHED_RR");
			break;
		case SCHED_NORMAL:
			printf("SCHED_OTHER");
			break;
		case SCHED_BATCH:
			printf("SCHED_BATCH");
			break;
		case SCHED_IDLE:
			printf("SCHED_IDLE");
			break;
		default:
			printf("unknown\n");
	}
	priority_min = sched_get_priority_min(policy);
	priority_max = sched_get_priority_max(policy);
	printf(" PRI_MIN: %d PRI_MAX: %d\n", priority_min, priority_max);
}

int setpriority(pthread_t *thr, int newpolicy, int newpriority)
{
	int policy, ret;
	struct sched_param param;

	if (newpriority > sched_get_priority_max(newpolicy) || newpriority < sched_get_priority_min(newpolicy)){
		printf("Invalid priority: MIN: %d, MAX: %d", sched_get_priority_min(newpolicy), sched_get_priority_max(newpolicy));

		return -1;
	}

	pthread_getschedparam(*thr, &policy, &param);
	printf("current: ");
	print_sched(policy);

	param.sched_priority = newpriority;
	ret = pthread_setschedparam(*thr, newpolicy, &param);
	if (ret != 0)
		perror("perror(): ");

	pthread_getschedparam(*thr, &policy, &param);
	printf("new: ");
	print_sched(policy);

	return 0;
}

int main(int argc, char **argv)
{
	
	

	if (argc < 5){
		printf("usage: ./%s <número_de_threads> <tamanho_do_buffer_global_em_kilobytes> <politica> <prioridade>\n\n", argv[0]);

		return 0;
	}

	pthread_mutex_init(&mutex, NULL);

	
	int policy;
	int priority;


	nThreads = atoi(argv[1]);
	bufferSize = atoi(argv[2]) * 1000;
	policy = atoi(argv[3]);
	priority = atoi(argv[4 ]);

	buffer = malloc(bufferSize);

	pthread_t thr [nThreads];

	for(int i =0; i<nThreads; i++)
		pthread_create(&thr[i], NULL, run, (void *)(intptr_t)i);

	for(int i =0; i<nThreads; i++)
		pthread_join(thr[i], NULL);


	printf("Buffer: [");

	for(int i=0; i<bufferSize; i++)
		printf(" %c", buffer[i]);

	printf(" ]");

	// setpriority(&thr, SCHED_FIFO, 1);
	// sleep(timesleep);
	
	return 0;
}
