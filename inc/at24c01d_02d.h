#ifndef __AT24C01D_02D_H_
#define __AT24C01D_02D_H_

#include "eonOS.h"

// ===============================================================
// Types
// ===============================================================

typedef struct {
  // I2Cx bus
  I2C_TypeDef *I2Cx;
  // Hardware address (0 - 7) depends on A2, A1, A0 pins
  uint8_t hw_addr;
  // Write protection pin
  pin_t wp_pin;
  // private variable do not modify outside
  uint8_t _fullAddress;
  // private variable do not modify outside
  uint8_t _i2cbuf[9];
} at24c01d_02d_t;

// ===============================================================
// Functions
// ===============================================================

/**
 * @brief Initialize eeprom AT24C01D/AT24C02D.
 * 
 * @param ee structure that holds eeprom AT24C01D/AT24C02D data.
 */
void at24c0xd_init( at24c01d_02d_t *ee );

/**
 * @brief Writes a single byte in the specified memory address.
 * 
 * @param ee structure that holds eeprom AT24C01D/AT24C02D data.
 * @param address the memory address you want to write.
 * @param data the data you want to write.
 * @return true if success, false if error.
 */
bool at24c0xd_write(at24c01d_02d_t *ee, uint8_t address, uint8_t data);

/**
 * @brief Writes multiple bytes from the specified startAddress. This function internally uses
 * eeprom page write mode.
 * 
 * @param ee structure that holds eeprom AT24C01D/AT24C02D data.
 * @param startAddress address from where we are going to write.
 * @param data buffer with all the bytes we want write (max. 254 bytes).
 * @param nbytes number of bytes we want to write (max. 254 bytes).
 * @return true if success, false if error.
 */
bool at24c0xd_writeMultiple(at24c01d_02d_t *ee, uint8_t startAddress, uint8_t *data, uint8_t nbytes);

/**
 * @brief Read the byte that is in the specified memory address.
 * 
 * @param ee structure that holds eeprom AT24C01D/AT24C02D data.
 * @param address the memory address you want to read.
 * @return the byte read from the specified address.
 */
uint8_t at24c0xd_read(at24c01d_02d_t *ee, uint8_t address);

/**
 * @brief Reads multiple bytes from the specified startAddress.
 * 
 * @param ee structure that holds eeprom AT24C01D/AT24C02D data.
 * @param startAddress address from where we are going to read.
 * @param rxdata buffer that will hold the received data.
 * @param nbytes number of bytes we want to read.
 * @return true if success, false if error.
 */
bool at24c0xd_readMultiple(at24c01d_02d_t *ee, uint8_t startAddress, uint8_t *rxdata, uint8_t nbytes);

#endif