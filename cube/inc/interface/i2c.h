#ifndef I2C_H
#define I2C_H


#include "hardware/i2c.h"

#define I2C_BUFFER_MAX 500

class I2C {
public:
    I2C(i2c_inst_t* i2c_, const uint& baudrate, const uint& sda_pin, const uint& scl_pin);
    int read(const uint& slave_addr, const uint8_t& reg_addr, uint8_t* buffer, const uint8_t size);
    int read(const uint& slave_addr, uint8_t* buffer, const uint8_t size);
    int write(const uint& slave_addr, const uint8_t& reg_addr, uint8_t* buffer, const uint8_t size);
private:
    i2c_inst_t* i2c;
};


#endif