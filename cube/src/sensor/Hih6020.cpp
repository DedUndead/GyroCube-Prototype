#include "sensor/Hih6020.h"
#include "interface/i2c.h"

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
    // Fetch data, confirm read status
    if (i2c->read(address, data, DATA_LENGTH_BYTES) != 0) {
        return false;
    }

    // Check status of the device
    if (data[0] & STATUS_BITS != 0) {
        return false;
    }

    return 0;
}