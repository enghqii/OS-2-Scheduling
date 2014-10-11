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

int init_input(char * filename);
void print_process(Process* process);

int main(int argc, char ** argv)
{
	if(argc < 2)
	{
		printf("no input file\n");
		return 0;
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
		int 		i = 0;
		char 	line[256];

		for(i = 0; fgets(line, sizeof(line), input); i++)
		{
			if(line[0] == '#')
			{
				continue;
			}
			else
			{
				sscanf(line,"%2s %d %d %d\n", 
					processes[i].pid, 
					&(processes[i].arrive_time), 
					&(processes[i].service_time), 
					&(processes[i].priority)
				);

				print_process(&processes[i]);
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

void print_process(Process* process)
{
	printf("pid : %s arrival : %d service : %d priority : %d\n", 
		process->pid, 
		process->arrive_time, 
		process->service_time, 
		process->priority
	);
}