#ifndef PROC_METRICS_H
#define PROC_METRICS_H

#include "proc_data.h"
#include <sys/time.h>

/**
 * @struct CPUDelta
 * @brief Stores previous CPU measurements for calculating usage deltas
 *
 * This structure maintains the previous CPU time measurements and timestamp
 * for a process, enabling accurate CPU usage calculation between updates.
 */
typedef struct {
    long prev_cpu_time;      /**< Previous CPU time measurement in microseconds */
    long prev_sys_time;      /**< Previous system time measurement in microseconds */
    struct timeval prev_time; /**< Timestamp of previous measurement */
} CPUDelta;

/**
 * @brief Initializes an array of CPUDelta structures
 *
 * Allocates and initializes CPUDelta structures for tracking CPU usage
 * of multiple processes. Sets initial timestamps and zeroes all counters.
 *
 * @param len Number of processes to track (size of the array to allocate)
 * @return Pointer to allocated CPUDelta array, or NULL if allocation fails
 */
CPUDelta* init_cpu_deltas(int len);

/**
 * @brief Calculates CPU usage percentage for a single process
 *
 * Computes the CPU usage percentage by comparing current CPU times with
 * previous measurements stored in the CPUDelta structure. Updates the
 * delta structure with new values for the next calculation.
 *
 * @param proc Pointer to ProcData structure containing current process info
 * @param delta Pointer to CPUDelta structure containing previous measurements
 * @return CPU usage percentage as a float between 0 and 100
 */
float calculate_cpu_percentage(const ProcData *proc, CPUDelta *delta);

/**
 * @brief Calculates memory usage percentage for a single process
 *
 * Computes the memory usage percentage by comparing the process's memory size
 * with the total system memory. Converts memory sizes to consistent units
 * before calculation.
 *
 * @param proc Pointer to ProcData structure containing process memory info
 * @return Memory usage percentage as a float between 0 and 100
 */
float calculate_mem_percentage(const ProcData *proc);

/**
 * @brief Updates CPU and memory metrics for all processes
 *
 * Processes an array of ProcData structures, calculating and updating
 * CPU and memory percentages for each process. This is the main function
 * that should be called periodically to refresh process metrics.
 *
 * @param proc_data Array of ProcData structures to update
 * @param len Number of processes in the array
 * @param deltas Array of CPUDelta structures (one per process)
 */
void update_process_metrics(ProcData *proc_data, int len, CPUDelta *deltas);

/**
 * @brief Frees memory allocated for CPU deltas
 *
 * Properly cleans up the memory allocated by init_cpu_deltas.
 * Should be called when the CPU deltas are no longer needed.
 *
 * @param deltas Pointer to CPUDelta array to be freed
 */
void cleanup_cpu_deltas(CPUDelta *deltas);

#endif /* PROC_METRICS_H */
