# i²C

Possible address conflict betwen ADS1115 

## ADS1115

- http://www.ti.com/lit/ds/symlink/ads1115.pdf

- Fast-Mode: max. 0.4MHz
- High-Speed-Mode: max. 3.4MHz

| Addr Pin | Addr |
| --- | ---: |
| GND | 1001000 = 0x48 |
| VDD | 1001001 = 0x49 |
| SDA | 1001010 = 0x4A |
| SCL | 1001011 = 0x4B |

## OLED (SSD1306)

- https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
- Address: 0x3c
- Max speed: Clock Cycle Time min. 2.5µs = 0,4MHz
- But works with 2MHz...

| Addr |
| :---: |
| 1001000 = 0x48 |

## MPU9250

- https://invensense.tdk.com/wp-content/uploads/2015/02/PS-MPU-9250A-01-v1.1.pdf
- Fast-Mode: max. 0,4MHz (SPI: max 20MHZ)

| AD0 | Addr |
| --- | ---: |
| 0 | 1101000 = 0x68 |
| 1 | 1101001 = 0x69 |

## INA219

- http://www.ti.com/lit/ds/symlink/ina219.pdf
- Fast-Mode: max. 0.4MHz
- High-Speed-Mode: max. 2.56MHz
- Register contents are updated 4 μs after completion of the write command. Therefore, a 4-μs delay is required
between completion of a write to a given register and a subsequent read of that register (without changing the
pointer) when using SCL frequencies in excess of 1 MHz.

| A1 | A0 | Addr |
| --- | --- | --- |
| GND | GND | 1000000 = 0x40|
| GND | VS+ | 1000001 = 0x41 |
| GND | SDA | 1000010 = 0x42 |
| GND | SCL | 1000011 = 0x43 |
| VS+ | GND | 1000100 = 0x44 |
| VS+ | VS+ | 1000101 = 0x45 |
| VS+ | SDA | 1000110 = 0x46 |
| VS+ | SCL | 1000111 = 0x47 |
| SDA | GND | 1001000 = 0x48 |
| SDA | VS+ | 1001001 = 0x49 |
| SDA | SDA | 1001010 = 0x4a |
| SDA | SCL | 1001011 = 0x4b |
| SCL | GND | 1001100 = 0x4c |
| SCL | VS+ | 1001101 = 0x4d |
| SCL | SDA | 1001110 = 0x4e |
| SCL | SCL | 1001111 = 0x4f |

## DS3231

- https://datasheets.maximintegrated.com/en/ds/DS3231.pdf
- Standard-Mode: max. 0.1Mhz
- Fast-Mode: max. 0.4MHz

| Addr |
| :---: |
| 1101000 = 0x68 |
