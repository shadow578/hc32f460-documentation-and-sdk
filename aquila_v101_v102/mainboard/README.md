# Voxelab Aquila V1.0.1 / V1.0.2 Mainboard

the Aquila V1.0.1 / V1.0.2 mainboards are used in the Voxelab Aquila, Aqulia X2 and Aquila S2 3D printers.

## ICs

- `HC32F460KCTA` SoC (32-bit ARM Cortex-M4)
- `MS35775` stepper drivers (4x; TMC2208 clones)
- `24C16` I2C EEPROM (2Kb)
- `CH340G` USB-to-serial converter (Host USB Serial)
- `SGM` 6232YPS8 buck converter (probably +5V rail)
- `AMS1117-3.3` linear voltage regulator (+3.3V rail)

> for datasheets, see [datasheets](datasheets/) directory

## Connectors

### X-Stepper

connected to `MS35775` stepper driver.

```
 -  ---  -
| 1 2 3 4 |
 ---------

1: OB2 (motor coil B output 2)
2: OB1 (motor coil B output 1)
3: OA1 (motor coil A output 1)
4: OB2 (motor coil A output 2)
```

- sense resistors: 0.15 ohm from `BRA` and `BRB` to `GND`
- `MS1` configurable via resistor `R59` (GND) and `R58` (3V3); `R58` is populated with 10k
- `MS2` configurable via resistor `R57` (GND) and `R56` (3V3); `R56` is populated with 10k; -> 1/16 microstepping
- `PDN` connected to `3V3` via resistor `R62` (10k); -> no power down
- `DIR` connected to `PB9`
- `STEP` connected to `PC2`
- `EN` connected to `PC3`

> note: extrapolated from E-Stepper circuit

### Y-Stepper

connected to `MS35775` stepper driver.

```
 -  ---  -
| 1 2 3 4 |
 ---------

1: OB2 (motor coil B output 2)
2: OB1 (motor coil B output 1)
3: OA1 (motor coil A output 1)
4: OB2 (motor coil A output 2)
```

- sense resistors: 0.15 ohm from `BRA` and `BRB` to `GND`
- `MS1` configurable via resistor `R69` (GND) and `R68` (3V3); `R68` is populated with 10k
- `MS2` configurable via resistor `R66` (GND) and `R67` (3V3); `R67` is populated with 10k; -> 1/16 microstepping
- `PDN` connected to `3V3` via resistor `R72` (10k); -> no power down
- `DIR` connected to `PB7`
- `STEP` connected to `PB8`
- `EN` connected to `PC3`

> note: extrapolated from E-Stepper circuit

### Z-Stepper

connected to `MS35775` stepper driver.

```
 -  ---  -
| 1 2 3 4 |
 ---------

1: OB2 (motor coil B output 2)
2: OB1 (motor coil B output 1)
3: OA1 (motor coil A output 1)
4: OB2 (motor coil A output 2)
```

- sense resistors: 0.15 ohm from `BRA` and `BRB` to `GND`
- `MS1` configurable via resistor `R79` (GND) and `R78` (3V3); `R78` is populated with 10k
- `MS2` configurable via resistor `R76` (GND) and `R77` (3V3); `R77` is populated with 10k; -> 1/16 microstepping
- `PDN` connected to `3V3` via resistor `R82` (10k); -> no power down
- `DIR` connected to `PB5`
- `STEP` connected to `PB6`
- `EN` connected to `PC3`

> note: extrapolated from E-Stepper circuit

### E-Stepper

connected to `MS35775` stepper driver.

```
 -  ---  -
| 1 2 3 4 |
 ---------

1: OB2 (motor coil B output 2)
2: OB1 (motor coil B output 1)
3: OA1 (motor coil A output 1)
4: OB2 (motor coil A output 2)
```

- sense resistors: 0.15 ohm from `BRA` and `BRB` to `GND`
- `MS1` configurable via resistor `R5` (GND) and `R4` (3V3); `R4` is populated with 10k
- `MS2` configurable via resistor `R2` (GND) and `R3` (3V3); `R3` is populated with 10k; -> 1/16 microstepping
- `PDN` connected to `3V3` via resistor `R8` (10k); -> no power down
- `DIR` connected to `PB3`
- `STEP` connected to `PB4`
- `EN` connected to `PC3`

### Power IN

screw terminal for power input.

```
 - --- -
|(1) (2)|
 -------

1: GND
2: +24V (to fuse)
```

### Hotend Fan

screw terminal for hotend fan.

```
 - --- -
|(1) (2)|
 -------

1: +24V
2: GND
```

> note: `+24V` is always on

### Hotbed Heater

screw terminal for hotbed heater.

```
 - --- -
|(1) (2)|
 -------

1: GND via Q1
2: +24V
```

- `Q1` is switched by `PA2`

### Hotend Heater

screw terminal for hotbed heater.

```
 - --- -
|(1) (2)|
 -------

1: GND via Q2
2: +24V
```

- `Q2` is switched by `PA1`

### Part Cooling Fan

connector for 24V part cooling fan

```
 -- --
| 1 2 |
 -----

1: +24V
2: GND via Q3
```

- `Q3` is switched by `PA0`

### Motherboard Fan

connector for 24V motherboard enclosure fan

```
 -- --
| 1 2 |
 -----

1: +24V
2: GND via Q3
```

> note: motherboard fan is wired in parallel with part cooling fan

### Extruder Sensor

no information is available

> note: only present on Aquila V1.0.1 mainboard

### Filament Runout Sensor

connected to `HC32F460` SoC.

```
 - --- -
| 1 2 3 |
 -------

1: SW
2: GND
3: 3V3
```

- `SW` connected to `PA4`

> note: connection of `SW` to `PA4` could not be measured, maybe there is a resistor in-between

### X Endstop

connected to `HC32F460` SoC.

```
 -- --
| 1 2 |
 -----

1: GND
2: SW
```

- `SW` connected to `PA5`

### Y Endstop

connected to `HC32F460` SoC.

```
 -- --
| 1 2 |
 -----

1: GND
2: SW
```

- `SW` connected to `PA6`

### Z Endstop

connected to `HC32F460` SoC.

```
 -- --
| 1 2 |
 -----

1: GND
2: SW
```

- `SW` connected to `PA7`

### Hotbed Thermistor

connected to `HC32F460` SoC.

```
 -- --
| 1 2 |
 -----

1: SENSE
2: GND
```

- `SENSE` connected to `PC4`

### Hotend Thermistor

connected to `HC32F460` SoC.

```
 -- --
| 1 2 |
 -----

1: SENSE
2: GND
```

- `SENSE` connected to `PC5`

### BLTouch

BLTouch probe port, connected to `HC32F460` SoC.

```
 -- ----- --
| 1 2 3 4 5 |
 -----------

1: "G" -> GND
2: "V" -> +5V
3: "IN" -> SERVO
4: "G" -> GND
5: "OUT" -> OUTPUT
```

- `SERVO` connected to `PB0`
- `OUTPUT` connected to `PB1`

### Screen

port for screen connection, connected to `HC32F460` SoC.

```
 ------
| 1  2 |
| 3  4 |
  5  6 |
| 7  8 |
| 9 10 |
 ------
```

- `1` connected to `PC6`
- `2` connected to `PB2`
- `3` connected to `PC0`
- `4` connected to `PC1`
- `5` connected to `PB14`
- `6` connected to `PB13`
- `7` connected to `PB12`
- `8` connected to `PB15`
- `9` connected to `GND`
- `10` connected to `+5V`

> note: pin functions depend on attached screen

### SWD / ISP

the SWD pin header is connected to `HC32F460` SoC.
it can also be used for programming the SoC via ISP with the HDCS ISP tool (available in `page-dump\development tools\HUADA programmer\HDSC ISP V2.08.zip`).


```
 ---------
| 1 2 3 4 |
 ---------

1: 3V3
2: SWDIO
3: SWCLK
4: GND
```

- `SWDIO` connected to `PA13`
- `SWCLK` connected to `PA14`


> note: pins `PA13` (`SWDIO`) and `PA14` (`SWCLK`) are 5V-tolerant (see datasheet, section 2.2, table 2-4)

#### ISP Operation

ISP seems to be possible by connecting a serial port to the `SWD` header and connecting pin `PB11` (`MD`/`MODE`) to `GND`\*.

##### 1. Connect Serial Port

- `PA13` (`SWDIO`) is connected to `RX` of serial module
- `PA14` (`SWCLK`) is connected to `TX` of serial module
- `RESET` has to be connected to `RTS/DTR` of serial module\*. resistor `R15` is the pull-up resistor for the `RESET` pin, so it can be used for the connection.
- `PB11` (`MD`/`MODE`) is connected to `GND`\*\*. resistor pad `R14` near the MCU may be shorted to archive this.


> \* the ISP user manual and in-program instructions contradict here. according to section 1.2 Table 1, `RESET` doesn't have to be connected. but the in-program instructions say it has to be connected. it's possibly optional to connect the `RESET` pin, tho then you'll probably have to manually reset the MCU when programming it.

> \*\* the ISP user manual is not really clear about this. depending on the MCU, pulling it to `GND` or `VCC` are required. But i'm fairly certain that it's `GND` for the `HC32F460`. 

> note: all pins seem to be 5V-tolerant, but it's probably a good idea to use a 3.3V serial module anyway.


##### 2. ISP Tool Usage

1. extract the HDSC ISP tool (available in `page-dump\development tools\HUADA programmer\HDSC ISP V2.08.zip`)
2. optional: if you'd like to use the tool in english, open `Config.xml` and change `<EnglishLanguage>false</EnglishLanguage>` to `<EnglishLanguage>true</EnglishLanguage>
3. start the tool by executing `HDSC.exe`
  - if there is no serial port connected, the tool will show a warning message. click `OK` to continue.
4. set MCU settings
  - Target MCU: `HC32F460xExx/HC32F45xxExx`
  - COM Baud: leave at default (`1000000`), lower if needed
  - Hex File: select the hex file to flash. leave empty if you only want to read the flash.
  - COM Setting: select the serial port to use
5. MCU Flash Info (not modifiable)
  - Start Addr 0x: `00000000` (read/write from start of flash)
  - Page Size: `8192`
  - Page Count: `64`
  - FlashSize: `512K` (ok even if it does not match your MCU)
6. ??? refer to the user manual for more information from here on ???


> note: SWD and ISP have __not__ been tested


## USB Host Serial

USB host serial port is provided by `CH340G` USB-to-serial converter, which converts USB to UART.
UART is then connected to `HC32F460` SoC.

- `TX` provided on `PA9`
- `RX` provided on `PA15`

## SD Card Slot

sd card slot is connected to `HC32F460` SoC.

- `D0` connected to `PC8`
- `D1` connected to `PC9`
- `D2` connected to `PC10`
- `D3` connected to `PC11`
- `CLK` connected to `PC12`
- `CMD` connected to `PD2`
- `DETECT` connected to `PA10`

## I2C EEPROM

the I2C EEPROM is connected to `HC32F460` SoC.

- `SDA` connected to `PA11`
- `SCL` connected to `PA12`

## XTAL

the XTAL `X2` is connected to `HC32F460` SoC.

- 8MHz crystal
- `IN` connected to `PH0`
- `OUT` connected to `PH1`

## On-Board LED

there is a single on-board LED, connected to `HC32F460` SoC.
the LED is near the 'Part Cooling Fan' connector, and is marked `CPU`.

- `LED` connected to `PA3` (HIGH=OFF, LOW=ON)
