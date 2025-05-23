#include "GxIO.h"

GxIO::GxIO() {}

void GxIO::reset() {}
void GxIO::init() {}

uint8_t GxIO::transferTransaction(uint8_t d)
{
  writeDataTransaction(d);
  return readDataTransaction();
}

uint16_t GxIO::transfer16Transaction(uint16_t d)
{
  writeData16Transaction(d);
  return readData16Transaction();
}

uint8_t GxIO::readDataTransaction() { return 0; }
uint16_t GxIO::readData16Transaction() { return 0; }
uint8_t GxIO::readData() { return 0; }
uint16_t GxIO::readData16() { return 0; }
uint32_t GxIO::readRawData32(uint8_t part) { return 0; }

void GxIO::writeCommandTransaction(uint8_t c) {}
void GxIO::writeDataTransaction(uint8_t d) {}
void GxIO::writeData16Transaction(uint16_t d, uint32_t num) {}
void GxIO::writeCommand(uint8_t c) {}
void GxIO::writeData(uint8_t d) {}
void GxIO::writeData(uint8_t* d, uint32_t num) {}
void GxIO::writeData16(uint16_t d, uint32_t num) {}
void GxIO::writeAddrMSBfirst(uint16_t d) {}
void GxIO::startTransaction() {}
void GxIO::endTransaction() {}
void GxIO::setBackLight(bool lit) {}
