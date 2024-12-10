/******************************************************* START *******************************************************/

#include <bits/stdc++.h>
#include "parser.h"

using namespace std;


/******************************************************* Global Constants *******************************************************/

const string TRACE = "trace";
const string SHOW_STATISTICS = "stats";
const string ALGORITHMS[5] = {"", "FCFS", "RR-", "SJF", "SRTF"};

/******************************************************* Clearing Timeline *******************************************************/

void clear_timeline()
{
    for(int i=0; i<last_instant; i++)
        for(int j=0; j<process_count; j++)
            timeline[i][j] = ' ';
}


/******************************************************* Getting Process Information *******************************************************/

string get_process_name(tuple<string, int, int> &a)
{
    return get<0>(a);
}

int get_arrival_time(tuple<string, int, int> &a)
{
    return get<1>(a);
}

int get_burst_time(tuple<string, int, int> &a)
{
    return get<2>(a);
}


/******************************************************* Filling Wait Time *******************************************************/

void fill_in_wait_time(){
    for (int i = 0; i < process_count; i++)
    {
        int arrival_time = get_arrival_time(processes[i]);
        for (int k = arrival_time; k < completion_time[i]; k++)
        {
            if (timeline[k][i] != '*')
                timeline[k][i] = '.';
        }
    }
}


/******************************************************* Scheduling Processes *******************************************************/

void first_come_first_serve()
{
    int time = get_arrival_time(processes[0]);
    for (int i = 0; i < process_count; i++)
    {
        int process_index = i;
        int arrival_time = get_arrival_time(processes[i]);
        int burst_time = get_burst_time(processes[i]);

        time = max(time,arrival_time);
        completion_time[process_index] = (time + burst_time);
        turn_around_time[process_index] = (completion_time[process_index] - arrival_time);
        waiting_time[process_index] = (turn_around_time[process_index] - burst_time);

        for (int j = time; j < completion_time[process_index]; j++)
            timeline[j][process_index] = '*';
        for (int j = arrival_time; j < time; j++)
            timeline[j][process_index] = '.';
        time = completion_time[process_index];
    }
}

void round_robin(int original_quantum)
{
    queue<pair<int,int>>q; // {process index, service time}
    int j=0;

    // Push the first process in the queue if it arrives at time 0
    if(get_arrival_time(processes[j])==0){
        q.push({j,get_burst_time(processes[j])});
        j++;
    }

    int current_quantum = original_quantum;

    // Main loop for each time unit
    for(int time =0;time<last_instant;time++)
    {

        if(!q.empty())
        {
            int process_index = q.front().first;
            q.front().second = q.front().second-1; // Decrement remaining service time
            int remaining_burst_time = q.front().second;
            int arrival_time = get_arrival_time(processes[process_index]);
            int burst_time = get_burst_time(processes[process_index]);
            current_quantum--; // Decrement quantum
            timeline[time][process_index]='*'; // Indicate process execution

            // Push processes arriving at the next time unit
            while(j<process_count && get_arrival_time(processes[j])==time+1)
            {
                q.push(make_pair(j,get_burst_time(processes[j])));
                j++;
            }

            // Check if quantum and service time are both exhausted
            if(current_quantum==0 && remaining_burst_time==0)
            {
                completion_time[process_index]=time+1;
                turn_around_time[process_index] = (completion_time[process_index] - arrival_time);
                waiting_time[process_index] = (turn_around_time[process_index] - burst_time);
                current_quantum=original_quantum;
                q.pop();
            }
            // Check if only quantum is exhausted
            else if(current_quantum==0 && remaining_burst_time!=0)
            {
                q.pop();
                q.push(make_pair(process_index,remaining_burst_time));
                current_quantum=original_quantum;
            }
            // Check if only service time is exhausted
            else if(current_quantum!=0 && remaining_burst_time==0)
            {
                completion_time[process_index]=time+1;
                turn_around_time[process_index] = (completion_time[process_index] - arrival_time);
                waiting_time[process_index] = (turn_around_time[process_index] - burst_time);
                q.pop();
                current_quantum=original_quantum;
            }
        }

        // Push processes arriving at the next time unit
        while(j<process_count && get_arrival_time(processes[j])==time+1)
        {
            q.push({j,get_burst_time(processes[j])});
            j++;
        }
    }

    // Fill in wait times after the scheduling is complete
    fill_in_wait_time();
}

void shortest_job_first() // Shortest job first
{

    // Priority queue to store pairs of (service time, index) with the smallest service time at the top
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; 

    int j = 0;

    for (int i = 0; i < last_instant; i++)
    {   
        // Push all processes that have arrived by time 'i' into the priority queue
        while(j<process_count && get_arrival_time(processes[j]) <= i)
        {
            pq.push(make_pair(get_burst_time(processes[j]), j));
            j++;
        }

         // If the priority queue is not empty, process the shortest job first
        if (!pq.empty())
        {   
            // Get the index of the process with the shortest service time
            int process_index = pq.top().second;
            int arrival_time = get_arrival_time(processes[process_index]);
            int burst_time = get_burst_time(processes[process_index]);
            pq.pop();

            // Mark the timeline for the waiting time before the process starts execution
            int temp = arrival_time;
            for (; temp < i; temp++)
                timeline[temp][process_index] = '.';

            // Mark the timeline for the execution time of the process
            temp = i;
            for (; temp < i + burst_time; temp++)
                timeline[temp][process_index] = '*';

            // Update the finish time, turnaround time, and normalized turnaround time for the process
            completion_time[process_index] = (i + burst_time);
            turn_around_time[process_index] = (completion_time[process_index] - arrival_time);
            waiting_time[process_index] = (turn_around_time[process_index] - burst_time);

            // Move the current time to the end of the execution of the current process
            i = temp - 1;
        }
    }
}

void shortest_remaining_time_first()
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; // min heap
    int j = 0;
    for (int i = 0; i < last_instant; i++)
    {
        while(j<process_count &&get_arrival_time(processes[j]) == i)
        {
            pq.push(make_pair(get_burst_time(processes[j]), j));
            j++;
        }
        if (!pq.empty())
        {
            int process_index = pq.top().second;
            int remaining_time = pq.top().first;
            pq.pop();
            int burst_time = get_burst_time(processes[process_index]);
            int arrival_time = get_arrival_time(processes[process_index]);
            timeline[i][process_index] = '*';

            if (remaining_time == 1) // process finished
            {
                completion_time[process_index] = i + 1;
                turn_around_time[process_index] = (completion_time[process_index] - arrival_time);
                waiting_time[process_index] = (turn_around_time[process_index] - burst_time);
            }
            else
            {
                pq.push(make_pair(remaining_time - 1, process_index));
            }
        }
    }
    fill_in_wait_time();
}


/******************************************************* Printing Column Name *******************************************************/

void print_algorithm(int algorithm_index)
{
    int algorithm_id = algorithms[algorithm_index].first - '0';
    if(algorithm_id==2)
        cout << ALGORITHMS[algorithm_id] <<algorithms[algorithm_index].second <<endl;
    else
        cout << ALGORITHMS[algorithm_id] << endl;
}


/******************************************************* Printing Row Name *******************************************************/

void print_processes()
{
    cout << "Process    ";
    for (int i = 0; i < process_count; i++)
        {
            cout << "|  " << get_process_name(processes[i]) << "  ";
        }
    cout<<"| Mean| \n";
}

void print_arrival_time()
{
    cout << "Arrival    |";
    for (int i = 0; i < process_count; i++)
    {
        cout << " " << setw(2) << get_arrival_time(processes[i]) <<"  |";
    }
    cout << "-----|\n";
}

void print_burst_time()
{
    cout << "Burst      |";
    for (int i = 0; i < process_count; i++)
    {
        cout << setw(3) << get_burst_time(processes[i]) << "  |";
    }
        cout << "-----|\n";

}

void print_completion_time()
{
    cout << "Completion |";
    for (int i = 0; i < process_count; i++)
        {
            cout << setw(3) << completion_time[i] << "  |" ;
        }

    cout << "-----|\n";
}

void print_turn_around_time()
{
    cout << "Turnaround |";
    int sum = 0;
    for (int i = 0; i < process_count; i++)
    {
        cout << setw(3) << turn_around_time[i] << "  |";
        sum += turn_around_time[i];
    }

    float ans = 1.0 * sum / turn_around_time.size();

    if(ans>=10)
        cout << fixed << setprecision(2) << ans << "|\n";
    else
        cout << fixed << setprecision(2) << ans << " |\n";

	 	
}

void print_waiting_time()
{
    cout << "Waiting    |";
    float sum = 0;
    for (int i = 0; i < process_count; i++)
    {
        cout << setw(3) << waiting_time[i] << "  |";
        sum += waiting_time[i];
    }
    
    float ans = 1.0*sum/waiting_time.size();

    if( ans >=10 )
        cout << fixed << setprecision(2) << ans << "|\n";
	else
        cout << fixed << setprecision(2) << ans << " |\n";
 
}


/******************************************************* Printing Stats *******************************************************/

void print_stats(int algorithm_index)
{
    print_algorithm(algorithm_index);
    print_processes();
    print_arrival_time();
    print_burst_time();
    print_completion_time();
    print_turn_around_time();
    print_waiting_time();
}


/******************************************************* Printing Timeline *******************************************************/

void print_timeline(int algorithm_index)
{
    for (int i = 0; i <= last_instant; i++)
       { cout << i % 10<<" ";}

    cout <<"\n";

    cout<<"------";
    for (int i = 0; i <= 2*last_instant; i++)
        cout << "-";
        
    cout <<"\n";

    for (int i = 0; i < process_count; i++)
    {
        cout << get_process_name(processes[i]) << "     |";
        for (int j = 0; j < last_instant; j++)
        {
            cout << timeline[j][i]<<"|";
        }
        cout << " \n";
    }

    cout<<"------";
    for (int i = 0; i <= 2*last_instant; i++)
       { cout << "-";}
    
}


/******************************************************* Searching for Scheduling Process *******************************************************/

void execute_algorithm(char algorithm_id, int quantum,string operation)
{
    switch (algorithm_id)
    {
    case '1':
        if(operation==TRACE) cout<<"FCFS  ";
        first_come_first_serve();
        break;
    case '2':
        if(operation==TRACE) cout<<"RR-"<<quantum<<"  ";
        round_robin(quantum);
        break;
    case '3':
        if(operation==TRACE) cout<<"SJF   ";
        shortest_job_first();
        break;
    case '4':
        if(operation==TRACE) cout<<"SRTF  ";
        shortest_remaining_time_first();
        break;
    default:
        break;
    }
}


/******************************************************* Driver Code *******************************************************/

int main()
{
    parse();

    for (int idx = 0; idx < (int)algorithms.size(); idx++)
    {
        clear_timeline();

        cout << "\n";

        execute_algorithm(algorithms[idx].first, algorithms[idx].second,operation);

        if (operation == TRACE)
            print_timeline(idx);
        else if (operation == SHOW_STATISTICS)
            print_stats(idx);

        cout << "\n";
    }

    return 0;
}


/******************************************************* END *******************************************************/
