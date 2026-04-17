Temperature sensor driver, using the STM32F103RB and DS18B20 sensor
A bare metal embedded driver for reading temperature from a DS18B20 sensor over 1-Wire protocol, with UART output to PC terminal.

1. Features
- Bare metal register-level code
- 1-Wire protocol implementation
- UART driver for serial output
- Timer-based microsecond delay functions

2. Hardware
- STM32F103RB Nucleo board
- DS18B20 temperature sensor
- 4.7kΩ pull-up resistor

3. Pin Configuration
- PA2: UART TX
- PA3: UART RX  
- PA6: 1-Wire data line (D12 on Nucleo)

4. Output
Temperature readings sent over UART at 115200 baud.
