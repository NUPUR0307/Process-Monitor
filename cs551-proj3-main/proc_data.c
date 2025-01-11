#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <dirent.h>
#include <string.h>
#include "proc_data.h"
#include "display.h"


// For debugging
void print_proc_data(ProcData *proc_data, int len) {
    for (int i = 0; i < len; i++) {
        printf("Name: %s\n", proc_data[i].name);
        printf("Pid: %ld\n", proc_data[i].pid);
        printf("Mem size: %ld\n", proc_data[i].memory_size);
        printf("State: %s\n", proc_data[i].state);
        printf("Priority: %d\n", proc_data[i].priority);
        printf("Nice: %d\n", proc_data[i].nice);
        printf("CPU time: %ld\n", proc_data[i].cpu_time);
        printf("System time: %ld\n", proc_data[i].sys_time);
        printf("\n");
    }
}

char is_integer(const char *dir_name) {
    for (int i = 0; dir_name[i] != '\0'; i++) {
        if (!(48 <= dir_name[i] && dir_name[i] <= 57)) {
            return 0;
        }
    }
    return 1;
}

int get_num_procs() {
    int num;
    FILE *loadavg = fopen("/proc/loadavg", "r");
    if (loadavg == NULL) {
        perror("fopen");
        return -1;
    }

    char line[256];
    if (fgets(line, sizeof(line), loadavg) != NULL) {
        sscanf(line, "%*f %*f %*f %*d/%d", &num);
    }

    if (fclose(loadavg) == EOF) {
        perror("fclose");
        return -1;
    }

    return num;
}

int is_whitespace(char *c) {
    if (*c == ' ' || *c == '\t') {
        return 1;
    }
    return 0;
}

void init_procdata(ProcData **proc_data, int len) {
    for (int i = 0; i < len; i++) {
        (*proc_data)[i].cpu_time = 0;
        (*proc_data)[i].memory_size = 0;
        (*proc_data)[i].nice = 0;
        (*proc_data)[i].percent_cpu = 0.0;
        (*proc_data)[i].percent_mem = 0.0;
        (*proc_data)[i].pid = 0;
        (*proc_data)[i].priority = 0;
        (*proc_data)[i].sys_time = 0.0;
    }
}

int get_proc_data(ProcData **proc_data) {

    // Get the number of processes on the system
    int num_procs = get_num_procs();

    // Allocate proc data array
    *proc_data = (ProcData *) malloc(num_procs * sizeof(ProcData));

    init_procdata(proc_data, num_procs);

    DIR *dir = opendir("/proc");
    struct dirent *dir_entry;

    int i = 0;
    while ((dir_entry = readdir(dir)) != NULL) {

        if (is_integer(dir_entry->d_name)) {

            char status_dir[FILENAME_MAX];
            if (snprintf(status_dir, sizeof(status_dir), "/proc/%s/status", dir_entry->d_name) < 0) {
                perror("snprintf");
                return -1;
            }

            FILE *status = fopen(status_dir, "r");
            if (status == NULL) {
                perror("fopen");
                return -1;
            }

            char line[512];
            char data[256];
            
            // Get the desired proc data
            while (fgets(line, sizeof(line), status) != NULL) {

                if (strncmp(line, "Pid:", 4) == 0) {

                    if (sscanf(line, "Pid:%255[^\n]", data) == EOF) {
                        perror("sscanf");
                        return -1;
                    }

                    char *pid = data;
                    while (is_whitespace(pid)) {
                        pid++;
                    }
                    if (((*proc_data)[i].pid = atol(pid)) == 0) {
                        perror("atol");
                        return -1;
                    }

                } else if (strncmp(line, "State:", 6) == 0) {
                    if (sscanf(line, "State:%255[^\n]", data) == EOF) {
                        perror("sscanf");
                        return -1;
                    }
                    char *state = data;
                    while (is_whitespace(state)) {
                        state++;
                    }
                    strncpy((*proc_data)[i].state, state, sizeof((*proc_data)[i].state));
                } else if (strncmp(line, "VmRSS:", 6) == 0) {
                    if (sscanf(line, "VmRSS:%255[^\n]", data) == EOF) {
                        perror("sscanf");
                        return -1;
                    }
                    char *memory = data;
                    while (is_whitespace(memory)) {
                        memory++;
                    }
                    (*proc_data)[i].memory_size = atol(memory);
                } else if (strncmp(line, "Name:", 5) == 0) {
                    if (sscanf(line, "Name:%255[^\n]", data) == EOF) {
                        perror("sscanf");
                        return -1;
                    }
                    char *name = data;
                    while (is_whitespace(name)) {
                        name++;
                    }
                    strncpy((*proc_data)[i].name, name, sizeof((*proc_data)[i].name));
                }
            }

            char stat_dir[FILENAME_MAX];
            if (snprintf(stat_dir, sizeof(stat_dir), "/proc/%s/stat", dir_entry->d_name) < 0) {
                perror("snprintf");
                return -1;
            }

            FILE *stat = fopen(stat_dir, "r");
            if (stat == NULL) {
                perror("fopen");
                return -1;
            }
            if (fgets(line, sizeof(line), stat) != NULL) {
                if (sscanf(line, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %ld %ld %*d %*d %d %d",
                        &(*proc_data)[i].cpu_time, &(*proc_data)[i].sys_time, &(*proc_data)[i].priority, &(*proc_data)[i].nice) == -1) {
                            perror("sscanf");
                            return -1;
                        }
            }
            if (fclose(stat) == EOF) {
                perror("fclose");
                return -1;
            }
            if (fclose(status) == EOF) {
                perror("fclose");
                return -1;
            }
            i++;
        }
    }
    if (closedir(dir) == -1) {
        perror("closedir");
        return -1;
    }

    return i;
}