#ifndef HIH6020_H
#define HIH6020_H


#include "interface/i2c.h"

#define DEFAULT_ADDRESS   0x27
#define STATUS_BITS       0xC0
#define DATA_LENGTH_BYTES 4

class Hih6020 {
public:
    Hih6020(I2C* i2c_, uint address_ = DEFAULT_ADDRESS);
    int read_humidity();
    int read_temperature();
private:
    bool fetch_data();

    I2C* i2c;
    uint address;
    uint8_t data[DATA_LENGTH_BYTES];
};


#endif