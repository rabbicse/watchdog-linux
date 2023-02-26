#pragma once

#include <string>
#include <vector>
#include <iostream>

using namespace std;

struct Process
{
    string name;
    string command;
    vector<string> args;
    time_t start_time;
    pid_t pid;
};