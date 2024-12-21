# README: RTOS Concepts with Driver Integration

## Overview

This code demonstrates an application where real-time scheduling concepts are combined with device driver interactions. It focuses on scheduling periodic and aperiodic tasks using Rate Monotonic Scheduling (RMS) in a real-time operating system (RTOS) context. Additionally, it integrates interaction with a device driver for task control and communication.

## Key Concepts

1. **Periodic and Aperiodic Tasks**: 
   - Periodic tasks are scheduled with fixed time intervals (periods), while aperiodic tasks are invoked based on specific conditions or events.
   
2. **Rate Monotonic Scheduling (RMS)**:
   - Periodic tasks are assigned priorities inversely proportional to their periods. Shorter periods have higher priorities.

3. **Device Driver Integration**:
   - The application interacts with a device driver (`/dev/taskdriver`) for task control and communication.

4. **Task Execution**:
   - Each task performs specific computations, interacts with the driver, and executes either periodic or aperiodic code.

5. **Thread Management**:
   - Each task runs as a separate thread with appropriate scheduling and affinity to ensure they run on specified processors.

6. **Synchronization Mechanisms**:
   - Mutexes and condition variables are used for synchronizing aperiodic tasks.

## Detailed Workflow

- **Task Initialization**:
  - Task periods are defined in nanoseconds.
  - The main thread initializes each task with proper scheduling parameters and attributes.

- **Periodic Task Execution**:
  - Periodic tasks are executed in a loop, with timing controlled by `clock_nanosleep`.
  - Each periodic task writes its identifier (`[i]`) and signals completion back to the device driver.

- **Aperiodic Task Execution**:
  - Aperiodic tasks are executed in response to specific conditions or signals. 
  - Mutex and condition variables are used to synchronize aperiodic tasks across threads.

- **Driver Interaction**:
  - The device driver (`/dev/taskdriver`) facilitates task management by handling read/write operations to control task execution.

- **Affinity and Scheduling**:
  - Each task thread has CPU affinity set to ensure they run on designated cores.
  - Scheduling policies (`SCHED_FIFO`) and priorities are explicitly set to meet real-time constraints.

## Example Output

This project, when run, will continuously monitor task execution and display missed deadlines and worst-case execution times for each task. Additionally, synchronization of aperiodic tasks through condition variables can be observed.

---

## Attachments

![Example Image](output1.jpg)
![Example Image](output2.jpg)

