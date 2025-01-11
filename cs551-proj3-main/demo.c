#include "proc_monitor.h"
#include <stdlib.h>

int main(char argc, char *argv[]) {
    int num_procs_display = 10;
    int interval = 5;
    if (argc == 3) {
        num_procs_display = atoi(argv[1]);
        interval = atoi(argv[2]);
    }
    return proc_monitor(num_procs_display, interval);
}