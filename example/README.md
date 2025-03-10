# nPM2100 bare-metal drivers example.

This example demonstrates the essential features of the nPM2100 PMIC.
It is intended for evaluation of nPM2100 using an nPM2100 EK and getting familiar with how to integrate and use the nPM2100 bare-metal drivers in your project.
To run this example you need an nRF52840 DK and an nPM2100 EK connected as described in [Wiring](#Wiring)

### Getting started

This example is designed for use with nRF5 SDK.
To start with it you need to get nRF5 SDK source first. On Linux you can run

```bash
./setup.sh
```

from the directory containing this README.
This will download the latest version of nRF5 SDK and unzip it into `nRF5_SDK` directory for you.
Alternatively, download and extract it yourself by following [this link](https://nsscprodmedia.blob.core.windows.net/prod/software-and-other-downloads/sdks/nrf5/binaries/nrf5_sdk_17.1.0_ddde560.zip).

You will also need an Arm GNU Toolchain (**arm-none-eabi** variant).
This can be downloaded from the [official Arm website](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads).
Older versions of the toolchain can be found under [deprecated GNU Arm Embedded Toolchain Downloads](https://developer.arm.com/downloads/-/gnu-rm).
Make sure to use the same version as described in the nRF5 SDK release notes.

After installing the toolchain you might need to update its path and version in the Makefile (`GNU_INSTALL_ROOT`, `GNU_VERSION`)

### Wiring

With this configuration, the nPM2100 EK is wired to supply power to the nRF52840 DK.
This ensures that the TWI communication is at compatible voltage levels, and represents a realistic use case for the nPM2100 PMIC.

> ***NOTE:***
   To prevent leakage currents and program the DK, do not remove the USB connection.
   </br></br>
   Unplug the battery from the nPM2100 EK and set the DK power switch to "OFF" while applying the wiring.
   If you have issues communicating with the DK or programming it after applying the wiring, try to power cycle the DK and EK.

To connect your DK to the nPM2100 EK, complete the following steps:

1. Prepare the DK for being powered by the nPM2100 EK:

   * Set switch **SW9** ("nRF power source") to position "VDD".
   * Set switch **SW10** ("VEXT -> VnRF") to position "ON".

2. Connect the TWI interface and power supply between the DK and the nPM2100 EK as described in the following table:

    | nRF52840 DK pin       | nPM2100 EK pin |
    | ---------------       | -------------- |
    | P0.26                 | SDA            |
    | P0.27                 | SCL            |
    | P1.12                 | GPIO1          |
    | P21 External supply + | VOUT           |
    | GND                   | GND            |

3. Make the following connections on the nPM2100 EK:

   * Remove the USB power supply from the **J4** connector.
   * On the **P6** pin header, connect pins 1 and 2 with a jumper.
   * On the **BOOTMON** pin header, select **OFF** with a jumper.
   * On the **VSET** pin header, select **3.0V** with a jumper.
   * On the **VBAT SEL** switch, select **VBAT** position.
   * Connect a battery board to the **BATTERY INPUT** connector.

### Building and flashing

```bash
make
make flash
```

> ***NOTE:***
   For flashing the built FW you need to have nrfjprog available in your PATH

### Testing

After the board is flashed and reset, you will see the following message on the board's serial
interface if the initialization was successful:

```shell
<info> app: nPM2100 example started
<info> app: nPM2100 PMIC device OK
```

The example FW also prints ADC measurements every 2 seconds using the nPM2100's timer with expiry interrupts on GPIO1:

```shell
<info> app: Vbat: 1.112 V, Vout: 3.071 V, Die temp: 22.740°C
```

nPM2100's LDO is off by default but is set up to switch to HP (High Power) mode when **GPIO0** is active.
To enable LDO press the **GPIO0** button on the EK.
