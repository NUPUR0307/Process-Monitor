#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "proc_data.h"
#include "proc_metrics.h"
#include "display.h"

void clear_screen() {
    printf("\033[2J\033[H");
}

int compare_by_cpu(const void *a, const void *b) {
    ProcData *proc_a = (ProcData *)a;
    ProcData *proc_b = (ProcData *)b;
    return (proc_b->percent_cpu > proc_a->percent_cpu) - (proc_a->percent_cpu > proc_b->percent_cpu);
}

void display_top_processes(ProcData *proc_data, int len, int num_procs_display) {
    int display_count = len < num_procs_display ? len : num_procs_display;
    for (int i = 0; i < display_count; i++) {
        printf("%-20s %-10ld %-13s %-10.2f %-10.2f %-12ld %-10d %-10d\n",
               proc_data[i].name, 
               proc_data[i].pid, 
               proc_data[i].state,
               proc_data[i].percent_cpu, 
               proc_data[i].percent_mem, 
               proc_data[i].memory_size,
               proc_data[i].priority,
               proc_data[i].nice);
    }
}

void calculate_summary(ProcData *proc_data, int len, float *total_cpu, float *total_memory) {
    *total_cpu = 0.0f;
    *total_memory = 0.0f;

    for (int i = 0; i < len; i++) {
        *total_cpu += proc_data[i].percent_cpu;
        *total_memory += proc_data[i].memory_size;
    }
}

void display_summary(float total_cpu, float total_memory, int num_processes) {
    printf("\nSummary:\n");
    printf("Total Processes: %d\n", num_processes);
    printf("Total Memory Usage: %.2f MB\n", total_memory / 1024.0f);
    printf("------------------------------------------------------------------------------------------------------\n");
}

void truncate_names(ProcData *proc_data, int len, int num_procs_display) {
    for (int i = 0; i < num_procs_display; i++) {
        if (strnlen(proc_data[i].name, 4096) >= 20) {
            proc_data[i].name[17] = '.';
            proc_data[i].name[18] = '.';
            proc_data[i].name[19] = '.';
            proc_data[i].name[20] = '\0';
        }
    }
}

void refresh_display(ProcData *proc_data, int len, CPUDelta *deltas, int interval, int num_procs_display) {
    printf("\033[?1049h");
    printf("\033[?25l");
    
    while (1) {
        len = get_proc_data(&proc_data);
        if (len <= 0) {
            fprintf(stderr, "Error refreshing process data\n");
            break;
        }

        update_process_metrics(proc_data, len, deltas);
        qsort(proc_data, len, sizeof(ProcData), compare_by_cpu);

        truncate_names(proc_data, len, num_procs_display);
        
        printf("\033[H\033[2J");
        
        printf("%-20s %-10s %-13s %-10s %-10s %-12s %-10s %-10s\n",
               "Name", "PID", "State", "%CPU", "%MEM", "Memory (KB)", "Priority", "Nice");
        printf("------------------------------------------------------------------------------------------------------\n");

        display_top_processes(proc_data, len, num_procs_display);

        float total_cpu = 0.0f;
        float total_memory = 0.0f;
        calculate_summary(proc_data, len, &total_cpu, &total_memory);
        display_summary(total_cpu, total_memory, len);

        fflush(stdout);
        sleep(interval);
    }
}

void cleanup_display() {
    printf("\033[?1049l");
    printf("\033[?25h");
}
