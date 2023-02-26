#include "config.h"

Config::Config(const string &config_path, int period_ms)
    : config_file_(config_path) {}

map<string, Process> Config::readJson()
{
    try
    {
        cout << "Reading json file from: " << config_file_ << endl;

        // open config json file
        std::ifstream infile(config_file_);
        if (!infile)
        {
            throw std::runtime_error("Could not open configuration file: " + config_file_);
        }

        // read as json object
        json json_data;
        infile >> json_data;
        infile.close();

        map<string, Process> new_processes;
        // parse and iterate json array for processes
        for (auto &process : json_data["processes"])
        {
            string name = process["name"];
            string command = process["command"];

            Process proc;
            proc.name = name;
            proc.command = command;
            for (auto &arg : process["args"])
            {
                proc.args.push_back(arg);
            }
            proc.start_time = 0;
            proc.pid = -1;

            new_processes[name] = proc;

            cout << process["name"] << endl;
        }
        cout << "Read configuration done!" << endl;
    }
    catch (const exception &exp)
    {
        cout << "Error when reading config file. Details: " << exp.what() << endl;
    }
}