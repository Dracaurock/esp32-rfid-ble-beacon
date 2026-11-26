# ESP32 RFID to BLE beacon

An ESP32 reads the UID of a 13.56 MHz RFID tag with an MFRC522 reader, then puts that UID in its Bluetooth Low
Energy advertisement so that any phone or BLE scanner nearby can see which tag was presented last. I wrote it with
PlatformIO and the Arduino framework during the second year (semester 4) of my BUT in Networks &
Telecommunications at IUT Auxerre, in spring 2025.

## How it works

Everything is in `src/main.cpp`.

At startup the ESP32 initialises the MFRC522 over SPI and prints the reader's firmware version on the serial port
(115200 baud), mounts SPIFFS, and initialises the BLE stack. It also prepares a scan response that contains one
128-bit service UUID (a test value, `efcdab90-7856-3412-efcc-ab9078563412`).

In the main loop it waits for a tag. When one is read:

1. The UID bytes are turned into a lowercase hex string, two characters per byte (`a1b2c3d4` for a 4-byte UID).
2. The UID is added to a list in RAM if it is not already there, and the whole list is printed on the serial port.
3. A new advertisement is built and advertising is (re)started.

The advertisement payload is written byte by byte. For a 4-byte UID it looks like this:

| Bytes | Meaning |
|---|---|
| `05 09 52 46 49 44` | Complete local name, `"RFID"` |
| `03 FF E5 02` | Manufacturer specific data with company ID `0x02E5` (Espressif) |
| `02 FF 45` or `02 FF 50` | A second manufacturer data field with one byte: `0x45` (`'E'`) if the UID matches the one hard-coded in the sketch, `0x50` (`'P'`) for any other tag |
| `61 31 62 32 ...` | The UID string itself, in ASCII |

So a receiver can tell from one byte whether the known tag or another one was presented, and read the full UID
from the end of the packet.

The end of `main.cpp` keeps two earlier sketches, commented out: a passive BLE scanner and an active one that
looks for one device by its MAC address and prints its service UUIDs. They are not compiled, and can be flashed on
a second ESP32 to watch the beacon from the receiving side.

## Hardware

- ESP32 DevKit (ESP32-WROOM-32, PlatformIO board `esp32dev`)
- MFRC522 RFID reader module (13.56 MHz, MIFARE-type tags)

### Wiring

Only the chip select pin is set in the code (`MFRC522DriverPinSimple ss_pin(5)`). The clock and data lines use the
default `SPI` bus of the Arduino-ESP32 core, which is VSPI on the classic ESP32. The pins below for SCK, MISO and
MOSI are the default ones of that bus on an ESP32 DevKit, not values written in this project.

| MFRC522 pin | ESP32 pin | Note |
|---|---|---|
| SDA (SS) | GPIO5 | set in the code |
| SCK | GPIO18 | default VSPI pin |
| MOSI | GPIO23 | default VSPI pin |
| MISO | GPIO19 | default VSPI pin |
| RST | 3.3 V | the code uses a software reset and never drives this pin; it must stay high |
| IRQ | not connected | not used |
| GND | GND | |
| 3.3V | 3.3V | power the module from 3.3 V, not 5 V |

## Software

- [PlatformIO](https://platformio.org/) (VS Code extension or the `pio` command line)
- Libraries, downloaded automatically by PlatformIO from `platformio.ini`:
  - [Arduino_MFRC522v2](https://github.com/OSSLibraries/Arduino_MFRC522v2) (LGPL-2.1)
  - [ArduinoJson](https://arduinojson.org/) by Benoit Blanchon, v7 (MIT)
- The BLE and SPIFFS libraries come with the Arduino-ESP32 core.

## Build and upload

```
pio run                      # build
pio run -t upload            # flash the ESP32 over USB
pio device monitor           # serial monitor at 115200 baud
```

The PlatformIO environment is called `esp32doit-devkit-v1` but uses the generic `esp32dev` board definition.

Before flashing, replace the placeholder UID `a1b2c3d4` in `src/main.cpp` with the UID of your own tag (it is
printed on the serial monitor the first time you present the tag).

To check the result, open a BLE scanner app on a phone (nRF Connect, for example), look for a device named `RFID`
and read its raw advertising data.

## Known limitations

- The UID list only lives in RAM and is lost at reset. SPIFFS is mounted (and formatted if mounting fails) and
  ArduinoJson is included, but the function that saved the UIDs to `/uids.json` is commented out. As written, it
  would also replace the stored array on each call instead of appending to it.
- The UID string is appended to the payload without its own length and type header, so standard scanners show it as
  a malformed field. The receiver has to parse the raw bytes.
- The second manufacturer data field holds a single byte and no company ID, which does not follow the Bluetooth
  format for that field.
- A legacy BLE advertisement is limited to 31 bytes. 4-byte and 7-byte UIDs fit; a 10-byte UID would not.
- Advertising starts only after the first tag is read, and the last UID keeps being advertised until another tag
  is presented.
- The code uses the `std::string` version of the BLE API, which is the one in Arduino-ESP32 core 2.x. Core 3.x
  switched to Arduino `String`, so the PlatformIO platform version may need to be pinned for the code to build.

## Files

```
platformio.ini   PlatformIO environment and library dependencies
src/main.cpp     the whole program
include/         empty (PlatformIO template)
```

## Author

Antoine Blondeau, now an embedded systems engineering student at ENSTA.
