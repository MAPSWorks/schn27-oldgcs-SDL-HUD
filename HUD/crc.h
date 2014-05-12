#ifndef CRC_H
#define CRC_H

#include <cstdint>

uint8_t Crc8(std::uint8_t *pcBlock, int len);
uint16_t Crc16(std::uint8_t *pcBlock, int len);

uint8_t CheckSum(std::uint8_t *data, int len);

#endif
