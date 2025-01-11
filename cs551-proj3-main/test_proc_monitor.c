#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "proc_data.h"
#include "proc_metrics.h"

// Declare functions from display.c directly
void clear_screen();
int compare_by_cpu(const void *a, const void *b);
void display_top_processes(ProcData *proc_data, int len);
void refresh_display(ProcData *proc_data, int len, CPUDelta *deltas, int interval);

// Function to print process data for testing
void print_proc_data_for_test(ProcData *proc_data, int length) {
    printf("PID\tName\t\tCPU%%\tMEM%%\tState\tMemory (KB)\n");
    for (int i = 0; i < length; i++) {
        printf("%ld\t%-10s\t%.2f\t%.2f\t%-10s\t%ld\n",
               proc_data[i].pid, proc_data[i].name,
               proc_data[i].percent_cpu, proc_data[i].percent_mem,
               proc_data[i].state, proc_data[i].memory_size);
    }
    printf("\n");
}

// Test for normal process data retrieval
void test_process_data_retrieval() {
    printf("Running Process Data Retrieval Test...\n");
    ProcData *proc_data = NULL;
    int proc_count = get_proc_data(&proc_data);

    if (proc_count < 0) {
        printf("Error: get_proc_data failed with code %d\n", proc_count);
        return;
    }

    printf("Retrieved %d processes.\n", proc_count);
    print_proc_data_for_test(proc_data, proc_count);

    free(proc_data); // Free allocated memory
}

// Test for CPU and Memory Percentage Calculations
void test_cpu_memory_calculations() {
    printf("Running CPU and Memory Calculations Test...\n");

    ProcData *proc_data = NULL;
    int proc_count = get_proc_data(&proc_data);

    if (proc_count < 0) {
        printf("Error: get_proc_data failed with code %d\n", proc_count);
        return;
    }

    // Initialize CPU deltas
    CPUDelta *cpu_deltas = init_cpu_deltas(proc_count);
    if (!cpu_deltas) {
        printf("Error: Failed to initialize CPU deltas.\n");
        free(proc_data);
        return;
    }

    // Update metrics
    update_process_metrics(proc_data, proc_count, cpu_deltas);

    // Print updated process data
    printf("Updated Process Data with CPU and Memory Metrics:\n");
    print_proc_data_for_test(proc_data, proc_count);

    // Cleanup
    cleanup_cpu_deltas(cpu_deltas);
    free(proc_data);
}

// Test for Top Process Display
void test_top_process_display() {
    printf("Running Top Process Display Test...\n");

    ProcData *proc_data = NULL;
    int proc_count = get_proc_data(&proc_data);

    if (proc_count < 0) {
        printf("Error: get_proc_data failed with code %d\n", proc_count);
        return;
    }

    // Initialize CPU deltas
    CPUDelta *cpu_deltas = init_cpu_deltas(proc_count);
    if (!cpu_deltas) {
        printf("Error: Failed to initialize CPU deltas.\n");
        free(proc_data);
        return;
    }

    // Update metrics
    update_process_metrics(proc_data, proc_count, cpu_deltas);

    // Display top processes
    display_top_processes(proc_data, proc_count);

    // Cleanup
    cleanup_cpu_deltas(cpu_deltas);
    free(proc_data);
}

// Function to create a large number of child processes
void fork_child_processes(int num_children) {
    for (int i = 0; i < num_children; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process: Simulate some work and exit
            sleep(10); // Keep the child process alive for a short time
            exit(0);
        }
    }
}

// Test for handling a large number of processes
void test_large_number_of_processes() {
    printf("Running Large Number of Processes Test...\n");

    int num_children = 100; // Number of child processes to fork
    fork_child_processes(num_children);

    // Parent process retrieves process data
    ProcData *proc_data = NULL;
    int proc_count = get_proc_data(&proc_data);

    if (proc_count < 0) {
        printf("Error: get_proc_data failed with code %d\n", proc_count);
        return;
    }

    printf("Retrieved %d processes, including %d child processes.\n", proc_count, num_children);
    printf("Displaying first 10 processes for verification:\n");
    for (int i = 0; i < (proc_count < 10 ? proc_count : 10); i++) {
        printf("PID: %ld, Name: %s, State: %s, Memory: %ld KB\n",
               proc_data[i].pid, proc_data[i].name, proc_data[i].state, proc_data[i].memory_size);
    }

    // Cleanup
    free(proc_data);

    // Wait for all child processes to terminate
    for (int i = 0; i < num_children; i++) {
        wait(NULL); // Wait for each child process to exit
    }

    printf("Test for large number of processes completed.\n");
}

// Main function to run all tests
int main() {
    printf("Starting Test Program...\n");

    test_process_data_retrieval();
    test_cpu_memory_calculations();
    test_top_process_display();
    test_large_number_of_processes();

    printf("All tests completed.\n");
    return 0;
}
