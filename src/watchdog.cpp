#include "watchdog.h"

Watchdog::Watchdog(const string &config_path, int period_ms)
    : config_file_(config_path), period_ms_(period_ms), running_(false) {}

void Watchdog::start()
{
    running_ = true;

    // Start the configuration monitor thread
    configThread_ = thread(&Watchdog::configMonitor, this);

    // Wait for the thread to finish
    configThread_.join();
}

void Watchdog::stop()
{
    running_ = false;
}

void Watchdog::reload_config()
{
    try
    {
        cout << "reloading config..." << endl;

        // read config file
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

        // Stop processes that are no longer in the configuration
        for (auto &it : processes_)
        {
            if (new_processes.find(it.first) == new_processes.end())
            {
                cerr << "Stopping process " << it.second.name << " with PID " << it.second.pid << endl;
                kill(it.second.pid, SIGTERM);
            }
        }

        processes_ = new_processes;

        // Check for stopped or killed processes
        for (auto &[k, v] : processes_)
        {
            if (!is_process_running(v.pid))
            {
                std::cout << "Process " << v.name << " stopped. Restarting..." << std::endl;
                start_process(v);
            }
        }

        // Get the last modified time of the configuration file
        struct stat fileStats;
        stat(config_file_.c_str(), &fileStats);
        lastModified_ = fileStats.st_mtime;
    }
    catch (const exception &exp)
    {
        cout << "Error when reading config file. Details: " << exp.what() << endl;
    }
}

void Watchdog::check_processes()
{
    while (running_)
    {
        for (auto &it : processes_)
        {
            pid_t pid = it.second.pid;

            if (!is_process_running(pid))
            {
                cerr << "Process " << it.second.name << " with PID " << pid << " is not running, restarting..." << endl;
                restart_process(it.second);
            }
        }

        this_thread::sleep_for(chrono::milliseconds(period_ms_));
    }
}

void Watchdog::restart_process(Process &process)
{
    cout << "Stopping process " << process.name << " with PID " << process.pid << endl;
    kill(process.pid, SIGTERM);

    start_process(process);
}

bool Watchdog::is_process_running(pid_t pid)
{
    if (pid == -1)
    {
        return false;
    }

    if (kill(pid, 0) == 0)
    {
        return true;
    }

    return false;
}

pid_t Watchdog::start_process(const Process &process)
{
    try
    {
        pid_t pid = fork();

        if (pid < 0)
        {
            cerr << "Failed to start process " << process.name << ", fork() error: " << strerror(errno) << endl;
            return -1;
        }
        else if (pid == 0)
        {
            vector<string> args = {process.command};
            copy(process.args.begin(), process.args.end(), back_inserter(args));

            char *argv[args.size() + 1];
            for (size_t i = 0; i < args.size(); ++i)
            {
                argv[i] = const_cast<char *>(args[i].c_str());
            }
            argv[args.size()] = nullptr;

            execv(process.command.c_str(), argv);

            cerr << "Failed to start process " << process.name << ", execv() error: " << strerror(errno) << endl;
            exit(1);
        }
        else
        {
            return pid;
        }
    }
    catch (exception &exp)
    {
        cout << "Error start process: Details: " << exp.what() << endl;
    }

    return -1;
}

// Thread function to monitor the configuration file for changes
void Watchdog::configMonitor()
{
    struct stat fileStats;
    while (true)
    {
        // Sleep for 1 second to reduce CPU usage
        this_thread::sleep_for(chrono::seconds(1));

        // Get the last modified time of the configuration file
        stat(config_file_.c_str(), &fileStats);
        time_t lastModified = fileStats.st_mtime;

        // Check if the configuration file has been modified
        if (lastModified != lastModified_)
        {
            // If the file has been modified, read the new configuration
            reload_config();
        }
    }
}
