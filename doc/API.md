# SDStorage API Documentation

## Overview
/**
 * @brief SDStorage class for emulating EEPROM-like storage on an SD card.
 * @details Provides a synchronous interface for reading and writing data to an SD card file,
 *          optimized for shutdown-time writes in home automation systems. Inherits from StorageBase,
 *          supports ESP32 and AVR platforms with verification and efficient update operations.
 */

The `SDStorage` class emulates an I2C EEPROM-like interface using an SD card file, supporting synchronous read/write operations for configuration and state storage. It is designed for reliable data persistence, particularly for shutdown-time writes, with verification to ensure data integrity. The library supports ESP32 and AVR platforms, using a 4-byte header for size metadata and efficient block-based updates.

## Class Declaration
```cpp
/**
 * @brief SDStorage class for emulating EEPROM-like storage on an SD card.
 */
class SDStorage : public StorageBase {
 public:
  SDStorage();
  ~SDStorage();
  bool begin(size_t size, const char *filename, int pin = 4);
  uint8_t readu8(uint16_t addr);
  bool writeu8(uint16_t addr, uint8_t val);
  bool updateu8(uint16_t addr, uint8_t val);
  bool format(uint8_t v);
  uint32_t getSize();
  void flush();
  uint8_t *readArray(uint16_t addr, uint8_t *buffer, uint16_t length);
  bool writeArray(uint16_t addr, const uint8_t *buffer, uint16_t length);
  bool updateArray(uint16_t addr, const uint8_t *buffer, uint16_t length);
  bool verifyArray(uint16_t addr, const uint8_t* buffer, uint16_t length);
};
```

## Public Methods

### Constructor
```cpp
/**
 * @brief Constructs an SDStorage instance.
 */
SDStorage()
```
- **Example**:
  ```cpp
  SDStorage sd;
  ```

### Destructor
```cpp
/**
 * @brief Destructor.
 */
~SDStorage()
```

### begin
```cpp
/**
 * @brief Initializes the SD card and opens the storage file.
 * @param size Size of the emulated storage in bytes.
 * @param filename Name of the SD file (8.3 format, max 12 characters).
 * @param pin SD card chip select pin (default: 4).
 * @return true if initialization successful, false otherwise.
 */
bool begin(size_t size, const char *filename, int pin = 4)
```
- **Example**:
  ```cpp
  SDStorage sd;
  if (sd.begin(32768, "storage.bin", 4)) {
    Serial.println("SD initialized");
  }
  ```

### readu8
```cpp
/**
 * @brief Reads a single byte from the specified address.
 * @param addr Address (0 to size-1).
 * @return The byte read, or 0 if failed or address invalid.
 */
uint8_t readu8(uint16_t addr)
```
- **Example**:
  ```cpp
  uint8_t value = sd.readu8(100); // Read byte at address 100
  ```

### writeu8
```cpp
/**
 * @brief Writes a single byte to the specified address, with verification.
 * @param addr Address.
 * @param val Byte to write.
 * @return true if successful, false if failed or address invalid.
 */
bool writeu8(uint16_t addr, uint8_t val)
```
- **Example**:
  ```cpp
  if (sd.writeu8(100, 0x55)) {
    Serial.println("Write successful");
  }
  ```

### updateu8
```cpp
/**
 * @brief Updates a byte only if it differs from the current value.
 * @param addr Address.
 * @param val Byte to write.
 * @return true if successful or no write needed, false otherwise.
 */
bool updateu8(uint16_t addr, uint8_t val)
```
- **Example**:
  ```cpp
  sd.updateu8(100, 0x55); // Updates only if different
  ```

### format
```cpp
/**
 * @brief Formats the storage file with the specified value.
 * @param v Byte value to fill.
 * @return true if successful, false otherwise.
 */
bool format(uint8_t v)
```
- **Example**:
  ```cpp
  sd.format(0x00); // Fill storage with zeros
  ```

### getSize
```cpp
/**
 * @brief Returns the size of the emulated storage.
 * @return Size in bytes.
 */
uint32_t getSize()
```
- **Example**:
  ```cpp
  uint32_t size = sd.getSize(); // Returns 32768
  ```

### flush
```cpp
/**
 * @brief Flushes pending writes to the SD card.
 */
void flush()
```
- **Example**:
  ```cpp
  sd.flush(); // Ensure data is written
  ```

### readArray
```cpp
/**
 * @brief Reads an array of bytes into the provided buffer.
 * @param addr Starting address.
 * @param buffer Buffer to store data.
 * @param length Number of bytes to read.
 * @return Pointer to the buffer, or nullptr if address invalid.
 */
uint8_t *readArray(uint16_t addr, uint8_t *buffer, uint16_t length)
```
- **Example**:
  ```cpp
  uint8_t data[100];
  sd.readArray(100, data, 100); // Read 100 bytes
  ```

### writeArray
```cpp
/**
 * @brief Writes an array of bytes to the specified address, with verification.
 * @param addr Starting address.
 * @param buffer Data to write.
 * @param length Number of bytes to write.
 * @return true if successful, false if failed or address invalid.
 */
bool writeArray(uint16_t addr, const uint8_t *buffer, uint16_t length)
```
- **Example**:
  ```cpp
  uint8_t data[100] = {0x55};
  sd.writeArray(100, data, 100); // Write 100 bytes
  ```

### updateArray
```cpp
/**
 * @brief Updates an array by writing only differing bytes in continuous blocks.
 * @param addr Starting address.
 * @param buffer Data to write.
 * @param length Number of bytes to update.
 * @return true if successful, false if failed or address invalid.
 */
bool updateArray(uint16_t addr, const uint8_t *buffer, uint16_t length)
```
- **Example**:
  ```cpp
  uint8_t data[100] = {0xAA};
  sd.updateArray(100, data, 100); // Update differing bytes
  ```

### verifyArray
```cpp
/**
 * @brief Verifies that the array at the address matches the provided buffer.
 * @param addr Starting address.
 * @param buffer Data to verify against.
 * @param length Number of bytes to verify.
 * @return true if verified, false if mismatch or address invalid.
 */
bool verifyArray(uint16_t addr, const uint8_t* buffer, uint16_t length)
```
- **Example**:
  ```cpp
  uint8_t data[100] = {0x55};
  if (sd.verifyArray(100, data, 100)) {
    Serial.println("Data verified");
  }
  ```

## Notes
/**
 * @brief Additional information and considerations.
 */
- **Address Validation**: All public methods validate addresses using `isValidAddress` to prevent out-of-bounds access, accounting for a 4-byte header.
- **Write Verification**: Write operations include verification for data integrity, ideal for shutdown-time writes.
- **Efficient Updates**: `updateArray` writes only differing byte blocks, minimizing SD card wear.
- **Platform Support**: Compatible with ESP32 and AVR, with optimized buffer handling.
- **Logging**: SD card errors (e.g., read/write failures) are logged via `Logger.h`.