#include "at24c01d_02d.h"

// ===============================================================
// Definitions
// ===============================================================

#define AT24C0XD_BASE 0xA0
// AT24C01D/AT24C02D allows page write of 8 bytes.
#define PAGE_WRITE_MODE_BYTES ((uint8_t) 8)
// AT24C01D/AT24C02D allows page write of 8 bytes.
// the last 3 zeros represents the 8 bytes per page (0 - 7 ~ 000 - 111)
#define PAGE_WRITE_MODE_MASK ((uint8_t) 0b11111000)

// ===============================================================
// Private functions
// ===============================================================

__STATIC_INLINE void _enable_wp(at24c01d_02d_t *ee) {
  delay(1);
  gpio_set(ee->wp_pin);
  delay(2); // wait until wp pin is stable
}

__STATIC_INLINE void _disable_wp(at24c01d_02d_t *ee) {
  gpio_reset(ee->wp_pin);
  delay(1);
}

// ===============================================================
// Public functions
// ===============================================================

// Initialize eeprom AT24C01D/AT24C02D.
//
void at24c0xd_init(at24c01d_02d_t *ee) {
  // hardware address can only be in the range [0-7]
  if (ee->hw_addr < 0 || ee->hw_addr > 7) {
    ee->hw_addr = 7;
  }
  // compute the full I2C address
  ee->_fullAddress = AT24C0XD_BASE | (ee->hw_addr << 1);
  // Initialize Write Protection (WP) pin
  gpio_mode(ee->wp_pin, OUTPUT_PP, NOPULL, SPEED_HIGH);
  // Enable WP at startup
  _enable_wp(ee);
}

// Writes a single byte in the specified memory address.
//
bool at24c0xd_write(at24c01d_02d_t *ee, uint8_t address, uint8_t data) {
  // set mem address
  ee->_i2cbuf[0] = address;
  // set data
  ee->_i2cbuf[1] = data;
  // disable write protection
  _disable_wp(ee);
  // write I2C
  bool ret = i2c_write(ee->I2Cx, ee->_fullAddress, &(ee->_i2cbuf[0]), 2, I2C_STOP);
  // some delay to avoid a instantenous new write
  delay(5);
  // enable write protection
  _enable_wp(ee);
  // return the result
  return ret;
}

// Writes multiple bytes from the specified startAddress. This function uses eeprom page write
// mode.
bool at24c0xd_writeMultiple(at24c01d_02d_t *ee, uint8_t startAddress, uint8_t *data, uint8_t nbytes) {
  bool ret = false;
  // disable write protection
  _disable_wp(ee);
  // cur_addr will track the cur address in the loop
  uint8_t cur_addr = startAddress;
  // write remaining bytes count
  uint8_t wr_remaining_cnt = nbytes;
  do {
    // the first (...) represents the page start
    uint8_t page_end = (cur_addr & PAGE_WRITE_MODE_MASK) + (PAGE_WRITE_MODE_BYTES - 1);
    // remaining bytes that can be written in current page from the current address
    uint8_t page_remaining = page_end - cur_addr + 1;
    // i2c tx bytes count
    uint8_t txb_cnt = 0;
    if (wr_remaining_cnt <= page_remaining) {
      txb_cnt = wr_remaining_cnt;
    } else {
      txb_cnt = page_remaining;
    }
    // set startAddress where we are going to start writing
    ee->_i2cbuf[0] = cur_addr;
    // calculate start_data_idx based on the total bytes (nbytes) and the write remaining
    uint8_t start_data_idx = nbytes - wr_remaining_cnt;
    // set data
    for (uint8_t i = 0; i < txb_cnt; i++) {
      ee->_i2cbuf[i + 1] = data[start_data_idx + i];
    }
    // write to I2C
    ret = i2c_write(ee->I2Cx, ee->_fullAddress, &(ee->_i2cbuf[0]), txb_cnt + 1, I2C_STOP);
    if (!ret) { break; }
    // increase cur address
    cur_addr += txb_cnt;
    // decrease remaining write count
    wr_remaining_cnt -= txb_cnt;
    // some delay to execute a new write operation
    delay(5);
  } while (wr_remaining_cnt > 0);
  // enable write protection
  _enable_wp(ee);
  // return the result
  return ret;
}

// Read the byte that is in the specified memory address.
//
uint8_t at24c0xd_read(at24c01d_02d_t *ee, uint8_t address) {
  // set address we want to read
  ee->_i2cbuf[0] = address;
  if (!i2c_write(ee->I2Cx, ee->_fullAddress, &(ee->_i2cbuf[0]), 1, I2C_NOSTOP)) {
    return 0;
  }
  // read the mem address
  if (!i2c_read(ee->I2Cx, ee->_fullAddress, &(ee->_i2cbuf[0]), 1, I2C_STOP)) {
    return 0;
  }
  // return the read byte
  return ee->_i2cbuf[0];
}

// Reads multiple bytes from the specified startAddress.
//
bool at24c0xd_readMultiple(at24c01d_02d_t *ee, uint8_t startAddress, uint8_t *rxdata, uint8_t nbytes) {
  // set the startAddress where we want to start reading
  ee->_i2cbuf[0] = startAddress;
  if (!i2c_write(ee->I2Cx, ee->_fullAddress, &(ee->_i2cbuf[0]), 1, I2C_NOSTOP)) {
    return false;
  }
  // read 'nbytes' from startAddress from I2C
  return i2c_read(ee->I2Cx, ee->_fullAddress, rxdata, nbytes, I2C_STOP);
}