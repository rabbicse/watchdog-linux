#pragma once

#include "process.h"
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

class Config {
public:
    /// @brief Generic Config class to read json, text or yaml config based on user preferences
    /// @param config_path 
    /// @param period_ms 
    Config(const string& config_path, int period_ms);

    map<string, Process> readJson();
    

private:
    // config and reload related
    string config_file_;        
    time_t lastModified_;
    thread configThread_;
};