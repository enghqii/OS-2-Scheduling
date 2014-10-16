#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>

// Decl

#define MAX_ROW 80
#define MAX_GRAPH_DATA (MAX_ROW)

// Process BEGIN

enum PROCESS_STATE
{
    STATE_NONE = 0,
    STATE_READY,
    STATE_RUNNING,
    STATE_EXIT,
};

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
        //printf("[%s] is now READY\n", this->pid);
    }

    // running
    if(this->state == STATE_RUNNING)
    {
        this->running_time += dt;
        //printf("Cur Process is [%s], runnin' time: [%d]\n", this->pid, this->running_time);
    }

    // exit
    if(this->state != STATE_EXIT && (this->service_time == this->running_time))
    {
        this->complete_time = cpu_time;
        this->state = STATE_EXIT;
        //printf("[%s] is now EXIT on time of [%d]\n", this->pid, this->complete_time);
    }
}

// Process END

// Global vars
FILE *  input;

Process processes[260];
int num_process = 0;

// Prototypes
int init_input(char * filename);
int check_process(Process* process, int line);
void print_processes();
void reset_all_processes();

void SJF_simulation();
void SRT_simulation();
void RR_simulation();
void PR_simulation();

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
            reset_all_processes();
            SJF_simulation();

            reset_all_processes();
            SRT_simulation();

            reset_all_processes();
            RR_simulation();

            reset_all_processes();
            PR_simulation();
        }
    }

    return 0;
}

int init_input(char * filename)
{
    if((input = fopen(filename, "r")))
    {
        int     line_number = 0;
        char    line[256];
        num_process = 0;

        while(fgets(line, sizeof(line), input))
        {
            line_number++;

            if(line[0] == '#' || line[0] == '\n' || line[0] == '\0')
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
                    fprintf(stderr, "invalid format in line %d\n", line_number);
                    continue;
                }

                if(check_process(&processes[num_process], line_number) == false)
                {
                    continue;
                }
            }

            num_process++;
        }

        //print_processes();
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

bool is_capital_alpha_or_digit(char c)
{
    return (('A' <= c && c <= 'Z') || ('0' <= c && c <= '9')? true : false);
}

int check_process(Process* process, int line)
{
    int i = 0;

    if(strlen(process->pid) != 2 || !(is_capital_alpha_or_digit(process->pid[0]) && is_capital_alpha_or_digit(process->pid[1])))
    {
        printf("invalid process id \'%s\' in line %d, ignored", process->pid, line);
    }

    for(i = 0; i < num_process; i++)
    {
        if(strcmp(processes[i].pid, process->pid) == 0)
        {
            fprintf(stderr, "duplicate process id \'%s\' in line %d, ignored\n", process->pid, line);
            return false;
        }
    }

    if( 0 > process->arrive_time || process->arrive_time > 30 )
    {
        fprintf(stderr, "invalid arrive time \'%d\' in line %d, ignored\n", process->arrive_time, line);
        return false;
    }

    if( 1 > process->service_time || process->service_time > 30 )
    {
        fprintf(stderr, "invalid service time \'%d\' in line %d, ignored\n", process->service_time, line);
        return false;
    }

    if( 1 > process->priority || process->priority > 10 )
    {
        fprintf(stderr, "invalid priority \'%d\' in line %d, ignored\n", process->priority, line);
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

// Simulation related

bool all_process_done();
void init_graph_data(int* graph_data);
void simulation_output(char* title, int cpu_time, int graph_data[MAX_GRAPH_DATA]);

void SJF_simulation()
{
    const int   deltaTime = 1;
    int         cpu_time = 0;
    int         wall_time = 0;

    Process *   cur_process = NULL;

    int         graph_data[MAX_GRAPH_DATA];
    init_graph_data(graph_data);

    while(!all_process_done())
    {
        //printf(" CPU TIME [%d] \n", cpu_time);

        // update each process
        int i = 0;
        for(i = 0; i < num_process; i++)
        {
            update_process(&processes[i], deltaTime, wall_time);
        }

        if(all_process_done())
        {
            break;
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
                //printf("Changing process index of [%d]\n", target_index);
                cur_process = &processes[target_index];
                cur_process->state = STATE_RUNNING;
            }
        }

        if(cur_process)
        {
            graph_data[wall_time] = (int) (cur_process - processes);
            cpu_time += deltaTime;
        }
        wall_time += deltaTime;
        //printf("=========================\n");
    }

    //printf("\n");
    simulation_output("SJF" ,cpu_time, graph_data);
}

void SRT_simulation()
{
    const int   deltaTime = 1;
    int         cpu_time = 0;
    int         wall_time = 0;

    Process *   cur_process = NULL;

    int         graph_data[MAX_GRAPH_DATA];
    init_graph_data(graph_data);

    //printf("\nSRT simulating\n\n");

    while(!all_process_done())
    {
        //printf(" CPU TIME [%d] \n", cpu_time);
        // update each process
        int i = 0;
        for(i = 0; i < num_process; i++)
        {
            update_process(&processes[i], deltaTime, wall_time);
        }

        if(all_process_done())
        {
            break;
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
                    //printf("~Remaining time of [%s] is [%d]\n",processes[j].pid, remaining_time);

                    if( remaining_time < shortest_remaining_time )
                    {
                        target_index = j;
                        shortest_remaining_time = remaining_time;
                    }
                }
            }

            if(target_index != -1)
            {
                //printf("Changing process index of [%d]\n", target_index);

                if(cur_process && (STATE_EXIT != cur_process->state))
                    cur_process->state = STATE_READY;
                cur_process = &processes[target_index];
                cur_process->state = STATE_RUNNING;
            }
        }

        if(cur_process)
        {
            graph_data[wall_time] = (int) (cur_process - processes);
            cpu_time += deltaTime;
        }
        wall_time += deltaTime;
        //printf("=========================\n");
    }

    //printf("\n");
    simulation_output("SRT", cpu_time, graph_data);
}

void RR_simulation()
{
    const int   time_quantum = 1;
    const int   deltaTime = 1;
    int         cpu_time = 0;
    int         wall_time = 0;

    int         cur_process_index = 0;
    Process *   cur_process = NULL;

    int         graph_data[MAX_GRAPH_DATA];
    init_graph_data(graph_data);

    //printf("\nRR simulating\n\n");

    while(true)
    {
        //printf(" CPU TIME [%d] \n", cpu_time);

        // update each process
        int i = 0;
        for (i = 0; i < num_process; i++)
        {
            update_process(&processes[i], deltaTime, wall_time);
        }

        if(all_process_done())
        {
            break;
        }

        // it's time to rotation
        if(cpu_time % time_quantum == 0)
        {
            int cnt = 0;
            do
            {
                cur_process_index++;
                cur_process_index %= num_process;

                cnt++;

            }while(processes[cur_process_index].state != STATE_READY && cnt < num_process);


            if(cur_process && (STATE_EXIT != cur_process->state))
                cur_process->state = STATE_READY;
            cur_process = &processes[cur_process_index];
            cur_process->state = STATE_RUNNING;
        }

        if(cur_process)
        {
            graph_data[wall_time] = (int) (cur_process - processes);
            cpu_time += deltaTime;
        }
        wall_time += deltaTime;
        //printf("=========================\n");
    }

    simulation_output("RR", cpu_time, graph_data);
}

void PR_simulation()
{
    const int   deltaTime = 1;
    int         cpu_time = 0;
    int         wall_time = 0;

    Process *   cur_process = NULL;

    int         graph_data[MAX_GRAPH_DATA];
    init_graph_data(graph_data);

    //printf("\nPR simulating\n\n");

    while(!all_process_done())
    {
        //printf(" CPU TIME [%d] \n", cpu_time);
        // update each process
        int i = 0;
        for (i = 0; i < num_process; i++)
        {
            update_process(&processes[i], deltaTime, wall_time);
        }

        if(all_process_done())
        {
            break;
        }

        // pick next
        {
            int target_index = -1;
            int min_prior = INT_MAX;

            int j = 0;
            for (j = 0; j < num_process; j++)
            {
                if(processes[j].state == STATE_READY || processes[j].state == STATE_RUNNING)
                {
                    if(min_prior > processes[j].priority)
                    {
                        target_index = j;
                        min_prior = processes[j].priority;
                    }
                }
            }

            if(target_index != -1 && (cur_process != &processes[target_index]))
            {
                if(cur_process && (STATE_EXIT != cur_process->state))
                    cur_process->state = STATE_READY;
                cur_process = &processes[target_index];
                cur_process->state = STATE_RUNNING;
            }
        }

        if(cur_process)
        {
            graph_data[wall_time] = (int) (cur_process - processes);
            cpu_time += deltaTime;
        }
        wall_time += deltaTime;
        //printf("=========================\n");
    }

    simulation_output("PR", cpu_time, graph_data);
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

void init_graph_data(int* graph_data)
{
    int i = 0;
    for (i = 0; i < MAX_GRAPH_DATA; i++)
    {
        graph_data[i] = -1;
    }
}

void simulation_output(char* title, int cpu_time, int graph_data[MAX_GRAPH_DATA])
{
    // need to compute some vars

    float avg_turnaround_time = 0;
    float avg_waiting_time = 0;

    int i = 0;
    for(i = 0; i < num_process; i++)
    {
        //printf("[%s]'s complete time is [%d]\n", processes[i].pid, processes[i].complete_time);

        avg_turnaround_time += (processes[i].complete_time - processes[i].arrive_time);
        avg_waiting_time += (processes[i].complete_time - processes[i].arrive_time) - processes[i].service_time;
    }

    //printf("\n");

    avg_turnaround_time /= num_process;
    avg_waiting_time /= num_process;

    /// print out begin
    printf("[%s]\n", title);

    for(i = 0; i < num_process; i++)
    {
        int j = 0;

        printf("%2s ", processes[i].pid);

        for(j = 0; j < MAX_GRAPH_DATA; j++)
        {
            if(graph_data[j] == i)
                printf("*");
            else
                printf(" ");
        }

        printf("\n");
    }

    printf("CPU TIME: %d\n", cpu_time);
    printf("AVERAGE TURNAROUND TIME: %.2f\n", avg_turnaround_time);
    printf("AVERAGE WAITING TIME: %.2f\n", avg_waiting_time);

    printf("\n");
}