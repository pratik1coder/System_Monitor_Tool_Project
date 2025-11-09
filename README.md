# System Monitor Tool

## Overview
The **System Monitor Tool** is a real-time system process monitoring application developed in **C++**.
It displays active system processes along with important resource usage statistics such as:

- **PID (Process ID)**
- **CPU Usage (%)**
- **Memory Usage (RSS in MB)**
- **Command / Executable Name**

This project was completed as part of the **Wipro Embedded COE Capstone Program**, demonstrating skills in:
- Linux/macOS system-level programming
- Shell command integration with C++
- Real-time data processing and display
- C++17 development workflow

---

## Features
| Feature | Description |
|--------|-------------|
| Process Listing | Shows all currently running system processes |
| CPU Usage Monitoring | Displays real-time CPU utilization of each process |
| Memory Usage Display | Shows memory usage (RSS in MB) |
| Automatic Refresh | Updates the display every 2 seconds |
| Sorted Output | Processes sorted by highest CPU usage |

---

## Technologies Used
| Component | Details |
|----------|---------|
| Programming Language | C++ (C++17) |
| Operating System | macOS |
| System Command Used | `ps -axo pid,pcpu,rss,command` |
| Compiler Used | `clang++` / `g++` |

---

## Project File Structure
```
System-Monitor-Tool/
│
├── monitor.cpp            # Source code
└── README.md              # Project documentation
```

---

## How to Compile
Open terminal inside the project folder and run:

```bash
clang++ monitor.cpp -std=c++17 -O2 -o smon
```

(If you are using Linux with g++, use `g++` instead of `clang++`.)

---

## How to Run
```bash
./smon
```

The tool will now display processes and auto-refresh every 2 seconds.

---

## Sample Output (Terminal Preview)
```
System Monitor Tool
PID     %CPU     RSS(MB)    CMD
931     14.9     285.5      /System/...
571     7.1      38.2       /usr/libexec...
...
```

---

## Author Details
| Field | Information |
|------|-------------|
| **Name** | Sai Pratik Rath |
| **Registration No.** | 2241018082 |
| **Institute** | Institute of Technical Education and Research |

---

## License
This project is created for academic and learning purposes under Wipro COE Training Program.
