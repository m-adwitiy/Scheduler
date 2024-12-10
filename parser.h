#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include <bits/stdc++.h>

using namespace std;

/** This file handles parsing the data we are going to work with **/
/** It also holds all the global variables we parse into         **/

// Global Variables

string operation;
int last_instant, process_count;
vector<pair<char, int>> algorithms; // < algorithm_id, quantum >
vector<tuple<string,int,int>> processes; // < process_name, arrival_time, service_time >
vector<vector<char>>timeline;


//Global Results

vector<int>completion_time;
vector<int>turn_around_time;
vector<int>waiting_time;


// Parsing Functions

void parse_algorithms(string algorithm_chunk)
{
    stringstream stream(algorithm_chunk);
    while (stream.good())
    {
        string temp_str;
        getline(stream, temp_str, ',');
        stringstream ss(temp_str);
        getline(ss, temp_str, '-');
        char algorithm_id = temp_str[0];
        getline(ss, temp_str, '-');
        int quantum = temp_str.size() >= 1 ? stoi(temp_str) : -1;
        algorithms.push_back( make_pair(algorithm_id, quantum) );
    }
}

void parse_processes()
{
    string process_chunk, process_name;
    int process_arrival_time, process_burst_time;
    for(int i=0; i<process_count; i++)
    {   
        // cout<< "Enter details of process " << i+1 <<endl;  
        cin >> process_chunk;

        stringstream stream(process_chunk);
        string temp_str;
        getline(stream, temp_str, ',');
        process_name = temp_str;
        getline(stream, temp_str, ',');
        process_arrival_time = stoi(temp_str);
        getline(stream, temp_str, ',');
        process_burst_time = stoi(temp_str);

        processes.push_back( make_tuple(process_name, process_arrival_time, process_burst_time) );
       // processToIndex[process_name] = i;
    }
}

void parse()
{
    string algorithm_chunk;
    /*
    cout << "Enter your operation(trace/stats)"<<endl;
    cin >> operation ;
    cout<< "Enter algorithm-quantum"<<endl;
    cin >> algorithm_chunk;

    cout<<"Enter last instant of your timeline"<<endl;
    cin >> last_instant;
    cout <<"Enter number of processes" << endl; 
    cin >> process_count;
    */
   cin >> operation >> algorithm_chunk >> last_instant >> process_count;

    parse_algorithms(algorithm_chunk);
    parse_processes();
    completion_time.resize(process_count);
    turn_around_time.resize(process_count);
    waiting_time.resize(process_count);
    timeline.resize(last_instant);
    for(int i=0; i<last_instant; i++)
        for(int j=0; j<process_count; j++)
            timeline[i].push_back(' ');
}


#endif // PARSER_H_INCLUDE
