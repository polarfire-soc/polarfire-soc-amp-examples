# PolarFire SoC RPMsg FreeRTOS Example

This project contains a number of applications demos that showcase the usage of RPMsg (Remote Processor Messaging) framework to send messages between a FreeRTOS context and a remote software context (i.e. Linux, bare metal or another FreeRTOS context) using the RPMsg-lite framework.

This project contains two different build configurations:

- Remote Build Configuration: Builds an application in RPMsg Remote mode

- Master Build Configuration: Builds an application in RPMsg Master mode

Different combinations of operating systems can be supported in a master and remote role. For instance:

- FreeRTOS + FreeRTOS

- FreeRTOS + Bare Metal

- Linux + FreeRTOS

The project can be compiled using SoftConsole or externally by using the provided Makefile.
This allows to integrate and build the application in Yocto and Buildroot environments.

## How to build this example
### Linux + FreeRTOS AMP configuration

This project is automatically built by Yocto and Buildroot when using the AMP machine. 

The application is configured to run from DDR on U54_4 application core.

Instructions on how to build and run the Linux + FreeRTOS demo are available [here](https://github.com/polarfire-soc/polarfire-soc-documentation/tree/master/asymmetric-multiprocessing/amp.md).

### FreeRTOS + FreeRTOS AMP Configuration

This project can also be used to build a Bare Metal (RPMsg Master) + Bare Metal (RPMsg Remote) AMP Configuration.

Detailed instructions on how to build the Bare Metal + Bare Metal RPMsg demo are available [here](https://github.com/polarfire-soc/polarfire-soc-documentation/blob/master/asymmetric-multiprocessing/rpmsg.md#freertos-freertos-demo).

### FreeRTOS + BM AMP Configuration

This project can also be used to build a FreeRTOS + Bare Metal AMP Configuration.

Detailed instructions on how to build the FreeRTOS + Bare Metal RPMsg demo are available [here](https://github.com/polarfire-soc/polarfire-soc-documentation/blob/master/asymmetric-multiprocessing/rpmsg.md#freertos-bm-demo).

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

1. Generate a HSS payload containing the mpfs-rpmsg-freertos ELF file. Please see the [HSS AMP payloads](https://github.com/polarfire-soc/polarfire-soc-documentation/blob/master/asymmetric-multiprocessing/rpmsg.md#amp-payloads) documentation section for instructions on how to generate a HSS AMP payload

2. Flash the generated payload in a non-volatile off chip memory such as eMMC or SD-card

3. On power-on, the Icicle Kit should boot the HSS and start the RPMsg FreeRTOS application

4. Connect using the "mpfs-rpmsg-freertos hw one-hart attach master" for debugging the RPMsg master application or "mpfs-rpmsg-freertos hw one-hart attach remote" debug configuration for debugging the RPMsg remote configuration.

5. Add any breakpoints as desired

6. Press resume button on the debugger

8. The FreeRTOS RPMsg example project should continue running and displaying messages over the associated UART

For more information about AMP and inter-core communication using RPMsg on PolarFire SoC, please refer to the [PolarFire SoC RPMsg documentation](https://github.com/polarfire-soc/polarfire-soc-documentation/blob/master/asymmetric-multiprocessing/rpmsg.md).

