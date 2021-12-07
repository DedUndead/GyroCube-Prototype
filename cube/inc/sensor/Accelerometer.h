#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H


#include "pico/stdio.h"
#include "interface/i2c.h"

#define DEF_ADDRESS      0x1E
#define CTRL_REG1        0x2A
#define OUT_X_MSB        0x01

#define XYZ_BUFFER       3
#define XYZ_BUFFER_FULL  6
#define ACC_ERROR_STATUS 0xEFFF

typedef struct acc_measurements {
    int x;
    int y;
    int z;
} acc_measurements;

/**
 * @brief Abstraction of accelerometer class
 * Provides interface to read/config values on chip
 */
class Accelerometer {
public:
    Accelerometer(I2C* i2c_, uint8_t address_ = DEF_ADDRESS);
    int enable();
    int read(acc_measurements& buffer);
private:
    int fetch_data();

    bool fast_mode;                       // Flag determines fast mode operation
    uint8_t buffer_short[XYZ_BUFFER];     // Short buffer for fast reading
    uint8_t buffer_full[XYZ_BUFFER_FULL]; // Full buffer for full reading

    uint8_t address;
    I2C* i2c;
};


#endif