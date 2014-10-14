#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>

// Decl

enum PROCESS_STATE
{
	STATE_NONE,
	STATE_READY,
	STATE_RUNNING,
	STATE_EXIT,
};

// Process

typedef struct _Process{

	// readonly
	char pid[3];
	int arrive_time;
	int service_time;
	int priority;

	// real time
	int state;
	int running_time;

    int complete_time;

} Process;

void reset_process(Process* this)
{
	this->state = STATE_NONE;
	this->running_time = 0;
    this->complete_time = 0;
}

void update_process(Process* this, int dt, int cpu_time)
{
    // arrive
	if(this->state != STATE_READY && (this->arrive_time == cpu_time))
	{
		this->state = STATE_READY;
        printf("[%s] is now READY\n", this->pid);
	}

    // running
    if(this->state == STATE_RUNNING)
    {
        this->running_time += dt;
    }

    // exit
	if(this->state != STATE_EXIT && (this->service_time == this->running_time))
	{
        this->complete_time = cpu_time;
		this->state = STATE_EXIT;
        printf("[%s] is now EXIT on time of [%d]\n", this->pid, this->complete_time);
	}
}

// Global vars
FILE *	input;

Process	processes[260];
int num_process = 0;

// Prototypes

int init_input(char * filename);
int check_process(Process* process);
void print_processes();
void reset_all_processes();

bool all_process_done();

void SJF_simulation();
void SRT_simulation();

int main(int argc, char ** argv)
{
	if(argc < 2)
	{
		fprintf(stderr, "input file must specified\n");
		return -1;
	}
	else
	{
		if(init_input(argv[1]))
		{
			SJF_simulation();
            reset_all_processes();

            SRT_simulation();
		}
	}

	return 0;
}

int init_input(char * filename)
{
	if((input = fopen(filename, "r")))
	{
		char 	line[256];

		num_process = 0;

		while(fgets(line, sizeof(line), input))
		{

			if(line[0] == '#')
			{
				continue;
			}
			else
			{
				int ret = sscanf(line,"%2s %d %d %d\n",
					processes[num_process].pid,
					&(processes[num_process].arrive_time),
					&(processes[num_process].service_time),
					&(processes[num_process].priority)
				);

				reset_process(&processes[num_process]);

				if(ret < 4)
				{
					fprintf(stderr, "invalid format\n");
					continue;
				}

				if(check_process(&processes[num_process]) == false)
				{
					continue;
				}
			}

			num_process++;
		}

		print_processes();
		return true;
	}
	else
	{
		fprintf(stderr, "failed to load input file \'%s\'\n", filename);
		return false;
	}
}

void print_processes()
{
	int i = 0;
	for(i = 0; i < num_process; i++){

		Process* process = &processes[i];

		printf("[%d] pid : %s arrival : %d service : %d priority : %d\n",
			i,
			process->pid,
			process->arrive_time,
			process->service_time,
			process->priority
		);
	}
}

int check_process(Process* process)
{
	int i = 0;
	for(i = 0; i < num_process; i++)
	{
		if(strcmp(processes[i].pid, process->pid) == 0)
		{
			fprintf(stderr, "duplicate process id \'%s\'\n", process->pid);
			return false;
		}
	}

	if( 0 > process->arrive_time || process->arrive_time > 30 )
	{
		fprintf(stderr, "invalid arrive time \'%d\'\n", process->arrive_time);
		return false;
	}

	if( 1 > process->service_time || process->service_time > 30 )
	{
		fprintf(stderr, "invalid service time \'%d\'\n", process->service_time);
		return false;
	}

	if( 1 > process->priority || process->priority > 10 )
	{
		fprintf(stderr, "invalid priority \'%d\'\n", process->priority);
		return false;
	}

	return true;
}

void reset_all_processes()
{
    int i = 0;
    for(i = 0; i < num_process; i++)
    {
        reset_process(&processes[i]);
    }
}

bool all_process_done()
{
	int i=0;
	for(i = 0; i < num_process; i++)
	{
		if(processes[i].state != STATE_EXIT)
			return false;
	}
	return true;
}

void simulation_output(int cpu_time)
{

    float avg_turnaround_time = 0;
    float avg_waiting_time = 0;

    int i = 0;
    for(i = 0; i < num_process; i++)
    {
        printf("[%s]'s complete time is [%d]\n", processes[i].pid, processes[i].complete_time);

        avg_turnaround_time += (processes[i].complete_time - processes[i].arrive_time);
        avg_waiting_time += (processes[i].complete_time - processes[i].arrive_time) - processes[i].service_time;
    }

    printf("\n");

    avg_turnaround_time /= num_process;
    avg_waiting_time /= num_process;

    printf("CPU TIME: %d\n", cpu_time);
    printf("AVERAGE TURNAROUND TIME: %.2f\n", avg_turnaround_time);
    printf("AVERAGE WAITING TIME: %.2f\n", avg_waiting_time);

}

void SJF_simulation()
{
	const int   deltaTime = 1;
	int         cpu_time = 0;

	Process *   cur_process = NULL;

	while(!all_process_done())
    {
        printf(" CPU TIME [%d] \n", cpu_time);
		// update each process
		int i = 0;
		for(i = 0; i < num_process; i++)
		{
			update_process(&processes[i], deltaTime, cpu_time);
		}

		// pick next process
		if( cur_process == NULL || cur_process->state != STATE_RUNNING )
		{
			// ready and shortest
			int target_index = -1;
			int shortest_time = INT_MAX;

			int j = 0;
			for(j = 0; j < num_process; j++)
			{
				if(processes[j].state == STATE_READY)
				{
					if( processes[j].service_time < shortest_time )
					{
						target_index = j;
						shortest_time = processes[j].service_time;
					}
				}
			}

            if(target_index != -1)
            {
                printf("Changing process index of [%d]\n", target_index);
                cur_process = &processes[target_index];
                cur_process->state = STATE_RUNNING;
            }
            else
            {
                break;
            }
		}

		printf("Cur Process is [%s], runnin' time: [%d]\n", cur_process->pid, cur_process->running_time);

		cpu_time += deltaTime;
        printf("=========================\n");
	}

    printf("\n");
    simulation_output(cpu_time);
}

void SRT_simulation()
{
    const int   deltaTime = 1;
    int         cpu_time = 0;

    Process *   cur_process = NULL;

    printf("\nSRT simulating\n\n");

    while(!all_process_done())
    {
        printf(" CPU TIME [%d] \n", cpu_time);
        // update each process
        int i = 0;
        for(i = 0; i < num_process; i++)
        {
            update_process(&processes[i], deltaTime, cpu_time);
        }

        // pick next process
        {
            // ready and shortest
            int target_index = -1;
            int shortest_remaining_time = INT_MAX;

            int j = 0;
            for(j = 0; j < num_process; j++)
            {
                if(processes[j].state == STATE_READY || processes[j].state == STATE_RUNNING)
                {
                    int remaining_time = processes[j].service_time - processes[j].running_time;

                    printf("~Remaining time of [%s] is [%d]\n",processes[j].pid, remaining_time);

                    if( remaining_time < shortest_remaining_time )
                    {
                        target_index = j;
                        shortest_remaining_time = remaining_time;
                    }
                }
            }

            if(target_index != -1)
            {
                printf("Changing process index of [%d]\n", target_index);

                if(cur_process && (STATE_EXIT != cur_process->state))
                    cur_process->state = STATE_READY;
                cur_process = &processes[target_index];
                cur_process->state = STATE_RUNNING;
            }
            else
            {
                break;
            }
        }

        printf("Cur Process is [%s], runnin' time: [%d]\n", cur_process->pid, cur_process->running_time);
        cpu_time += deltaTime;
        printf("=========================\n");
    }

    printf("\n");
    simulation_output(cpu_time);
}