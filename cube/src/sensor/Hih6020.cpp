#include "sensor/Hih6020.h"
#include "interface/i2c.h"
#include "stdio.h"

Hih6020::Hih6020(I2C* i2c_, uint address_) : 
    i2c(i2c_),
    address(address_)
{ /* Empty constructor body */ }

/**
 * @brief Read humidity from the sensor
 * @return Error code on error, reading on success 
 */
int Hih6020::read_humidity()
{
    // Check if data can be obtained
    if (!fetch_data()) return HIH_ERROR_STATUS;

    // According to honeywell data organization
    uint16_t raw_reading = ((data[0] & MASK_STATUS) << 8) | data[1];

    return convert_humidity(raw_reading);
}

/**
 * @brief Read temperature from the sensor
 * @return Error code on error, reading on success  
 */
int Hih6020::read_temperature()
{
    // Check if data can be obtained and update data buffer
    if (!fetch_data()) return HIH_ERROR_STATUS;

    // According to honeywell data organization
    uint16_t raw_reading = (data[2] << 6) | (data[3] >> 2);

    return convert_temperature(raw_reading);
}

/**
 * @brief Fetch temperature and humidity data from the sensor
 * @return True on success
 */
bool Hih6020::fetch_data()
{
    // Fetch data, confirm read status
    if (i2c->wake(address) <= 0) {
        return false;
    }

    sleep_ms(40); // Wait for the measurement

    if (i2c->read(address, data, DATA_LENGTH_BYTES) <= 0) {
        return false;
    }

    // Check status of the device
    if ((data[0] & STATUS_BITS) != 0) {
        return false;
    }

    return true;
}

/**
 * @brief Transfer function for humidity reading
 * @param humidity Raw reading
 * @return         Actual humidity value
 */
int Hih6020::convert_humidity(const uint16_t& humidity)
{
    return humidity / (float) SCALING_FACTOR * HUMIDITY_MAX;
}

/**
 * @brief Transfer function for temperature reading
 * @param temperature Raw reading
 * @return            Actual temperature value
 */
int Hih6020::convert_temperature(const uint16_t& temperature)
{
    return temperature / (float) SCALING_FACTOR * (TEMPERATURE_MAX - TEMPERATURE_MIN) + TEMPERATURE_MIN;
}