# SDStorage Library

## Overview
/**
 * @brief Library for synchronous SD card-based storage emulation.
 * @details Emulates EEPROM-like storage using an SD card file, optimized for
 *          shutdown-time writes in home automation systems. Inherits from StorageBase,
 *          supports ESP32 and AVR platforms with efficient block-based updates.
 */

The `SDStorage` library provides a synchronous interface for emulating I2C EEPROM-like storage on an SD card, designed for reliable configuration and state storage in home automation systems. It supports shutdown-time writes with verification for data integrity, efficient updates to minimize SD card wear, and compatibility with ESP32 and AVR platforms. The library uses a 4-byte header for size metadata and leverages block operations for performance.

## Features
/**
 * @brief Key features of the SDStorage library.
 */
- Synchronous API for simple, blocking read/write operations.
- Write verification for all write operations (`writeu8`, `writeArray`, `updateArray`).
- Efficient block-based updates via `updateArray` to reduce SD card wear.
- Shutdown-time write optimization for reliable configuration saving.
- Platform support for ESP32 and AVR with appropriate buffer handling.
- Error logging for SD card operations using `Logger.h`.
- Support for 8.3 filename format with a 4-byte size header.

## Installation
/**
 * @brief Instructions for installing the SDStorage library.
 */
1. Clone or download the library:
   ```bash
   git clone https://github.com/dynapptor/SDStorage.git
   ```
2. Copy the `SDStorage` folder to your Arduino `libraries` directory.
3. Include the library in your sketch:
   ```cpp
   #include <SDStorage.h>
   ```

## Dependencies
/**
 * @brief Required dependencies for the library.
 */
- **Arduino SD Library**: For SD card file operations.
- **Arduino SPI Library**: For SD card communication.
- **StorageBase**: Base class for storage operations.
- **Logger.h**: For error and debug logging (optional, customizable).

## Usage
/**
 * @brief Example usage of the SDStorage library.
 */

### Initialization
```cpp
#include <SD.h>
#include <SDStorage.h>

SDStorage sd;

void setup() {
  if (!sd.begin(32768, "storage.bin", 4)) {
    Serial.println("SD initialization failed!");
    while (1);
  }
}
```

### Reading Configuration at Startup
```cpp
uint8_t config[100];
sd.readArray(0, config, 100); // Read 100 bytes from address 0
```

### Writing Configuration Before Shutdown
```cpp
uint8_t config[100] = {0x55}; // Example config data
if (sd.writeArray(0, config, 100)) {
  Serial.println("Config saved successfully");
} else {
  Serial.println("Config save failed");
}
```

### Updating Configuration (Web Interface)
```cpp
uint8_t newConfig[50] = {0xAA};
sd.updateArray(100, newConfig, 50); // Update only differing bytes
```

### Formatting Storage
```cpp
sd.format(0x00); // Fill storage with zeros
```

## Configuration
/**
 * @brief Configuration options for the SDStorage library.
 */
- **Storage Size**: Set via `begin`, typically 4 KB to 64 KB to emulate EEPROM sizes.
- **Filename**: 8.3 format (max 12 characters), set via `begin`.
- **Chip Select Pin**: Default pin 4, configurable via `begin`.
- **Write Buffer**: Flushes every 512 bytes or on demand via `flush`.

## Notes
/**
 * @brief Additional information and considerations.
 */
- **Address Validation**: Uses `isValidAddress` to prevent out-of-bounds access, accounting for a 4-byte header.
- **Performance**: Block-based operations (`readArray`, `writeArray`, `updateArray`) optimize performance.
- **SD Card Lifespan**: `updateArray` minimizes write cycles by updating only differing bytes.
- **Platform Considerations**:
  - **ESP32**: Supports FreeRTOS multitasking, efficient for larger buffers.
  - **AVR**: Suitable for constrained environments with smaller buffers.
- **Error Handling**: SD card errors (e.g., read/write failures) are logged for debugging.

## License
/**
 * @brief License information.
 */
MIT License. See `LICENSE` file for details.