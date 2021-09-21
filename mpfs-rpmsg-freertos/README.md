# PolarFire SoC AMP (Asymmetric Multiprocessing) FreeRTOS example

This example project provides a simple application running FreeRTOS. This application can be used in an Asymmetric Multiprocessing (AMP) software architecture.

The example project has two FreeRTOS tasks:

Task1: displays messages over the UART3 console every 300ms

Task2: displays messages over the UART3 console and toggles GPIO2 pin 16 (LED1) every 500ms

The project can be compiled using SoftConsole or externally by using the provided Makefile.
This allows to integrate and build the application in Yocto and Buildroot environments.
## How to use this example
### Linux + FreeRTOS AMP configuration

This project is automatically built by Yocto and Buildroot when using the AMP machine. 

The application is configured to run from DDR on U54_3 application core.

Instructions on how to build and run the Linux + FreeRTOS demo are available [here](https://github.com/polarfire-soc/polarfire-soc-documentation/tree/master/asymmetric-multiprocessing/amp.md).

### Other AMP configurations

This project can also be used in a custom AMP software configuration (i.e. FreeRTOS + Bare metal) by using the Hart Software Services (HSS) payload generator.

To build the project and generate a HSS payload for a custom AMP software architecture:

1. (Optional) modify the start address of your application using the ddr_cached_32bit memory section in the mpfs-ddr-loaded-by-boot-loader.ld linker script located in the boards/icicle-kit-es/platform_config/linker folder

2. (Optional) modify the MPFS_HAL_FIRST_HART and MPFS_HAL_LAST_START macros in the mss_sw_config.h header file if targetting the application to another hart

3. Build this example project using SoftConsole or the Makefile provided. Information on how to build using the makefile is provided [here](#makefile-build)

4. Generate a HSS payload containing the output ELF file of this FreeRTOS application (mpfs-amp-freertos.elf) and the additional binary of your second context

Instructions on how to generate an HSS payload are described [here](https://github.com/polarfire-soc/polarfire-soc-documentation/blob/master/software-development/hss-payloads.md).

#### (Optional) Building the project using the Makefile<a name="makefile-build"></a>

If building the project outside of SoftConsole by using the provided Makefile, the system PATH must be configured to include the location of a RISC-V toolchain and Python version 3.x.

For example, to use SoftConsole built-in RISC-V toolchain:

On Linux:
```
export PATH=$PATH:$HOME/Microchip/SoftConsole-v2021.1/python3/bin:$HOME/Microchip/SoftConsole-v2021.1/riscv-unknown-elf-gcc/bin
make
```

On Windows:

If using Windows, Python version 3.x should be added to the system path. This could be achieved by installing Python 3 on Windows or by using SoftConsole built-in python.

To use SoftConsole built-in python:
```
copy C:\Microchip\SoftConsole-v2021.1\python\python.exe C:\Microchip\SoftConsole-v2021.1\python\python3.exe
```

To configure the path con Windows:
```
path %SystemRoot%;%SystemRoot%;C:\Microchip\SoftConsole-v2021.1\build_tools\bin;C:\Microchip\SoftConsole-v2021.1\python;C:\Microchip\SoftConsole-v2021.1\riscv-unknown-elf-gcc\bin
make
```
## How to debug the application in DDR

The application is configured to run from DDR. It must be loaded to DDR using a previous stage program such as the HSS.

Pre-requisites: the Hart Software Services (HSS) should be built and loaded to the Icicle Kit before following the debugging steps described below.

1. Generate a HSS payload containing the mpfs-amp-freertos ELF file. Please see the [HSS payloads](https://github.com/polarfire-soc/polarfire-soc-documentation/blob/master/software-development/hss-payloads.md) documentation page for instructions on how to generate a HSS payload

2. Flash the generated payload in a non-volatile off chip memory such as eMMC or SD-card

3. On power-on, the Icicle Kit should boot the HSS and start the AMP FreeRTOS application

4. Connect using "mpfs-amp-freertos hw one-hart attach" debug configuration 

5. Add any breakpoints as desired

6. Press resume button on the debugger

8. The FreeRTOS AMP example project should continue running and displaying messages over UART3
 

