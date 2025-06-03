/**
 * @file SDStorage.h
 * @brief Header file for the SDStorage class, providing SD card-based storage emulation.
 * @author Ferenc Mayer
 * @date 2025-06-02
 */

#pragma once
/**
 * @brief Prevents multiple inclusions of the header file.
 */

#include <Logger.h>
/**
 * @brief Includes Logger for error and debug logging.
 */

#include <SD.h>
/**
 * @brief Includes SD library for SD card file operations.
 */

#include <SPI.h>
/**
 * @brief Includes SPI library for SD card communication.
 */

#include <StorageBase.h>
/**
 * @brief Includes StorageBase for the base storage interface.
 */

/**
 * @brief SDStorage class for emulating EEPROM-like storage on an SD card.
 * @details Inherits from StorageBase, provides synchronous read/write operations
 *          using an SD card file, optimized for shutdown-time writes in home
 *          automation systems. Supports verification and efficient updates.
 */
class SDStorage : public StorageBase {
 private:
  /**
   * @brief Seeks to the specified address in the file, accounting for the header.
   * @param addr Address to seek (0 to size-1).
   * @return true if seek successful, false otherwise.
   */
  bool _seek(uint32_t addr);

 protected:
  uint32_t _size;        ///< Size of the emulated storage in bytes.
  char _filename[13];    ///< Filename for the SD storage file (max 8.3 format, 12 chars + null).
  File _ee;              ///< File object for SD card operations.
  uint32_t _update = 0;  ///< Counter for bytes written since last flush.

  /**
   * @brief Opens the SD file with the specified size and filename.
   * @param size Size of the emulated storage.
   * @param filename Name of the SD file (8.3 format, max 12 characters).
   * @return true if opened successfully, false otherwise.
   */
  bool open(uint32_t size, const char *filename);

  /**
   * @brief Closes the SD file.
   */
  void close();

  /**
   * @brief Reads an array of bytes into the provided buffer.
   * @param addr Starting address (0 to size-1).
   * @param buffer Buffer to store data.
   * @param length Number of bytes to read.
   * @return Pointer to the buffer, or nullptr if address invalid.
   */
  uint8_t *readArray(uint16_t addr, uint8_t *buffer, uint16_t length) override;

  /**
   * @brief Writes an array of bytes to the specified address.
   * @param addr Starting address.
   * @param buffer Data to write.
   * @param length Number of bytes to write.
   * @return true if successful, false otherwise.
   */
  bool writeArray(uint16_t addr, const uint8_t *buffer, uint16_t length) override;

  /**
   * @brief Updates an array by writing only differing bytes.
   * @param addr Starting address.
   * @param buffer Data to write.
   * @param length Number of bytes to update.
   * @return true if successful, false otherwise.
   */
  bool updateArray(uint16_t addr, const uint8_t *buffer, uint16_t length) override;

 public:
  /**
   * @brief Constructs an SDStorage instance.
   */
  SDStorage();

  /**
   * @brief Destructor.
   */
  ~SDStorage();

  /**
   * @brief Initializes the SD card and opens the storage file.
   * @param size Size of the emulated storage in bytes.
   * @param filename Name of the SD file (8.3 format, max 12 characters).
   * @param pin SD card chip select pin (default: 4).
   * @return true if initialization successful, false otherwise.
   */
  bool begin(size_t size, const char *filename, int pin = 4);

  /**
   * @brief Reads a single byte from the specified address.
   * @param addr Address (0 to size-1).
   * @return The byte read, or 0 if failed or address invalid.
   */
  uint8_t readu8(uint16_t addr) override;

  /**
   * @brief Writes a single byte to the specified address.
   * @param addr Address.
   * @param val Byte to write.
   * @return true if successful, false if failed or address invalid.
   */
  bool writeu8(uint16_t addr, uint8_t val) override;

  /**
   * @brief Updates a byte only if it differs from the current value.
   * @param addr Address.
   * @param val Byte to write.
   * @return true if successful or no write needed, false otherwise.
   */
  bool updateu8(uint16_t addr, uint8_t val) override;

  /**
   * @brief Formats the storage file with the specified value.
   * @param v Byte value to fill.
   * @return true if successful, false otherwise.
   */
  bool format(uint8_t v) override;

  /**
   * @brief Returns the size of the emulated storage.
   * @return Size in bytes.
   */
  uint32_t getSize() override;

  /**
   * @brief Flushes pending writes to the SD card.
   */
  void flush() override;

  /**
   * @brief Verifies that the array at the specified address matches the provided buffer.
   * @param addr Starting address.
   * @param buffer Data to verify against.
   * @param length Number of bytes to verify.
   * @return true if verified, false if mismatch or address invalid.
   */
  bool verifyArray(uint16_t addr, const uint8_t *buffer, uint16_t length);
};
