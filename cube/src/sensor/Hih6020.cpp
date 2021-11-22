#include "sensor/Hih6020.h"

Hih6020::Hih6020(I2C* i2c_, uint address_) : 
    i2c(i2c_),
    address(address_)
{ /* Empty constructor body */ }

int Hih6020::read_humidity()
{
    return 0;
}

int Hih6020::read_temperature()
{
    return 0;
}

bool Hih6020::fetch_data()
{
    return 0;
}