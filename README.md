# CS 551 Project 3: Process Monitor

In this project, I implemented a system level function that displays the process data for the number of processes the programmer specifies. This function required the following steps to implement:

1. Parsing process directories for the desired data and aggregating this data into an array of structures.

2. Calculating the %CPU and %MEM usage of each process relative to each display interval.

3. Displaying the top processes according to %CPU usage in specified time intervals.

To compile and run the process monitor from the demo.c program:

```bash
make
gcc -o demo demo.c proc_monitor.a
./demo
```

## Fetching Process Data Module


### Overview

The desired data was retrieved by iterating through each process in the `/proc` directory and parsing data from the `/proc/[pid]/status`, `/proc/[pid]/stat`, and `/proc/loadavg` directories. 

### /proc/[pid]/loadavg
The number of processes running on the system was parsed from `/proc/loadavg`. 

### /proc/[pid]/status
The process name, pid, memory size, and state was parsed from `/proc/[pid]/status`.

### /proc/[pid]/stat
The CPU time, system time, priority, and nice value were parsed from `/proc/[pid]/stat`. 

### Data Aggregation
All of this data was stored in an array of `ProcData` structs to then be processed for %CPU and %MEM calculations and displayed in the terminal. 

### References
See chapter 12 of "The 
Linux Programming Interface" textbook by Michael Kerrisk for an in depth explanation of the `\proc` file system.


---------------------------------------------------------------------------------------------------

## Overview
This module provides functionality for monitoring CPU and memory usage of Linux processes by reading and analyzing data from the `/proc` filesystem. It calculates accurate CPU percentages accounting for system clock ticks and multi-core systems, as well as memory usage relative to total system memory.

## Key Features
- Real-time CPU usage calculation per process
- Memory usage monitoring
- Multi-core CPU normalization
- Resource cleanup utilities

## Functions

### CPU Delta Management
- `init_cpu_deltas()`: Initializes tracking structures for CPU usage calculation
- `cleanup_cpu_deltas()`: Frees allocated resources

### Metrics Calculation
- `calculate_cpu_percentage()`: Computes CPU usage (0-100%) for a process
- `calculate_mem_percentage()`: Computes memory usage (0-100%) for a process
- `update_process_metrics()`: Updates both CPU and memory metrics for all processes

## Implementation Details
- Uses `/proc/<pid>/stat` for CPU time data
- Converts system clock ticks to microseconds for accurate CPU calculation
- Handles multi-core systems by normalizing CPU percentages
- Uses `sysconf()` to get system memory information

## Dependencies
- Standard C libraries
- Linux proc filesystem
- POSIX-compliant system

---------------------------------------------------------------------------------------------------

## Overview
The provided files, `display.c` and `proc_monitor.c`, work together to implement a process monitoring system that retrieves, processes, and displays system process information in real-time. Here's a breakdown of their functionalities:

## display.c
This file handles the user interface and output formatting for the process monitor. Key functionalities include:
### 1. Screen Management:
Clears and manages the terminal screen for smooth updates using ANSI escape sequences (function: `clear_screen`), preparing 
it for periodic display of process data. It also manages terminal modes for smooth updates and navigation (function: 
`cleanup_display`).

### 2. Sorting and Display:
Sorts processes by CPU usage in descending order (function: `compare_by_cpu`) and displays them in a formatted table 
(function: `display_top_processes`), while ensuring long process names are truncated for consistent layout (function: 
`truncate_names`).

### 3.Summarization:
Calculates and displays aggregate statistics such as total process and memory consumption (functions: `calculate_summary` 
and `display_summary`) alongside the process table.

### 4. Real Time Updates:
Periodically refreshes and updates the process data display (function: `refresh_display`), ensuring the latest metrics are 
shown by calling external functions like `update_process_metrics` and `get_proc_data`.

### 5. Cleanup:
Restores the terminal to its original state upon program exit, ensuring a clean termination (function:`cleanup_display`).

## proc_monitor.c
The main control file that manages the overall execution of the process monitor. This function take two parameters, `num_procs_display` and `interval`, which allows the programmer to specify the number of processes to display and the time interval for refereshing the display. It handles control-c signal interruptions (function: `sigint_handler`), retrieves process data (function: `get_proc_data`), initializes CPU usage tracking (function: `init_cpu_deltas`), and calls the display refresh function (`function: refresh_display`). It also manages resources and runs the monitoring loop with periodic updates.

---------------------------------------------------------------------------------------------------

## Overview
To validate the functionality of the process monitor, a set of test cases was implemented in the `test_proc_monitor.c` program. These test cases verify different aspects of the process monitor system, ensuring accuracy and reliability in data retrieval, calculation, and display.

## Test Cases

### 1. Process Data Retrieval
- Verifies that process information is correctly retrieved from the `/proc` file system
- Checks if the number of processes and their details (PID, Name, State, Memory, etc.) match the system's actual processes

### 2. CPU and Memory Utilization Calculations
- Validates the correctness of %CPU and %MEM calculations
- Ensures that utilization metrics are updated correctly for all retrieved processes

### 3. Top Processes Display
- Displays all retrieved processes with their calculated metrics
- Confirms that the display functionality is working as intended

### 4. Test for Handling Large Numbers of Processes
- Forks 100 child processes to simulate a heavy-load environment
- Verifies that the system can retrieve and display process information accurately even under heavy process load

## Running the Tests
```bash
make
gcc -o test_proc_monitor test_proc_monitor.c proc_monitor.a
./test_proc_monitor
```

---------------------------------------------------------------------------------------------------
 ⁠
