#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "proc_data.h"
#include "proc_metrics.h"
#include "display.h"

void sigint_handler(int sig) {

    cleanup_display();
    exit(0);
}

int proc_monitor(int num_procs_display, int interval) {
    ProcData *proc_data;
    int num_procs;

    signal(SIGINT, sigint_handler);

    // Retrieve process data
    num_procs = get_proc_data(&proc_data);
    if (num_procs < 0) {
        fprintf(stderr, "Error retrieving process data.\n");
        return EXIT_FAILURE;
    }

    // Initialize CPU deltas
    CPUDelta *deltas = init_cpu_deltas(num_procs);
    if (deltas == NULL) {
        fprintf(stderr, "Error initializing CPU deltas.\n");
        free(proc_data);
        return EXIT_FAILURE;
    }

    // Start refreshing the display every "interval" seconds
    refresh_display(proc_data, num_procs, deltas, interval, num_procs_display);

    return EXIT_SUCCESS;
}