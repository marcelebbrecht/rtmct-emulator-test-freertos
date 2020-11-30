# FreeRTOS Emulator for RTMCT validation - vanilla version

This is just a copy of the [original project](https://github.com/alxhoff/FreeRTOS-Emulator) to be used as a submodule in the [RTMCT emulator test suite](https://github.com/marcelebbrecht/rtmct-emulator-test) for validating improvements on timer queue management mentionen in the [RTMCT project](https://github.com/marcelebbrecht/rtmct).

## Branches

There exists two main branches:
- master: It includes a new main.c that reads a given taskset from a file. For more information please refer to the [RTMCT emulator test suite](https://github.com/marcelebbrecht/rtmct-emulator-test).
- boi: This containt a modified timer manager that is described in the [RTMCT project](https://github.com/marcelebbrecht/rtmct).
