#ifndef HIH6020_H
#define HIH6020_H


#include "interface/i2c.h"

#define DEFAULT_ADDRESS   0x27
#define STATUS_BITS       0xC0
#define MASK_STATUS       0x3F
#define DATA_LENGTH_BYTES 4
#define HIH_ERROR_STATUS  0x65
#define SCALING_FACTOR    (0x4000 - 2)
#define HUMIDITY_MAX      100
#define TEMPERATURE_MAX   125
#define TEMPERATURE_MIN   -40

class Hih6020 {
public:
    Hih6020(I2C* i2c_, uint address_ = DEFAULT_ADDRESS);
    int read_humidity(bool fetch = true);
    int read_temperature(bool fetch = true);
private:
    bool fetch_data();
    int convert_humidity(const uint16_t& humidity);
    int convert_temperature(const uint16_t& temperature);

    I2C* i2c;
    uint8_t address;
    uint8_t data[DATA_LENGTH_BYTES];
};


#endif