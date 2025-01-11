#ifndef _PROC_DATA
#define _PROC_DATA


typedef struct ProcData {
    char name[4096];
    float percent_cpu;
    float percent_mem;
    long pid;
    long cpu_time; // microseconds
    long sys_time; // microseconds
    long memory_size;
    int priority;
    int nice;
    char state[32];
} ProcData;

int get_proc_data(struct ProcData **proc_data);

#endif