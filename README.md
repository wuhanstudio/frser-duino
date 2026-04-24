# Arduino Serprog

> This firmware implements a Serial Flasher Protocol v1 using an Arduino framework.

```
flashrom.exe -p serprog:dev=COM7:115200 -V
```

| Signal   | Arduino Uno Pin | SPI Flash Pin | Notes                    |
| -------- | --------------- | ------------- | ------------------------ |
| **MOSI** | D11             | DI / SI       | Master Out → Slave In    |
| **MISO** | D12             | DO / SO       | Master In ← Slave Out    |
| **SCK**  | D13             | CLK / SCLK    | SPI clock                |
| **CS**   | D10             | CS           | Chip select (active LOW) |
| **GND**  | GND             | GND           | Ground reference         |
| **VCC**  | 3.3V ⚠️         | VCC           | Must match flash voltage |

| Signal   | STM32F103C8 Pin | SPI Flash Pin | Notes                    |
| -------- | --------------- | ------------- | ------------------------ |
| **MOSI** | PA7             | DI / SI       | Master Out → Slave In    |
| **MISO** | PA6             | DO / SO       | Master In ← Slave Out    |
| **SCK**  | PA5             | CLK / SCLK    | SPI clock                |
| **CS**   | PA4             | CS            | Chip select (active LOW) |
| **GND**  | GND             | GND           | Ground reference         |
| **VCC**  | 3.3V ⚠️         | VCC           | Must match flash voltage |

| Signal   | ESP32 Pin | SPI Flash Pin | Notes                    |
| -------- | --------- | ------------- | ------------------------ |
| **MOSI** | GPIO 23   | DI / SI       | Master Out → Slave In    |
| **MISO** | GPIO 19   | DO / SO       | Master In ← Slave Out    |
| **SCK**  | GPIO 18   | CLK / SCLK    | SPI clock                |
| **CS**   | GPIO 5    | CS            | Chip select (active LOW) |
| **GND**  | GND       | GND           | Ground reference         |
| **VCC**  | 3.3V ⚠️   | VCC           | Must match flash voltage |

## References

- Serial Flasher Protocol v1: https://www.flashrom.org/supported_hw/supported_prog/serprog/serprog-protocol.html
- Original implementation only supports AVR devices: https://github.com/urjaman/frser-duino
