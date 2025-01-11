#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "proc_metrics.h"

/**
 * @brief Initializes an array of CPU delta tracking structures
 *
 * @param len Number of processes to track
 * @return CPUDelta* Pointer to array of initialized CPUDelta structures, NULL if error
 */
CPUDelta* init_cpu_deltas(int len) {
    if (len <= 0) return NULL;

    CPUDelta *deltas = malloc(len * sizeof(CPUDelta));
    if (!deltas) return NULL;
    
    memset(deltas, 0, len * sizeof(CPUDelta));
    
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    for (int i = 0; i < len; i++) {
        deltas[i].prev_time = current_time;
    }
    
    return deltas;
}

/**
 * @brief Calculates CPU usage percentage for a process
 *
 * Computes CPU usage by comparing current and previous CPU times,
 * normalized for system clock ticks and number of CPU cores.
 *
 * @param proc Pointer to process data structure
 * @param delta Pointer to CPU delta tracking structure
 * @return float CPU usage percentage (0-100), 0.0f if error
 */
float calculate_cpu_percentage(const ProcData *proc, CPUDelta *delta) {
    if (!proc || !delta) return 0.0f;

    struct timeval current_time;
    if (gettimeofday(&current_time, NULL) != 0) return 0.0f;

    // Convert clock ticks to microseconds
    long clk_tck = sysconf(_SC_CLK_TCK);
    if (clk_tck <= 0) return 0.0f;

    // Convert current CPU times from clock ticks to microseconds
    long curr_cpu_us = (proc->cpu_time * 1000000) / clk_tck;
    long curr_sys_us = (proc->sys_time * 1000000) / clk_tck;

    // Calculate elapsed time in microseconds
    long elapsed_us = (current_time.tv_sec - delta->prev_time.tv_sec) * 1000000 +
                     (current_time.tv_usec - delta->prev_time.tv_usec);
    
    if (elapsed_us <= 0) return 0.0f;

    // Calculate CPU time differences
    long cpu_diff = curr_cpu_us - delta->prev_cpu_time;
    long sys_diff = curr_sys_us - delta->prev_sys_time;

    cpu_diff = (cpu_diff < 0) ? 0 : cpu_diff;
    sys_diff = (sys_diff < 0) ? 0 : sys_diff;

    long total_cpu_time = cpu_diff + sys_diff;

    // Calculate percentage
    float cpu_usage = ((float)total_cpu_time * 100.0f) / (float)elapsed_us;

    // Normalize for multi-core systems
    long num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_cores > 0) {
        cpu_usage /= num_cores;
    }

    // Store current values for next calculation
    delta->prev_cpu_time = curr_cpu_us;
    delta->prev_sys_time = curr_sys_us;
    delta->prev_time = current_time;

    return (cpu_usage < 0.0f) ? 0.0f : 
           (cpu_usage > 100.0f) ? 100.0f : cpu_usage;
}

/**
 * @brief Calculates memory usage percentage for a process
 *
 * Computes memory usage by comparing process memory size with total system memory.
 *
 * @param proc Pointer to process data structure
 * @return float Memory usage percentage (0-100), 0.0f if error
 */
float calculate_mem_percentage(const ProcData *proc) {
    if (!proc || proc->memory_size < 0) return 0.0f;

    unsigned long long pages = (unsigned long long)sysconf(_SC_PHYS_PAGES);
    unsigned long long page_size = (unsigned long long)sysconf(_SC_PAGE_SIZE);
    
    if (pages <= 0 || page_size <= 0) return 0.0f;
    
    unsigned long long total_mem_kb = (pages * page_size) / 1024;
    if (total_mem_kb == 0) return 0.0f;
    
    double mem_percentage = ((double)proc->memory_size * 100.0) / (double)total_mem_kb;
    
    return (mem_percentage < 0.0) ? 0.0f :
           (mem_percentage > 100.0) ? 100.0f : (float)mem_percentage;
}

/**
 * @brief Updates CPU and memory metrics for all processes
 *
 * @param proc_data Array of process data structures
 * @param len Number of processes in array
 * @param deltas Array of CPU delta tracking structures
 */
void update_process_metrics(ProcData *proc_data, int len, CPUDelta *deltas) {
    if (!proc_data || !deltas || len <= 0) return;
    
    for (int i = 0; i < len; i++) {
        proc_data[i].percent_cpu = calculate_cpu_percentage(&proc_data[i], &deltas[i]);
        proc_data[i].percent_mem = calculate_mem_percentage(&proc_data[i]);
    }
}

/**
 * @brief Frees memory allocated for CPU delta tracking structures
 *
 * @param deltas Pointer to array of CPU delta structures
 */
void cleanup_cpu_deltas(CPUDelta *deltas) {
    free(deltas);
}