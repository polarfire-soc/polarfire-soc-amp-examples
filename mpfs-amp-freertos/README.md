# PolarFire SoC AMP FreeRTOS example

This example project provides a simple application running FreeRTOS . This application can be used in an AMP software architecture.

The example project has two FreeRTOS tasks:

Task1: displays messages over the UART3 console every 300ms

Task2: displays messages over the UART3 console  and toggles GPIO2_16 every 500ms

The project can be compiled using SoftConsole or externally by using the provided makefile. 
This allows to integrate and build the application in Yocto and Buildroot environments.

## How to use this example

### Linux + FreeRTOS AMP configuration

This project is automatically built by Yocto when using the icicle-kit-es-amp machine. 

The application is configured to run from DDR on U54_3 application core.

Instructions on how to build Yocto icicle-kit-es-amp machine are described [here](https://github.com/polarfire-soc/meta-polarfire-soc-yocto-bsp)

Instructions on how to build Buildroot with AMP configuration are described [here](https://github.com/polarfire-soc/polarfire-soc-buildroot-sdk)

On connecting Icicle kit J11 to the host PC, you should see 4 COM port interfaces connected. To use this project configure the COM port interface 3 as below:

- 115200 baud
- 8 data bits
- 1 stop bit
- no parity

### Custom AMP configuration

This project can also be used in a custom AMP software configuration (i.e. FreeRTOS + Bare metal) by using the HSS payload generator.

To generate an HSS payload for a custom AMP software architecture:

1. (Optional) modify the MPFS_HAL_FIRST_HART and MPFS_HAL_LAST_START macros in the mss_sw_config.h header file if targetting the application to another hart. Default hart is U54-1

2. (Optional) modify the start address of your application using the ddr_cached_32bit memory section in the mpfs-ddr-loaded-by-boot-loader.ld linker script

3. Build this example project using Softconsole or the provided makefile

4. Generate an HSS payload using the output ELF file 

Instructions on how to generate an HSS payload are described [here](https://github.com/polarfire-soc/polarfire-soc-documentation/blob/master/software-development/hss-payloads.md)


## How to debug the application in DDR

The application is configured to run from DDR. It must be loaded to DDR using a previous stage program. 

The MPFS HAL ddr demo program is useful for this while developing and debugging. The Hart Software Services (HSS) can be used once your program is developed. 

1. Build the project with the following configurations:

1.1 set both MPFS_HAL_FIRST_HART and MPFS_HAL_LAST_START to 1 in mss_sw_config.h

2. modify the start address of your application to 0x80000000 using the ddr_cached_32bit memory section in the mpfs-ddr-loaded-by-boot-loader.ld linker script

3. Build the project using Softconsole or makefile provided

4. Load the [MPFS HAL DDR demo](https://github.com/polarfire-soc/polarfire-soc-bare-metal-library/tree/master/examples/mpfs-hal/mpfs-hal-ddr-demo)  program to eNVM. This boots the Icicle kit and prints information on UART0



5. Use Ymodem to load this program to DDR (use bin file located in the Debug directory of this example project)

6. Connect using "mpfs-uart-mac-freertos-lwip-DDR-Release" debug 

7. Add any break-points as desired desire

8. Press resume button on the debugger

9. Select option 7 "Start U54_1 from DDR @0x80000000" 

10. The FreeRTOS AMP example project should start running and displaying messages over UART3.