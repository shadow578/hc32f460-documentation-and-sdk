# Voxelab Aquila V1.0.1 and V1.0.2 TMC Stepper UART Control Modification

This document describes how to modify the Aquila V1.0.2 HC32 mainboard in order to add TMC Stepper UART control lines.
Doing this allows changing stepper driver parameters (e.g. stepping mode, current) from Marlin firmware.


> [!NOTE]
> This document was written for the Aquila V1.0.2 mainboard.   
> However, since the Aquila V1.0.1 mainboard is fairly similar, it should still work.   


## Preword

Having the option to control the parameters of the stepper drivers is quite useful. 
For one, you'll be able to enable the otherwise unavailable 1/256 microstepping mode that the TMC2208 and clones provide.
Additionally, you can adjust the motor current more precisely than by using the "classic" analog pot.


However, the biggest advantage is to change the stepping mode, for example to disable StealthChop (quiet) mode.
By doing this, advanced motion features like Linear Advance and Input Shaping are said to work better.


## Hardware

On my mainboard, the following ICs were used:

- HDSC HC32F460 MCU
  - this is the main processor. If you have a different one, the information in this document could still be useful but the pins will not match.
- KalixChips MS35775 Stepper Drivers 
  - These four ICs drive the stepper motors. These ICs are close to exact clones of the TMC2208 (see the [datasheet](../datasheets/MS35775%20full.pdf) to see yourself), including the UART control mode.
  - If you have different stepper drivers, and they don't happen to be original TMC2208, do not continue with this mod until you've confirmed they support the UART control mode.
  - If your board has TMC2209 stepper drivers, consider combining this document with another one specifically for those drivers. They support connecting to a single UART line, so require less pins and less cabling. However, this exact mod should still mostly apply.


> [!IMPORTANT]
> double-check that your mainboard matches the hardware described here


## Modification

This modification will set the mainboard up in such a way that each stepper driver is connected to one pin of the MCU, allowing half-duplex software-serial to be used.
To ensure that a error while soldering or a broken stepper driver will not immediately damage the MCU, a 1k series resistor is added to each line.

> [!NOTE]
> this mod leaves the PDN_UART 10k pull-up resistor in place.  
> this way, the modified mainboard will behave as before, unless the software takes control to drive the pins.


### Requirements

- sharp hobby knife
- soldering iron with fine tip
- thin, isolated wire
- 4x 1k SMD resistor


### Pins

On this mainboard, there aren't many pins left free for use.
The free pins are (assuming you've installed a BLTouch probe):

Useable | Pin | Function / Connection | Note
-|-|-|-|
⚠️ | PA8 | CH340G DTR | connected to CH340G DTR; otherwise useable
✅ | PB2 | Screen header #2 | free for DWIN screen
✅ | PB10 | R90 |
❌ | PB11 | MD | multiplexed with MODE selection logic; input-only pin
✅ | PC6 | Screen header #1 | free for DWIN screen
✅ | PC14 | XTAL32 | can use GPIO
❌ | PC15 | XTAL32 | datasheet says this pin works as GPIO, but in my testing input function didn't work
✅ | PH2 | R101 |


I used pins PA8, PB10, PC14 and PH2 for my connections.
This is what the rest of this document will describe too


### Cutting trace PA8

Since PA8 is connected to the CH340G, this trace must be cut. 
See the following image for where to cut. 


TODO IMAGE HERE PCB MOD BACK


After cutting the trace, ensure there's no copper sticking up that could short out to the case and measure with a multimeter that the trace is actually cut completely.
To protect the area, place a bit of kapton tape over it.


### Soldering the Wires

Solder the wires as shown in the following image.


TODO IMAGE HERE PCB MOD FRONT


to ensure the wires don't come loose, secure them with a bit of hot glue.
for the PA8 connection, scrape away the solder mask on one of the wires near the HC32 before soldering.


## Configuring Marlin

Change the Marlin configuration to the following:
- Change the stepper driver type from `TMC2208_STANDALONE` to `TMC2208`
- in Configuration_adv.h, in the `@tmc/control` section, add the pin definitions:
  - `#define X_SERIAL_TX_PIN PB10`
  - `#define Y_SERIAL_TX_PIN PA8`
  - `#define Z_SERIAL_TX_PIN PC14`
  - `#define E0_SERIAL_TX_PIN PH2`
- in the same section, update the `RSENSE` value of X,Y,Z,E0 to `0.15` (this board used 150 mOhm shunt resistors)
- enable `TMC_DEBUG` until you've confirmed that everything works


Now, flash the firmware onto your modified mainboard. 
For initial testing, only the power cables have to be connected.


## Confirm everything works

to confirm the mod was sucessfull, send a `M122` command to show the connection status. 
there should be no errors in the output of the command.


if one stepper driver shows all registers as all zeros, double-check your wiring.
additionally, double-check that the used pins are actually not connected to anything else (different revisions may differ here).


## Disclaimer

Perform this modification at your own risk.
While this modification should be pretty safe, there's no gurantees.
