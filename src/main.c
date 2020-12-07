/*
 * FreeRTOS Emulator for RTMCT validation - Main program
 *
 * This program takes arguments from cli and executes a taskset
 * imported from a file
 *
 * Written by Marcel Ebbrecht <marcel.ebbrecht@tu-dortmund.de>
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

// general settings with constants
#define mainGENERIC_PRIORITY (tskIDLE_PRIORITY)
#define mainGENERIC_STACK_SIZE ((unsigned short)25600)
#define PRIORITY_WORKER 1
#define PRIORITY_KILLER 2
#define PRINT_NUMBER_OF_PERIODS_PER_LINE 20

// enable output
#define TRACE

// we support up to 1000 tasks, prepare data structures
UBaseType_t tasksJobs[1000];
UBaseType_t tasksPeriods[1000];
TickType_t startTimes[1000];
UBaseType_t simulationId = ULONG_MAX;
TickType_t simulationDuration = INT_MAX;
UBaseType_t tasksCount = ULONG_MAX;

// default task
void vDefaultTask(void *pvParameters)
{
    UBaseType_t taskId = (UBaseType_t)pvParameters;
    tasksJobs[taskId] = 0;
    startTimes[taskId] = xTaskGetTickCount();
    for (;;) {
        // just increase jobcounter and wait
        tasksJobs[taskId] = tasksJobs[taskId] + 1;
        vTaskDelayUntil(&startTimes[taskId], tasksPeriods[taskId]);
    }
}

// end simulator task
void vKillSystem(void *pvParameters)
{
    // let task sleep until end of simulation duration
    TickType_t startTime = xTaskGetTickCount();
    vTaskDelayUntil(&startTime, simulationDuration);

    // print stats prior exit
#ifdef TRACE
    prints("\n");
    prints("Simulation ended after %d ticks, task stats:\n",
           xTaskGetTickCount());
    prints("\n");
    prints("Number\t\tPeriod\t\tJobs\n");
    for (unsigned short i = 0; i < tasksCount; i++) {
        prints("%d\t\t%d\t\t%d\n", (i + 1), tasksPeriods[i],
               tasksJobs[i]);
    }
    prints("\n");
#endif

    // stop scheduler
    vTaskEndScheduler();
}

// help message and error code handling
void printHelp(BaseType_t errorcode)
{
    switch (errorcode) {
        case 1:
            prints("\nError: Invalid number of arguments\n");
            break;
        case 2:
            prints("\nError: Invalid mode\n");
            break;
        case 3:
            prints("\nError: Invalid filename\n");
            break;
        case 4:
            prints("\nError: Invalid taskset definition\n");
            break;
    }

    prints("\nUsage:    FreeRTOS_Emulator MODE TASKSET\n\n");
    prints("          MODE      1 or 0 (whether run taskset or not)\n");
    prints("          TASKSET   path to filename containing taskset information\n");
    prints("\nExample:  FreeRTOS_Emulator 1 ../taskset.txt\n\n");
}

int main(int argc, char *argv[])
{
    // input checking state variables
    BaseType_t runnable = pdFALSE;
    BaseType_t errorcode = 0;

    // initialize values for arrays
    for (UBaseType_t i = 0; i < 1000; i++) {
        tasksJobs[i] = 0;
        tasksPeriods[i] = 0;
    }

    // test number parameters
    if (argc == 3) {
        // test mode
        if ((strcmp("0", argv[1]) == 0) ||
            (strcmp("1", argv[1]) == 0)) {
            // test file exists
            if (access(argv[2], F_OK) != -1) {
                // extract taskset
                UBaseType_t periodReads = 0;
                FILE *input_file;
                input_file = fopen(argv[2], "r");
                while (!feof(input_file) &&
                       periodReads <= tasksCount) {
                    if (simulationId == ULONG_MAX) {
                        fscanf(input_file, "%lu",
                               &simulationId);
                    }
                    else {
                        if (simulationDuration ==
                            INT_MAX) {
                            fscanf(input_file, "%u",
                                   &simulationDuration);
                        }
                        else {
                            if (tasksCount ==
                                ULONG_MAX) {
                                fscanf(input_file,
                                       "%lu",
                                       &tasksCount);
                            }
                            else {
                                fscanf(input_file,
                                       "%lu",
                                       &tasksPeriods
                                       [periodReads++]);
                            }
                        }
                    }
                }
                fclose(input_file);
                if (simulationId < ULONG_MAX &&
                    simulationDuration < INT_MAX &&
                    tasksCount < ULONG_MAX &&
                    tasksCount == (periodReads - 1)) {
                    runnable = pdTRUE;
                }
                else {
                    errorcode = 4;
                }
            }
            else {
                errorcode = 3;
            }
        }
        else {
            errorcode = 2;
        }
    }
    else {
        errorcode = 1;
    }

    if (runnable) {
        // print simulation details
#ifdef TRACE
        prints("\n");
        prints("Simulation ID:          %d\n", simulationId);
        prints("Simulation duration:    %d\n", simulationDuration);
        if (strcmp("1", argv[1]) == 0) {
            prints("Run tasks:              yes\n");
        }
        else {
            prints("Run tasks:              no\n");
        }
        prints("Number of tasks:        %d\n", tasksCount);
        prints("Periods:                %u", tasksPeriods[0]);

        // print task periods
        for (UBaseType_t i = 1; i < tasksCount; i++) {
            prints("\t%d", tasksPeriods[i]);
            if ((i + 1) % PRINT_NUMBER_OF_PERIODS_PER_LINE == 0) {
                prints("\n                ");
            }
        }
        prints("\n\n");
#endif

        // create worker tasks
        for (UBaseType_t i = 0; i < tasksCount; i++) {
            tasksJobs[i] = 0;
            xTaskCreate(vDefaultTask, "Default Task",
                        mainGENERIC_STACK_SIZE * 2, (void *)i,
                        PRIORITY_WORKER, NULL);
        }

        // create killer task
        xTaskCreate(vKillSystem, "Ending Task",
                    mainGENERIC_STACK_SIZE * 2, NULL, PRIORITY_KILLER,
                    NULL);

        // start scheduler
        if (strcmp("1", argv[1]) == 0) {
            vTaskStartScheduler();
        }

        // end program
        return EXIT_SUCCESS;
    }
    else {
        printHelp(errorcode);
        return EXIT_FAILURE;
    }
}

// cppcheck-suppress unusedFunction
__attribute__((unused)) void vMainQueueSendPassed(void)
{
    /* This is just an example implementation of the "queue send" trace hook. */
}

// cppcheck-suppress unusedFunction
__attribute__((unused)) void vApplicationIdleHook(void)
{
#ifdef __GCC_POSIX__
    struct timespec xTimeToSleep, xTimeSlept;
    /* Makes the process more agreeable when using the Posix simulator. */
    xTimeToSleep.tv_sec = 1;
    xTimeToSleep.tv_nsec = 0;
    nanosleep(&xTimeToSleep, &xTimeSlept);
#endif
}
