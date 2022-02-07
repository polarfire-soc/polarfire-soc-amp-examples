# PolarFire SoC RPMsg Bare Metal Example

This project contains a number of applications demos that showcase the usage of RPMsg (Remote Processor Messaging) framework to send messages between a BM (bare metal) context and a remote software context (i.e. another BM/FreeRTOS context) using the RPMsg-lite framework.

This project contains two different build configurations:

- Remote Build Configuration: Builds an application in RPMsg Remote mode.

- Master Build Configuration: Builds an application in RPMsg Master mode.

Different combinations of operating systems can be supported in a master and remote role. For instance:

- Bare metal + FreeRTOS

- Bare metal + Bare Metal

The project can be compiled using SoftConsole or externally by using the provided Makefile.
This allows to integrate and build the application in Yocto and Buildroot environments.

## How to build this example

### BM + BM AMP Configuration

This project can be used to build a Bare Metal (RPMsg Master) + Bare Metal (RPMsg Remote) AMP Configuration.

To build a BM + BM AMP configuration:

1. Build the mpfs-rpmsg-bm project in `Master` build mode configuration

2. Build the mpfs-rpmsg-bm project in `Remote` build mode configuration

3. Generate a payload using the reference `hss-payload-bm_bm.yaml` available in the resources directory:

```
    $ cd polarfire-soc-amp-examples/resources
    $ hss-payload-generator -c hss-payload-bm_bm.yaml payload_bm_bm.bin
```

4. Flash the payload from the tools/ directory to eMMC or SD-card following the steps shown in the [Updating PolarFire SoC Icicle-Kit FPGA Design and Linux Image](https://github.com/polarfire-soc/polarfire-soc-documentation/blob/master/boards/mpfs-icicle-kit-es/updating-icicle-kit/updating-icicle-kit-design-and-linux.md) document to program using USBImager or, for example, using dd on Linux:

```
sudo dd if=payload_bm_bm.bin of=/dev/sdX
```
> Be very careful while picking /dev/sdX device! Look at dmesg, lsblk, GNOME Disks, etc. before and after plugging in your USB flash device/uSD/SD to find a proper device. Double check it to avoid overwriting any of system disks/partitions!


Detailed instructions on how to build the Bare Metal + Bare Metal RPMsg demo are available [here](https://github.com/polarfire-soc/polarfire-soc-documentation/blob/master/asymmetric-multiprocessing/rpmsg.md#rpmsg-rtos-intro).

### FreeRTOS + BM AMP Configuration

This project can also be used to build a FreeRTOS + Bare Metal AMP Configuration.

To build a FreeRTOS + BM AMP configuration:

1. Build the mpfs-rpmsg-freertos project in `Master` build mode configuration

2. Build the mpfs-rpmsg-bm project in `Remote` build mode configuration

3. Generate a payload using the reference `hss-payload-freertos_bm.yaml` available in the resources directory:

```
    $ cd polarfire-soc-amp-examples/resources
    $ hss-payload-generator -c hss-payload-freertos_bm.yaml payload_freertos_bm.bin
```

4. Flash the payload from the tools/ directory to eMMC or SD-card following the steps shown in the [Updating PolarFire SoC Icicle-Kit FPGA Design and Linux Image](https://github.com/polarfire-soc/polarfire-soc-documentation/blob/master/boards/mpfs-icicle-kit-es/updating-icicle-kit/updating-icicle-kit-design-and-linux.md) document to program using USBImager or, for example, using dd on Linux:

```
sudo dd if=payload_freertos_bm.bin of=/dev/sdX
```
> Be very careful while picking /dev/sdX device! Look at dmesg, lsblk, GNOME Disks, etc. before and after plugging in your USB flash device/uSD/SD to find a proper device. Double check it to avoid overwriting any of system disks/partitions!

Detailed instructions on how to build the FreeRTOS + Bare Metal RPMsg demo are available [here](https://github.com/polarfire-soc/polarfire-soc-documentation/blob/master/asymmetric-multiprocessing/rpmsg.md#rpmsg-rtos-intro).

### (Optional) Building the project using the Makefile<a name="makefile-build"></a>

If building the project outside of SoftConsole by using the provided Makefile, the system PATH must be configured to include the location of a RISC-V toolchain and Python version 3.x.

For example, to use SoftConsole built-in RISC-V toolchain:

On Linux:
```
$ export PATH=$PATH:<SC_INSTALL_DIR>/python3/bin:<SC_INSTALL_DIR>/riscv-unknown-elf-gcc/bin
$ cd polarfire-soc-amp-examples/mpfs-rpmsg-bm
$ make
```

where `<SC_INSTALL_DIR>` is as a placeholder for the actual SoftConsole install directory. For example `$HOME/Microchip/SoftConsole-v2021.3-7.0.0.599`.

On Windows:

For building on Windows from the command line one must configure the path appropriately, e.g.:
```
C:\> path %SystemRoot%;%SystemRoot%;<SC_INSTALL_DIR>\build_tools\bin;<SC_INSTALL_DIR>\python3;<SC_INSTALL_DIR>\riscv-unknown-elf-gcc\bin
C:\> cd polarfire-soc-amp-examples\mpfs-rpmsg-bm
C:\> make
```

where `<SC_INSTALL_DIR>` is as a placeholder for the actual SoftConsole install directory. For example `C:\Microchip\SoftConsole-v2021.3-7.0.0.599`.
## How to debug the application in DDR

The application is configured to run from DDR. It must be loaded to DDR using a previous stage program such as the HSS.

Pre-requisites: the Hart Software Services (HSS) should be built and loaded to the Icicle Kit before following the debugging steps described below.

1. Generate a HSS payload containing the mpfs-rpmsg-freertos ELF file. Please see the [HSS AMP payloads](https://github.com/polarfire-soc/polarfire-soc-documentation/blob/master/asymmetric-multiprocessing/rpmsg.md#amp-payloads) documentation section for instructions on how to generate a HSS AMP payload

2. Flash the generated payload in a non-volatile off chip memory such as eMMC or SD-card

3. On power-on, the Icicle Kit should boot the HSS and start the RPMsg Bare Metal application

4. Connect using the "mpfs-rpmsg-bm hw one-hart attach master" for debugging the RPMsg master application or "mpfs-rpmsg-bm hw one-hart attach remote" debug configuration for debugging the RPMsg remote configuration.

5. Add any breakpoints as desired

6. Press resume button on the debugger

8. The Bare Metal RPMsg example project should continue running and displaying messages over the associated UART

For more information about AMP and inter-core communication using RPMsg on PolarFire SoC, please refer to the [PolarFire SoC RPMsg documentation](https://github.com/polarfire-soc/polarfire-soc-documentation/blob/master/asymmetric-multiprocessing/rpmsg.md).

