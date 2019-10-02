#ifndef MYFILE_H_
#define MYFILE_H_
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include "readConfig.c"

/* definitions for the process States */ 
#define ARRIVAL 0
#define FINISH 1
#define DISK_ARRIVAL 2
#define DISK_FINISH 3
#define SIMULATION_END 4
#define IDLE 0
#define BUSY 1

/* global config variables */
int SEED = 0; 
int INIT_TIME = 0;
int	FIN_TIME = 0;
int ARRIVE_MIN = 0;
int ARRIVE_MAX = 0;
float QUIT_PROB = 0;
int CPU_MIN = 0;
int CPU_MAX = 0;
int DISK1_MIN = 0;
int DISK1_MAX = 0;
int DISK2_MIN = 0;
int DISK2_MAX = 0;

/* event struct for the jobs created */
typedef struct event{
	int time; 
	int jobNumber;
	int type;
}event;

/* linked list node for the priority and fifo queues used for processes */
typedef struct node{
	struct event job; //event information
	struct node *next;
}node;

/* creates an event with 3 inputs, all ints (time, job number, job type) */
event createEvent(int, int, int);

/* first in first out linked list push */
void fifo_Push(node**, event, int*);

/* priority queue linked list push */
void priority_Push(node**, event, int*);

/* pop for both linked list queues */
event queue_Pop(node**, int*);

/* initializing 3 first in first out linked lists to null */
node* CPU = NULL, *DISK_1 = NULL, *DISK_2 = NULL;

/* initializing the event priority queue linked list to null */
node* eventQ = NULL;

/* function for the random number between a min and max (cpu/disk) */
int randomNumber(int, int);

/* functions that deal with the event in stages of CPU and DISKs */
void process_CPU(event);
void process_DISK(event);

/* records event function for statistics */
void recordEvent(char*);
void recordStats(char*);

/* find the max of each statistics */
void findMax();

/* print all the stats */
void printStats();

/* global variable for the times */
int currentTime = 0;
int randTime = 0;

/* initializng the events as tasks */
event task;

/* sets each of the node stages to 0 except number of jobs which starts at 1 */
int cpuNodes = 0, disk1Nodes = 0, disk2Nodes = 0, eventNodes = 0, numJobs = 1;

/* sets up the state each stage to IDLE (not busy) */
int cpuState = IDLE, disk1State = IDLE, disk2State = IDLE;

/***************************************
 * Statistics Variables
 ***************************************/

int disk1, disk2;
int job_Numbers = 0;
int priority_total = 0, cpu_total = 0, disk1_total = 0, disk2_total = 0;
int priority_max = 0, cpu_max = 0, disk1_max = 0, disk2_max = 0;
int iterations_total = 0; //total
float priority_average = 0, cpu_average = 0, disk1_average = 0, disk2_average = 0;
float cpu_time = 0, disk1_time = 0, disk2_time = 0;
int cpu_old = 0, disk1_old = 0, disk2_old = 0;
float total_time = 0;
int cpu_job = 0, disk1_job = 0, disk2_job = 0;
int cpu_job_max = 0, disk1_job_max = 0, disk2_job_max = 0;



#endif