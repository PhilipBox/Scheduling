#include <stdio.h>
#include <stdlib.h>
#define SIZE 10

typedef struct pcb {
	int pid;
	double priority;
	int arrival_time;
	int burst_time;
	int remain_time;
	int waiting_time;
	int turnaround_time;
	int response_time;
	int check_run;
	int finished;

	struct pcb *next;
}PCB;

PCB job_queue[SIZE];
PCB *ready_queue;

// prototype of functions
void FCFS();
void input_readyQueue(int);
int Check_finish();
void Check_process(PCB*, int, int);
double total_Waiting_TIME();
int total_Response_TIME();
int total_TurnAround_TIME();
PCB *Q_in_last_process();

// global variable declare
int total_burst = 0;
int cpu_time;
int cpu_idle = 0;
int total_process = 0;

// FILE declare to use
FILE *fp, *wp;

int main(int argc, char *argv[])
{
	if (argc != 3) // check parameters 
	{
		printf("ERROR! The number of parameters ");
	}
	else {
		ready_queue = NULL;

		fp = fopen(argv[1], "r");
		wp = fopen(argv[2], "w");

		if (fp == NULL)
		{
			printf("ERROR! File read failed! \n");
			exit(0);
		}

		int i = 0; // use to while loop
				   // while : load the process data from input.txt file
		while (fscanf(fp, "%d %lf %d %d\n", &job_queue[i].pid, &job_queue[i].priority, &job_queue[i].arrival_time, &job_queue[i].burst_time) != EOF)
		{
			total_burst = total_burst + job_queue[i].burst_time;
			total_process++;
			i++;
		}

		printf("Scheduling : FCFS\n");
		fprintf(wp, "Scheduling : FCFS\n");
		printf("==========================================\n");
		fprintf(wp, "==========================================\n");
		FCFS(); // call of FCFS function
	}
	fclose(fp);
	fclose(wp);
}//end main function



 /*
 *  purpose: First-Come,First-Served Scheduling.
 *  @input: void(none)
 *  @output: void(none)
 */
void FCFS()
{
	PCB *pcb, *pre_pcb;
	cpu_time = total_burst;
	pcb = ready_queue;
	pre_pcb = ready_queue;

	for (int i = 0; i <= cpu_time; i++)
	{
		//Waiting time check 
		PCB *waiting;
		waiting = ready_queue;
		while (waiting)
		{
			if (!((*waiting).finished || (*waiting).check_run))
			{
				(*waiting).waiting_time++;
			}
			waiting = (*waiting).next;
		}
		//
		input_readyQueue(i);

		if (pcb == NULL)
		{
			if (pre_pcb == NULL)
			{
				pcb = Q_in_last_process();
				if (Q_in_last_process() != NULL)
				{
					Check_process(pcb, i, 0);
					printf("<time %d> process %d is running\n", i, (*pcb).pid);
					fprintf(wp, "<time %d> process %d is running\n", i, (*pcb).pid);
					(*pcb).remain_time--;
					continue;
				}
			}
			else
			{
				PCB *search = ready_queue;
				pcb = Q_in_last_process();
				while (search)
				{
					if ((*search).pid == (*pre_pcb).pid)
					{
						if ((*search).next != NULL)
						{
							pcb = (*search).next;
							break;
						}
					}
					search = (*search).next;
				}
				if ((*pre_pcb).pid != (*pcb).pid)
				{
					Q_in_last_process();
					printf("<time %d> process %d is running\n", i, (*pcb).pid);
					fprintf(wp, "<time %d> process %d is running\n", i, (*pcb).pid);
					(*pcb).remain_time--;
					continue;
				}
				pcb = NULL;
			}
			cpu_time++;
			cpu_idle++;
			printf("<time %d> ---- System is idle ----\n", i);
			fprintf(wp, "<time %d> ---- System is idle ----\n", i);
		}
		else
		{
			if ((*pcb).remain_time == 0)
			{
				(*pcb).finished = 1;
				(*pcb).check_run = 0;
				printf("<time %d> process %d is finished\n", i, (*pcb).pid);
				fprintf(wp, "<time %d>  process %d is finished\n", i, (*pcb).pid);
				(*pcb).turnaround_time = i - (*pcb).arrival_time;
				pre_pcb = pcb;
				pcb = (*pcb).next;
				if (pcb == NULL)
				{
					if (Check_finish() == total_process)
						break;
					i--;
					continue;
				}
				Check_process(pcb, i, 1);
			}
			printf("<time %d> process %d is running\n", i, (*pcb).pid);
			fprintf(wp, "<time %d> process %d is running\n", i, (*pcb).pid);
			(*pcb).remain_time--;
		}
	}
	printf("==============================================\n");
	fprintf(wp, "==============================================\n");
	printf("Average response Time : %lf\n", (double)total_Response_TIME() / total_process);
	fprintf(wp, "Average response Time : %lf\n", (double)total_Response_TIME() / total_process);
	printf("Average waiting Time : %lf\n", (double)total_Waiting_TIME() / total_process);
	fprintf(wp, "Average waiting Time : %lf\n", total_Waiting_TIME() / total_process);
	printf("Average turnaround Time : %lf\n", (double)total_TurnAround_TIME() / total_process);
	fprintf(wp, "Average turnaround Time : %lf\n", (double)total_TurnAround_TIME() / total_process);
	printf("Average CPU usage : %lf\n", (double)(cpu_time - cpu_idle) / (cpu_time) * 100);
	fprintf(wp, "Average CPU usage : %lf\n", (double)(cpu_time - cpu_idle) / (cpu_time) * 100);

}//end FCFS


 /*
 *  purpose: load the process to ready queue from job queue
 *  @input: int time
 *  @output: void(none)
 */
void input_readyQueue(int time)
{
	PCB *check_pcb = ready_queue;
	PCB *enQueue, *pcb = ready_queue;

	for (int i = 0; i < total_process; i++)
	{
		if (time == job_queue[i].arrival_time)
		{
			while (check_pcb)
			{
				if ((*check_pcb).pid == job_queue[i].pid)
					return;
				check_pcb = (*check_pcb).next;
			}

			printf("<time %d> [new arrival] process %d\n", time, job_queue[i].pid);
			fprintf(wp, "<time %d> [new arrival] process %d\n", time, job_queue[i].pid);

			enQueue = (PCB*)malloc(sizeof(PCB));

			(*enQueue).pid = job_queue[i].pid;
			(*enQueue).priority = job_queue[i].priority;
			(*enQueue).arrival_time = job_queue[i].arrival_time;
			(*enQueue).burst_time = job_queue[i].burst_time;
			(*enQueue).remain_time = job_queue[i].burst_time;
			(*enQueue).waiting_time = job_queue[i].waiting_time;
			(*enQueue).turnaround_time = job_queue[i].turnaround_time;
			(*enQueue).response_time = job_queue[i].response_time;
			(*enQueue).check_run = job_queue[i].check_run;
			(*enQueue).finished = job_queue[i].finished;
			(*enQueue).next = NULL;

			while (pcb)
			{
				if ((*pcb).pid == (*enQueue).pid)
					return;
				pcb = (*pcb).next;
			}

			pcb = ready_queue;
			if (ready_queue == NULL)
			{
				ready_queue = enQueue;
				return;
			}

			while ((*pcb).next != NULL)
				pcb = (*pcb).next;
			(*pcb).next = enQueue;

		}// end if 
	}// end for loop (total process_loop)

}//end input_readyQueue function



 /*
 *  purpose: return to last process in Queue
 *  @input: void(none)
 *  @output: PCB pre_pcb
 */
PCB *Q_in_last_process()
{
	PCB *pcb = ready_queue;
	PCB *pre_pcb = ready_queue;

	while (pcb)
	{
		pre_pcb = pcb;
		pcb = (*pcb).next;
	}
	return pre_pcb;
}//end Q_in_last_process function


 /*
 *  purpose: Check to finish
 *  @input: void(none)
 *  @output: int last_PCB
 */
int Check_finish()
{
	int last_PCB = 0;
	PCB *pcb = ready_queue;

	while (pcb)
	{
		if ((*pcb).finished == 1)
			last_PCB++;
		pcb = (*pcb).next;
	}
	return last_PCB;
}//end Check_finish function


 /*
 *  purpose: Check process to Context Switch
 *  @input: PCB *pcb, int flag, int timn,
 *  @output: void(none)
 */
void Check_process(PCB *pcb, int time, int flag)
{
	(*pcb).check_run = 1;
	if (flag == 1)
	{
		printf("------------------------(Context-Switch)\n");
		fprintf(wp, "------------------------(Context-Switch)\n");
	}

	printf("<time %d> [Start] process %d\n", time, (*pcb).pid);
	fprintf(wp, "<time %d> [Start] process %d\n", time, (*pcb).pid);

	if ((*pcb).remain_time == (*pcb).burst_time)
		(*pcb).response_time = time - (*pcb).arrival_time;
}//end Check_process



 /*
 *  purpose: compute to total waiting time
 *  @input: void(none)
 *  @output: int total_waiting_time
 */
double total_Waiting_TIME()
{
	PCB *pcb = ready_queue;
	double total_waiting_time = 0;

	while (pcb) {
		total_waiting_time += (*pcb).waiting_time;
		pcb = (*pcb).next;
	}
	return total_waiting_time;
}// end total_Waiting_TIME


 /*
 *  purpose: compute to total response time
 *  @input: void(none)
 *  @output: int total_response_time
 */
int total_Response_TIME()
{
	PCB *pcb = ready_queue;
	int total_response_time = 0;

	while (pcb) {
		total_response_time += (*pcb).response_time;
		pcb = (*pcb).next;
	}
	return total_response_time;
}// end total_Response_TIME



 /*
 *  purpose: compute to total turnAround time
 *  @input: void(none)
 *  @output: int total_turnAround_time
 */
int total_TurnAround_TIME()
{
	PCB *pcb;
	int total_turnAround_time = 0;
	pcb = ready_queue;

	while (pcb) {
		total_turnAround_time  += (*pcb).turnaround_time;
		pcb = (*pcb).next;
	}
	return total_turnAround_time;
}//end total_TurnAround_TIME