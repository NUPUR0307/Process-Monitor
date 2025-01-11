// #ifndef DISPLAY_H
// #define DISPLAY_H

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include "proc_data.h"   
// #include "proc_metrics.h" 

// // Function to clear the terminal screen
// void clear_screen();

// // Function to display the top processes by %CPU
// void display_top_processes(ProcData *proc_data, int len);

// // Function to calculate the summary of processes
// void calculate_summary(ProcData *proc_data, int len, float *total_cpu, float *total_memory);

// // Function to display the summary of processes
// void display_summary(float total_cpu, float total_memory, int num_processes);

// // Function to periodically refresh the process data display
// void refresh_display(ProcData *proc_data, int len, CPUDelta *deltas, int interval);

// #endif 


#ifndef DISPLAY_H
#define DISPLAY_H

#include "proc_data.h"
#include "proc_metrics.h"

void clear_screen(void);
int compare_by_cpu(const void *a, const void *b);
void display_top_processes(ProcData *proc_data, int len, int num_procs_display);
void calculate_summary(ProcData *proc_data, int len, float *total_cpu, float *total_memory);
void display_summary(float total_cpu, float total_memory, int num_processes);
void refresh_display(ProcData *proc_data, int len, CPUDelta *deltas, int interval, int num_procs_display);
void cleanup_display(void);

#endif