// System Monitor Tool - C++17 (Linux)
// Build: g++ -std=gnu++17 -O2 -o smon monitor.cpp
// Run:   ./smon [--sort cpu|mem] [--interval 2]
#include <bits/stdc++.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
using namespace std;

struct Proc{ int pid; string cmd; double cpu; long rss_kb; };
long g_tot_mem_kb = 0;

long read_total_mem_kb(){
    ifstream f("/proc/meminfo"); string k; long v; string unit;
    while (f>>k>>v>>unit){ if (k=="MemTotal:") return v; }
    return 0;
}
long read_clk_ticks(){ return sysconf(_SC_CLK_TCK); }

unordered_map<int, long long> read_proc_cpu_times(){
    unordered_map<int,long long> m;
    DIR* d = opendir("/proc");
    if (!d) return m;
    dirent* ent;
    while ((ent = readdir(d))){
        if (!isdigit(ent->d_name[0])) continue;
        int pid = atoi(ent->d_name);
        string path = string("/proc/")+ent->d_name+"/stat";
        ifstream f(path);
        if (!f) continue;
        string comm, state; long long utime,stime,cutime,cstime,starttime;
        long long dummy; // read numerous fields
        string rest;
        // Parse carefully: comm can have spaces in parentheses
        string s; getline(f, s);
        // extract parts
        size_t lp=s.find('('), rp=s.rfind(')');
        if (lp==string::npos||rp==string::npos) continue;
        string before = s.substr(0, lp);
        string name = s.substr(lp+1, rp-lp-1);
        string after = s.substr(rp+2);
        stringstream ss(after);
        ss >> state;
        // skip to 14th,15th fields (utime, stime)
        vector<long long> fields;
        long long val;
        while (ss>>val) fields.push_back(val);
        if (fields.size() < 42) continue;
        utime = fields[11]; stime = fields[12];
        long long total = utime + stime;
        m[pid] = total;
    }
    closedir(d);
    return m;
}

vector<Proc> snapshot(double interval_sec, const unordered_map<int,long long>& prev, long long clk){
    vector<Proc> out;
    DIR* d = opendir("/proc");
    if (!d) return out;
    dirent* ent;
    auto now_times = read_proc_cpu_times();
    while ((ent = readdir(d))){
        if (!isdigit(ent->d_name[0])) continue;
        int pid = atoi(ent->d_name);
        string statp = string("/proc/")+ent->d_name+"/stat";
        string cmdp  = string("/proc/")+ent->d_name+"/cmdline";
        string statusp = string("/proc/")+ent->d_name+"/status";
        ifstream fs(statp); if (!fs) continue;
        string s; getline(fs, s);
        size_t lp=s.find('('), rp=s.rfind(')');
        if (lp==string::npos||rp==string::npos) continue;
        string name = s.substr(lp+1, rp-lp-1);
        string cmd;
        {
            ifstream fc(cmdp);
            getline(fc, cmd, '\0');
            if (cmd.empty()) cmd = name;
        }
        long rss_kb = 0;
        {
            ifstream fst(statusp);
            string k; long v; string unit;
            while (fst>>k){
                if (k=="VmRSS:"){ fst>>v>>unit; rss_kb=v; break; }
                string rest; getline(fst, rest);
            }
        }
        double cpu=0.0;
        auto itprev = prev.find(pid);
        auto itnow = now_times.find(pid);
        if (itprev!=prev.end() && itnow!=now_times.end()){
            long long delta = itnow->second - itprev->second;
            cpu = (double)delta / clk / interval_sec * 100.0;
        }
        out.push_back({pid, cmd, cpu, rss_kb});
    }
    closedir(d);
    return out;
}

int main(int argc, char** argv){
    string sortby="cpu"; double interval=2.0;
    for (int i=1;i<argc;i++){
        string a=argv[i];
        if (a=="--sort" && i+1<argc) sortby=argv[++i];
        else if (a=="--interval" && i+1<argc) interval=stod(argv[++i]);
    }
    g_tot_mem_kb = read_total_mem_kb();
    long long clk = read_clk_ticks();
    unordered_map<int,long long> prev = read_proc_cpu_times();

    while (true){
        auto procs = snapshot(interval, prev, clk);
        prev = read_proc_cpu_times();
        // sort
        if (sortby=="mem") sort(procs.begin(), procs.end(), [](auto&a, auto&b){ return a.rss_kb>b.rss_kb; });
        else sort(procs.begin(), procs.end(), [](auto&a, auto&b){ return a.cpu>b.cpu; });
        // clear
        cout << "\033[2J\033[H";
        cout << "smon — interval " << interval << "s — sort by " << sortby << " — q=quit, s cpu|mem, k <pid>\n";
        cout << left << setw(8) << "PID" << setw(8) << "%CPU" << setw(10) << "RSS(MB)" << "CMD\n";
        int shown=0;
        for (auto &p: procs){
            if (shown++>40) break;
            cout << left << setw(8) << p.pid
                 << setw(8) << fixed << setprecision(1) << p.cpu
                 << setw(10) << fixed << setprecision(1) << (p.rss_kb/1024.0)
                 << p.cmd.substr(0,80) << "\n";
        }
        cout.flush();
        // simple input polling
        fd_set set; FD_ZERO(&set); FD_SET(STDIN_FILENO, &set);
        timeval tv; tv.tv_sec = (int)interval; tv.tv_usec= (interval - tv.tv_sec)*1e6;
        int r = select(STDIN_FILENO+1, &set, nullptr, nullptr, &tv);
        if (r>0){
            string cmd; cin >> cmd;
            if (cmd=="q") break;
            if (cmd=="s"){ string which; cin>>which; if (which=="cpu"||which=="mem") sortby=which; }
            if (cmd=="k"){ int pid; cin>>pid; if (kill(pid, SIGTERM)!=0) perror("kill"); }
        }
    }
    return 0;
}
