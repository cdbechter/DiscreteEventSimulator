/**********************
 * Chris Bechter
 * 02/06/2019
 * CIS3207
 **********************/ 
#include "my_file.h"

int main() {
	char str[80];
    clock_t begin = clock();

	SEED = atoi(getValue("config.txt","SEED", 0, 1));
	INIT_TIME = atoi(getValue("config.txt","INIT_TIME", 0, 1));
	FIN_TIME = atoi(getValue("config.txt","FIN_TIME", 0, 1));
	ARRIVE_MIN = atoi(getValue("config.txt","ARRIVE_MIN", 0, 1));
	ARRIVE_MAX = atoi(getValue("config.txt","ARRIVE_MAX", 0, 1));
	QUIT_PROB = atof(getValue("config.txt","QUIT_PROB", 0, 1));
	CPU_MIN = atoi(getValue("config.txt","CPU_MIN", 0, 1));
	CPU_MAX = atoi(getValue("config.txt","CPU_MAX", 0, 1));
	DISK1_MIN = atoi(getValue("config.txt","DISK1_MIN", 0, 1));
	DISK1_MAX = atoi(getValue("config.txt","DISK1_MAX", 0, 1));
	DISK2_MIN = atoi(getValue("config.txt","DISK2_MIN", 0, 1));
	DISK2_MAX = atoi(getValue("config.txt","DISK2_MAX", 0, 1));

	/* debugging the getValue functions in readConfig */
	/* now printing out to stats to runs.txt */
	sprintf(str, "Seed: %d", SEED);
	recordStats(str);
	sprintf(str, "INIT_TIME: %d", INIT_TIME);
	recordStats(str);
	sprintf(str, "FIN_TIME: %d", FIN_TIME);
	recordStats(str);
	sprintf(str, "ARRIVE_MIN: %d", ARRIVE_MIN);
	recordStats(str);
	sprintf(str, "ARRIVE_MAX: %d", ARRIVE_MAX);
	recordStats(str);
	sprintf(str, "QUIT_PROB: %f", QUIT_PROB);
	recordStats(str);
	sprintf(str, "CPU_MIN: %d", CPU_MIN);
	recordStats(str);
	sprintf(str, "CPU_MAX: %d", CPU_MAX);
	recordStats(str);
	sprintf(str, "DISK1_MIN: %d", DISK1_MIN);
	recordStats(str);
	sprintf(str, "DISK1_MAX: %d", DISK1_MAX);
	recordStats(str);
	sprintf(str, "DISK2_MIN: %d", DISK2_MIN);
	recordStats(str);
	sprintf(str, "DISK2_MAX: %d\n", DISK2_MAX);
	recordStats(str);

	/* this randomization isn't needed but i left it in anyway
    time_t t;
	srand((unsigned) time(&t));
    SEED = rand() % 50;
    printf("Random Seed: %d\n\n", SEED); */

	srand(SEED);
	currentTime = INIT_TIME;
	total_time = FIN_TIME - INIT_TIME;

	/*add two events to priorty queue, job1 arrival and simulation finished*/
	priority_Push(&eventQ, createEvent(INIT_TIME, 1, ARRIVAL), &eventNodes);
	priority_Push(&eventQ, createEvent(FIN_TIME, 0, SIMULATION_END), &eventNodes);

    /* while loop for priority queue to push into cpu */
	while(eventNodes != 0 && currentTime < FIN_TIME) {
		//pop events from event queue and place them inside cpu queue
		task = queue_Pop(&eventQ, &eventNodes); //change 1
		currentTime = task.time;
		
        //Event Handler Switch
		switch(task.type) {
			case ARRIVAL: process_CPU(task); 
            break;
			case FINISH: process_CPU(task); 
            break;
			case DISK_ARRIVAL: process_DISK(task); 
            break;
			case DISK_FINISH: process_DISK(task); 
            break;
			case SIMULATION_END: 
            break;
		}
		/* for the statistics */
		iterations_total++;
		priority_average += priority_total;
		cpu_average += cpu_total;
		disk1_average += disk1_total;
		disk2_average += disk2_total;
		findMax();
	}
	printStats();
	clock_t end = clock();
	double time_spent = (double)(end-begin) / CLOCKS_PER_SEC;
	printf("\nTotal time spent executing: %fs\n\n", time_spent);
	sprintf(str, "Total time spent: %fs", time_spent);
	recordStats(str);
	sprintf(str, " ----------------------------------------------");
	recordStats(str);
	return 0;
}

/* first in first out queue used for the CPU and Disk processes */
void fifo_Push(node** head_ref, event data, int* total) {
	node* new_node = (node*) malloc(sizeof(node));
	new_node->job = data;
	new_node->next = NULL;

    /* making sure to start at beginning of queue */	
	node *last = *head_ref;
	(*total)++; //to add a new event in linked list
	if (*head_ref == NULL) {
		*head_ref = new_node;
		return;
	}
	while (last->next != NULL) {
		last = last->next;
    }
	last->next = new_node;
	return;
}

/* priority queue for the event queue, sorts, and keeps track of the times */
void priority_Push(node** head_ref, event data, int* total) {
	priority_total++;
	node* temp = (node*) malloc(sizeof(node));
	temp->job = data;
	temp->next = NULL;

	int priority = temp->job.time; //makes the time the priority of the queue
	(*total)++;

	/* start at beginning of list */	
	node *last = *head_ref;

	/* actual sorting to make sure the queue stays priority */
	if (*head_ref == NULL) { 
		*head_ref = temp;
		return;
	} else if(priority < last->job.time) {
		temp->next = last;
		*head_ref = temp; // have the pointer to the linkedlist point to the very start of the node which would be temp
		return;
	}

	/* traverse till we get to the end of list OR if we find the correct position where the node belongs */
	while (last->next != NULL && priority >= last->next->job.time) {
		last = last->next;
    }
	temp->next = last->next;
	last->next = temp;
	return;
}

/* pop first node (header) of FIFO QUEUE for each process */
event queue_Pop(node** head_ref, int* total) {
	node *temp = *head_ref;
	*head_ref = (*head_ref)->next;
	event job = temp->job;
	free(temp); //frees memory temp
	(*total)--; //decreasing size of queue
	return job;
}

/* event handler to create the events for the queues at each iteration of CPU and Disk flow */
event createEvent(int time, int jobNumber, int type) {
	event job;
	job.time = time;
	job.jobNumber = jobNumber;
	job.type = type;
	return job; 
}

/* randomly generate a number using config values for each function */
int randomNumber(int min, int max) {
	return (rand() % (max - min + 1)) + min; 
}

/* CPU process: In the CPU process, after the switch hits ARRIVAL for the CPU, a job is passed from the event 
	priority to the CPU fifo and then into the CPU itself. This records the event, creates another event job 
	to send to the priority queue, raises the CPU counter and figures out if the CPU is IDLE or BUSY. If IDLE, 
	the job gets popped and the CPU, decreases the counter, and becomes busy. If BUSY, does nothing, stays in the 
	queue. The QUIT_PROB then hits when popped to see if the job exits the simulation, if not, pushed to the DISK 
	process to decide which DISK to proceed with */
void process_CPU(event task) {
	char str[80];
	if(task.type == ARRIVAL) {
		sprintf(str, "At time %d, Job%d arrives at CPU Queue.", currentTime, task.jobNumber);
		recordEvent(str);
		
		/* determine the arrival time for the next job to enter the system and add it as an event to the priority queue */
		randTime = randomNumber(ARRIVE_MIN, ARRIVE_MAX) + currentTime; numJobs++;
		priority_Push(&eventQ, createEvent(randTime, numJobs, ARRIVAL), &eventNodes); 
		fifo_Push(&CPU, task, &cpuNodes); // job sent to the CPU fifo
		cpu_total++;

		/* When jobs reach some component (CPU, disk1, or disk2), if that component is free, the job begins service right away otherwise 
			this process is busy so it waits in the queue */ 
		if (cpuState == IDLE) {
			task = queue_Pop(&CPU, &cpuNodes);
			cpu_total--;
			randTime = randomNumber(CPU_MIN, CPU_MAX) + currentTime;
			sprintf(str, "At time %d, Job%d is in CPU.", currentTime, task.jobNumber);
			priority_Push(&eventQ, createEvent(randTime, task.jobNumber, FINISH), &eventNodes); // adds event to priorty queue about when cpu finishes
			recordEvent(str);
			cpuState = BUSY;
			cpu_old = currentTime;
		}
	} else {
		cpuState = IDLE;
		int time = currentTime - cpu_old;
		cpu_time += time;
		cpu_job++;
		if(time > cpu_job_max)
			cpu_job_max = time;
		
		sprintf(str, "At time %d, Job%d finishes at CPU.", currentTime, task.jobNumber);
		recordEvent(str);
		/* once the job hs finished, it now redetermines the quit probability from the config file to see if 
			the job exits or goes to another queue for re-processing */
		double quit = rand() <  QUIT_PROB * ((double)RAND_MAX + 1.0);
		if(quit) {
			sprintf(str, "At time %d, Job%d exits.", currentTime, task.jobNumber);
			recordEvent(str);
		} else {
			/* If the job event doesn't exit because its not less then the probability to quit, therefore not exiting, 
				it now gets pushed the the DISK's and once there has to decide which disk has the shortest queue, placing 
				the event into that queue, or randomly picking one if they are equal sized */
			//randTime = randomNumber(ARRIVE_MIN, ARRIVE_MAX) + currentTime;
			priority_Push(&eventQ, createEvent(currentTime, task.jobNumber, DISK_ARRIVAL), &eventNodes); //////////changed this
		}
	}	
}


/* Disk processing function: In this function, whatever is spit out by the CPU process, that hasnt hit the QUI_PROB
	will be sent to the disk process. After the switch hits the DISK_ARRIVAL, this function starts at 0 for which queue 
	to send the job too, then after an if statement to find out which queue has the least amount of nodes, sends the job to
	the queue. Then it pushes the new job, pops the current job, and creates a new event to put into the priority queue. It rasies 
	the counter for the DISK it's sent too for statistices, and becomes BUSY. If the DISKs are both BUSY, it queues up the job
	up until the current job is finished and gets put back into the CPU fifo queue. */
void process_DISK(event task) {
	char str[80];
	if (task.type == DISK_ARRIVAL) {
		int sendTo = 0; // holder that determines what disk the event will be sent to

        /* If statement to check each disks queue size and pick which one to use on 
            who has the smaller or less amount of nodes/events in it unless both are equal, 
            then just randomly pick a disc to send process too */
		if(disk1Nodes < disk2Nodes) {
			sendTo = 1;
		} else if(disk1Nodes > disk2Nodes) {
			sendTo = 2;
		} else {
			sendTo = (rand() <  0.5 * ((double)RAND_MAX + 1.0)) + 1;
		}

        /* If statement for each disk after program decides which to send too, to 
            pop the next in fifo queue if the disk is IDLE and then return busy after 
            creating another event */
		if(sendTo == 1) {
			fifo_Push(&DISK_1, task, &disk1Nodes);
			sprintf(str, "At time %d, Job%d arrives at Disk 1.", currentTime, task.jobNumber);
			recordEvent(str);
			disk1_total++;
			if(disk1State == IDLE){
				event job = queue_Pop(&DISK_1, &disk1Nodes);
				randTime = randomNumber(DISK1_MIN, DISK1_MAX) + currentTime;
				priority_Push(&eventQ, createEvent(randTime, job.jobNumber, DISK_FINISH), &eventNodes); ////////changed this
				disk1 = task.jobNumber;
				disk1State = BUSY;
				disk1_old = currentTime;
			} 
		} else if(sendTo == 2) {
			fifo_Push(&DISK_2, task, &disk2Nodes);
			sprintf(str, "At time %d, Job%d arrives at Disk 2.", currentTime, task.jobNumber);
			recordEvent(str);
			disk2_total++;
			if(disk2State == IDLE) {
				event job = queue_Pop(&DISK_2, &disk2Nodes);
				randTime = randomNumber(DISK2_MIN, DISK2_MAX) + currentTime;
				priority_Push(&eventQ, createEvent(randTime, job.jobNumber, DISK_FINISH), &eventNodes);
				disk2 = task.jobNumber;
				disk2State = BUSY;	
				disk2_old = currentTime;
			}
		}
	} else {
		//randTime = randomNumber(ARRIVE_MIN, ARRIVE_MAX) + currentTime;
		priority_Push(&eventQ, createEvent(currentTime, task.jobNumber, ARRIVAL), &eventNodes); 
		if(disk1 == task.jobNumber) {
			int time = currentTime - disk1_old;
			disk1_time += time;
			disk1State = IDLE;
			disk1Nodes = 0;
			disk1_job++;
			if(time > disk1_job_max)
				disk1_job_max = time;
			sprintf(str, "At time %d, Job%d finished at Disk 1.", currentTime, task.jobNumber);
			recordEvent(str);
		} else if(disk2 == task.jobNumber) {
				int time = currentTime - disk2_old;
				disk2_time += time;
				disk2State = IDLE;
				disk2Nodes = 0;
				disk2_job++;
				if(time > disk2_job_max)
					disk2_job_max = time;
			sprintf(str, "At time %d, Job%d finished at Disk 2.", currentTime, task.jobNumber);
			recordEvent(str);
		}
	}
}

/* finds all the max values for the statistics */
void findMax() {
	if(priority_max < priority_total)
		priority_max = priority_total;

	if(cpu_max < cpu_total)
		cpu_max = cpu_total;

	if(disk1_max < disk1_total)
		disk1_max = disk1_total;

	if(disk2_max < disk2_total)
		disk2_max = disk2_total;
}

/* simple recordEvent function that opens or creates a new file to append the data 
	too thats collected through the CPU/DISK processes */
void recordEvent(char *line) { 
	FILE * fp;
	fp = fopen ("log.txt","a");
	fprintf (fp,"%s\n",line); 
	fclose (fp);
}

/* Calculating all the statistics needed for the assignment */
void printStats() {
	char str[80];

	//Average Size of Each Queue
	sprintf(str, "Average Size of Each Queue.");
	recordStats(str);
	sprintf(str, "Priority Queue average %f", priority_average/iterations_total);
	recordStats(str);
	sprintf(str, "CPU average %f", cpu_average/iterations_total);
	recordStats(str);
	sprintf(str, "Disk1 average %f", disk1_average/iterations_total);
	recordStats(str);
	sprintf(str, "Disk2 average %f\n", disk2_average/iterations_total);
	recordStats(str);

	//Max Size of Each Queue
	sprintf(str, "Max Size of Each Queue.");
	recordStats(str);
	sprintf(str, "Priority Queue max %d", priority_max);
	recordStats(str);
	sprintf(str, "CPU max %d", cpu_max);
	recordStats(str);
	sprintf(str, "Disk1 max %d", disk1_max);
	recordStats(str);
	sprintf(str, "Disk2 max %d\n", disk2_max);
	recordStats(str);

	//Average Response Time
	sprintf(str, "Average Response Time.");
	recordStats(str);
	sprintf(str, "CPU Average Response Time %f", cpu_time/cpu_job);
	recordStats(str);
	sprintf(str, "Disk1 Average Response Time %f", disk1_time/disk1_job);
	recordStats(str);
	sprintf(str, "Disk2 Average Response Time %f\n", disk2_time/disk2_job);
	recordStats(str);

	//Max Response Time
	sprintf(str, "Max Response Time.");
	recordStats(str);
	sprintf(str, "CPU max Response Time %d", cpu_job_max);
	recordStats(str);
	sprintf(str, "Disk1 max Response Time %d", disk1_job_max);
	recordStats(str);
	sprintf(str, "Disk2 max Response Time %d\n", disk2_job_max);
	recordStats(str);

	//Utilization
	sprintf(str, "Utilization. Process Time / Total Time");
	recordStats(str);
	sprintf(str, "CPU Utilization %f, %f: %f", cpu_time, total_time, cpu_time/total_time);
	recordStats(str);
	sprintf(str, "Disk1 Utilization %f, %f: %f", disk1_time, total_time, disk1_time/total_time);
	recordStats(str);
	sprintf(str, "Disk2 Utilization %f, %f: %f\n", disk2_time, total_time, disk2_time/total_time);
	recordStats(str);

	//Throughput
	sprintf(str, "Throughput. Process Job count / Total Time");
	recordStats(str);
	sprintf(str, "Throughput of CPU %d, %f,: %f", cpu_job, total_time, cpu_job/total_time);
	recordStats(str);
	sprintf(str, "Throughput of DISK1 %d, %f,: %f", disk1_job, total_time, disk1_job/total_time);
	recordStats(str);
	sprintf(str, "Throughput of DISK2 %d, %f,: %f\n", disk2_job, total_time, disk2_job/total_time);
	recordStats(str);
}

void recordStats(char *line) { 
	FILE * fp;
	fp = fopen ("runslog.txt","a");
	fprintf (fp,"%s\n",line); 
	fclose (fp);
}