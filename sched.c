#include <stdio.h>
#include <stdbool.h>

typedef struct{
	
	char pid[3];

	int arrive_time;
	int service_time;
	int priority;

} Process;

FILE *	input;

Process	processes[260];
int num_proc = 0;

int init_input(char * filename);
void print_processes();
int check_process(Process* process);

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

		}
	}

	return 0;
}

int init_input(char * filename)
{
	if(input = fopen(filename, "r"))
	{
		char 	line[256];

		num_proc = 0;

		while(fgets(line, sizeof(line), input))
		{

			if(line[0] == '#')
			{
				continue;
			}
			else
			{
				int ret = sscanf(line,"%2s %d %d %d\n", 
					processes[num_proc].pid, 
					&(processes[num_proc].arrive_time), 
					&(processes[num_proc].service_time), 
					&(processes[num_proc].priority)
				);

				if(ret < 4)
				{
					fprintf(stderr, "invalid format\n");
					continue;
				}

				if(check_process(&processes[num_proc]) == false)
				{
					continue;
				}
			}

			num_proc++;
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
	for(i = 0; i < num_proc; i++){

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
	for(i = 0; i < num_proc; i++)
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