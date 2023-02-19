#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <map>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/stat.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

struct Process
{
    string name;
    string command;
    vector<string> args;
    time_t start_time;
    pid_t pid;
};

class Watchdog {
public:
    Watchdog(const string& config_path, int period_ms);
    void start();
    void stop();

private:
    // config and reload related
    string config_file_;    
    map<string, Process> processes_;
    time_t lastModified_;
    thread configThread_;

    int period_ms_;
    bool running_;

    void reload_config();
    void check_processes();
    bool is_process_running(pid_t pid);
    pid_t start_process(const Process& process);
    void restart_process(Process& process);
    void configMonitor();
};
