# System Monitor Tool
Build:
```
g++ -std=gnu++17 -O2 -o smon monitor.cpp
```
Run:
```
./smon --sort cpu --interval 2
```
Keys: `q` quit, `s cpu|mem` change sort, `k <pid>` kill process.
