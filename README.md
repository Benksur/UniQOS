# UniQOS

## Flash Instructions
Make sure STM32_Programmer_CLI and arm-none-eabi-gcc in PATH variables

arm-none-eabi-gcc version 14.2


## Hardware Components

- **STM32H7xx:** Microcontroller
- **RC7620:** Cellular Module
- **NAU88C22:** Audio Codec
- **DRV2603:** Vibration Motor Driver
- **LSM6DSVTR:** Motion sensor
- **WS2812:** RGB LED IC
- **BQ27441:** Fuel Gauge IC
- **MCP73871:** Battery Management


## Planned Task Mapping

| Task               | Priority |
| :----------------- | -------: |
| Cellular Task      |     High |
| Audio Task         |     High |
| Call State Task    |   Medium |
| Power Management   |   Medium |
| Watchdog Task      |   Medium |
| UI/Display Task    |  Low-mid |
| Input handler Task |  Low-mid |
| Logging Task       |      Low |

## Task Communication

- Queue from Input → UI
- Queue from UI → Call State
- Queue or event group from Call State → Cellular Task
- Queue from Cellular Task → Call State
- Event group or direct-to-task notification from Call State → Audio Codec Task


