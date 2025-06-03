#include "SDStorage.h"

#define FILE_HEADER_SIZE 4

SDStorage::SDStorage() {}

SDStorage::~SDStorage() {}

bool SDStorage::_seek(uint32_t addr) {
  if (!_ee.seek(addr + FILE_HEADER_SIZE)) {
    logger.error(F("seek failed to address: %i"), addr);
    return false;
  }
  return true;
}

bool SDStorage::begin(size_t size, const char *filename, int pin) {
  if (SD.begin(pin)) {
    logger.debug(F("SD begin success"));
    return open(size, filename);
  } else {
    logger.error(F("SD begin failed"));
    return false;
  }
}

bool SDStorage::open(uint32_t size, const char *filename) {
  if (strlen(filename) > 12) {
    logger.error(F("file '%s' name is too long, max 12 character allowed"), filename);
    return false;
  }
  strcpy(_filename, filename);
  uint8_t ret = 0;
  _size = size;
  if (!SD.exists(_filename)) {
    logger.debug(F("file '%s' does not exists, create and format it..."), _filename);
    ret = format('\0');
    if (ret) {
      logger.debug(F("file '%s' formatted successfully!"), _filename);
      return ret;
    }
  } else {
    _ee = SD.open(_filename, O_RDWR);
    if (!_ee) return false;
    uint32_t s;
    _ee.seek(0);
    uint8_t s32[4];
    if (_ee.read(s32, 4) == 4) {
      memcpy(&s, s32, sizeof(s));
      if (s != _size) {
        logger.debug(F("reformatting '%s' file to size %i ..."), _filename, size);
        ret = format('\0');
        if (ret) {
          logger.debug(F("file '%s' formatted successfully!"), _filename);
          return ret;
        }
      }
    } else {
      logger.error(F("Read error: addr=0 length=4 !"));
      return false;
    }
  }
  logger.debug(F("file '%s' opened successfully!"), _filename);
  return 1;
}

void SDStorage::close() {
  _ee.flush();
  _ee.close();
}

bool SDStorage::format(uint8_t v) {
  if (_ee) _ee.close();
  if (SD.exists(_filename)) {
    SD.remove(_filename);
  }
  _ee = SD.open(_filename, O_WRITE | O_CREAT);
  if (_ee) {
    size_t a;
    a = (_size < 512) ? _size : 512;
    uint8_t val[a];
    for (size_t i = 0; i < a; i++) {
      val[i] = v;
    }
    for (size_t i = 0; i < _size - a; i += a) {
      _ee.write(val, a);
      _ee.flush();
    }
    _ee.close();
    _ee = SD.open(_filename, O_RDWR);
    _ee.seek(0);
    uint8_t s[4];
    memcpy(s, &_size, sizeof(_size));
    _ee.write(s, sizeof(_size));
    flush();
    return true;
  } else {
    return false;
  }
}

uint32_t SDStorage::getSize() {
  return _size;
}

void SDStorage::flush() {
  _ee.flush();
  _update = 0;
}

uint8_t SDStorage::readu8(uint16_t addr) {
  if (!isValidAddress(addr + FILE_HEADER_SIZE)) return 0;
  if (_seek(addr)) {
    return _ee.read();
  } else {
    return 0;
  }
}
bool SDStorage::writeu8(uint16_t addr, uint8_t val) {
  if (!isValidAddress(addr + FILE_HEADER_SIZE)) return false;
  _update++;
  if (!_seek(addr)) return false;
  if (_ee.write(val) != 1) return false;
  if (_update == 512) {
    flush();
  }
  return (readu8(addr) == val);
}

bool SDStorage::updateu8(uint16_t addr, uint8_t val) {
  if (!isValidAddress(addr + FILE_HEADER_SIZE)) return false;
  if (readu8(addr) == val) return true;
  writeu8(addr, val);
  return (readu8(addr) == val);
}

uint8_t *SDStorage::readArray(uint16_t addr, uint8_t *buffer, uint16_t length) {
  if (!isValidAddress(addr + FILE_HEADER_SIZE, length)) return nullptr;
  _seek(addr);
  if (_ee.read(buffer, length) != (int)length) {
    logger.error(F("Read error: addr=%d length=%d"), addr, length);
  }
  return buffer;
}

bool SDStorage::writeArray(uint16_t addr, const uint8_t *buffer, uint16_t length) {
  if (!isValidAddress(addr + FILE_HEADER_SIZE, length)) return false;
  if ((_update + length) >= 512) {
    flush();
  }
  if (!_seek(addr)) return false;
  if (_ee.write(buffer, length) != length) return false;
  _update += length;
  if (_update >= 512) {
    flush();
  }
  return verifyArray(addr, buffer, length);
}

bool SDStorage::updateArray(uint16_t addr, const uint8_t *buffer, uint16_t length) {
  if (!isValidAddress(addr + FILE_HEADER_SIZE, length)) return false;

  uint8_t current[length];
  readArray(addr, current, length);

  uint16_t start = 0;
  bool in_diff = false;
  for (uint16_t i = 0; i < length; i++) {
    if (current[i] != buffer[i]) {
      if (!in_diff) {
        start = i;
        in_diff = true;
      }
    } else if (in_diff) {
      if (!writeArray(addr + start, buffer + start, i - start)) {
        logger.error(F("Write error: addr=%d, length=%d"), addr + start, i - start);
        return false;
      }
      in_diff = false;
    }
  }

  if (in_diff) {
    if (!writeArray(addr + start, buffer + start, length - start)) {
      logger.error(F("Write error: addr=%d, length=%d"), addr + start, length - start);
      return false;
    }
  }

  return verifyArray(addr, buffer, length);
}

bool SDStorage::verifyArray(uint16_t addr, const uint8_t *buffer, uint16_t length) {
  if (!isValidAddress(addr + FILE_HEADER_SIZE, length)) return false;
  uint8_t read_buffer[length];
  if (!readArray(addr, read_buffer, length)) {
    logger.error(F("Read error: addr=%d, length=%d"), addr, length);
    return false;
  }
  if (memcmp(read_buffer, buffer, length) != 0) {
    return false;
  }

  return true;
}