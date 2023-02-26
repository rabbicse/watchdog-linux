#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <mutex>
#include <condition_variable>
#include "native_api.h"

class Watchdog {
public:
    Watchdog(const std::string& config_file);
    ~Watchdog();

    Watchdog(const Watchdog&) = delete;
    Watchdog& operator=(const Watchdog&) = delete;

private:
    struct Process {
        std::string name;
        std::vector<std::string> args;
        NativeAPI::ProcessHandle handle;
    };

    void load_config();
    void watch_processes();
    void restart_process(const std::string& name, Process& process);
    void run_process(const std::string& name, Process& process);
    void stop_process(Process& process);
    bool is_process_running(const Process& process) const;

    std::string config_file_;
    std::unordered_map<std::string, Process> processes_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::unique_ptr<NativeAPI> native_api_;
    bool stop_watchdog_ = false;
};

#endif // WATCHDOG_H
