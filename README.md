<!--- FreeRTOS Emulator for RTMCT validation - README -->
<!--- Written by Marcel Ebbrecht <marcel.ebbrecht@tu-dortmund.de> -->

# FreeRTOS Emulator for RTMCT validation

This is just a copy of the [original project](https://github.com/alxhoff/FreeRTOS-Emulator) to be used as a submodule in the [RTMCT emulator test suite](https://github.com/marcelebbrecht/rtmct-emulator-test) for validating improvements on timer queue management mentionen in the [RTMCT project](https://github.com/marcelebbrecht/rtmct). It imports a taskset from a file provided by command-line.

## Branches

There exists two main branches:
- master: It includes a new main.c that reads a given taskset from a file. For more information please refer to the [RTMCT emulator test suite](https://github.com/marcelebbrecht/rtmct-emulator-test).
- boi: This containt a modified timer manager that is described in the [RTMCT project](https://github.com/marcelebbrecht/rtmct).

## Usage

- Follow build instructions from [original project](https://github.com/alxhoff/FreeRTOS-Emulator)
- To run simulation you have to pass two arguments: 
 - MODE: 1 to run the scheduler or 0 if not (to estimate overhead of system without running the taskset)
 - TASKSET: relative path to the file containing the taskset
- Example command: ```FreeRTOS_Emulator 1 taskset.txt```

## Taskset file

The file containing the taskset must be provided in a specific format:

```
TASKSET_ID
DURATION_TO_SIMULATE
NUMBER_OF_TASKS
PERIOD1
PERIOD2
PERIOD3
...
```

Example with id 1, 2000 ticks and 12 tasks:

```
1
2000
12
5
5
10
10
10
25
50
100
500
1000
1000
1000
```