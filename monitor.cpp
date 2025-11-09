#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <csignal>
#include <cstdio>
#include <cstdlib>

#ifdef __APPLE__
#define SMON_APPLE 1
#else
#define SMON_APPLE 0
#endif

#if SMON_APPLE
struct Proc { int pid; double cpu; long rss; std::string cmd; };

std::vector<Proc> get_processes() {
    std::vector<Proc> out;
    FILE* pipe = popen("ps -axo pid,pcpu,rss,command", "r");
    if(!pipe) return out;
    char buffer[4096];
    bool skip_header = true;

    while(fgets(buffer, sizeof(buffer), pipe)) {
        if(skip_header) { skip_header = false; continue; }
        std::stringstream ss(buffer);
        int pid; double cpu; long rss;
        if(!(ss >> pid >> cpu >> rss)) continue;
        std::string cmd;
        std::getline(ss, cmd);
        if(cmd.size() > 1 && cmd[0] == ' ') cmd.erase(0,1);
        out.push_back({pid, cpu, rss, cmd});
    }
    pclose(pipe);
    return out;
}

#else
#endif

int main() {
    while(true) {
        auto procs = get_processes();
        std::sort(procs.begin(), procs.end(), [](auto &a, auto &b){
            return a.cpu > b.cpu;
        });

        std::cout << "\033[2J\033[H"; 
        std::cout << "System Monitor Tool (macOS)\n";
        std::cout << "PID     %CPU     RSS(MB)    CMD\n";

        for(auto &p : procs) {
            std::cout << std::left << std::setw(8) << p.pid
                      << std::setw(8) << std::fixed << std::setprecision(1) << p.cpu
                      << std::setw(10) << (p.rss / 1024.0)
                      << p.cmd.substr(0,50) << "\n";
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}
