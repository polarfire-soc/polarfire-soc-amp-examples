# PolarFire SoC AMP FreeRTOS example

This example project provides a simple application running FreeRTOS. This application can be used in an AMP software architecture.

The example project has two FreeRTOS tasks:

Task1: displays messages over the UART3 console every 300ms

Task2: displays messages over the UART3 console and toggles GPIO2_16 (LED1) every 500ms

The project can be compiled using SoftConsole or externally by using the provided makefile. 
This allows to integrate and build the application in Yocto and Buildroot environments.

## How to use this example

### Linux + FreeRTOS AMP configuration

This project is automatically built by Yocto and Buildroot when using the AMP machine. 

The application is configured to run from DDR on U54_3 application core.

Instructions on how to build and run the Linux + FreeRTOS demo are described [here](https://bitbucket.microchip.com/projects/FPGA_PFSOC_ES/repos/polarfire-soc-documentation/browse/asymmetric-multiprocessing/amp.md?at=refs%2Fheads%2Fdevelop-amp)

### Other AMP configurations

This project can also be used in a custom AMP software configuration (i.e. FreeRTOS + Bare metal) by using the HSS payload generator.

To build the project and generate a HSS payload for a custom AMP software architecture:

1. (Optional) modify the start address of your application using the ddr_cached_32bit memory section in the mpfs-ddr-loaded-by-boot-loader.ld linker script located in the boards/icicle-kit-es/platform_config/linker folder

2. (Optional) modify the MPFS_HAL_FIRST_HART and MPFS_HAL_LAST_START macros in the mss_sw_config.h header file if targetting the application to another hart.

3. Build this example project using SoftConsole or the Makefile provided

4. Generate a HSS payload containing the output ELF file of this FreeRTOS application and the additional binary of your second context.

Instructions on how to generate an HSS payload are described [here](https://github.com/polarfire-soc/polarfire-soc-documentation/blob/master/software-development/hss-payloads.md)


## How to debug the application in DDR

The application is configured to run from DDR. It must be loaded to DDR using a previous stage program such as the HSS.

Pre-requisites: the Hart Software Services (HSS) should be built and loaded to the Icicle Kit before following the debugging steps described below.

1. Generate a HSS payload containing the mpfs-amp-freertos ELF file. Please see the [HSS payloads](https://github.com/polarfire-soc/polarfire-soc-documentation/blob/master/software-development/hss-payloads.md) documentation page for instructions on how to generate a HSS payload.

2. Flash the generated payload in a non-volatile off chip memory such as eMMC or SD-card

3. On power-on, the Icicle Kit should boot the HSS and start the AMP FreeRTOS application

4. Connect using "mpfs-amp-freertos hw one-hart attach" debug configuration 

5. Add any breakpoints as desired

6. Press resume button on the debugger

8. The FreeRTOS AMP example project should continue running and displaying messages over UART3
 

