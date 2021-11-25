#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "interface/i2c.h"
#include "stdio.h"

/* @brief I2C wrapper class
 * @param i2c      Pointer to hardware, i2c0/i2c1
 * @param baudrate Bus frequency requested
 * @param spa_pin  Target SDA pin
 * @param scl_pin  Target SCL pin
 */
I2C::I2C(i2c_inst_t* i2c_, const uint& baudrate, const uint& sda_pin, const uint& scl_pin) :
    i2c(i2c_)
{
    // Initialize hardware
    i2c_init(i2c, baudrate);
 
    // Initialize pins
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);

    // Enable pullup
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin); 
}

/* @brief Read data from specified slave's register
 * @param slave_addr Slave address
 * @param reg_addr   Register address
 * @param buffer     Buffer to read to
 * @param size       Size of buffer
 * @return           Number of bytes read, negative on error
 */
int I2C::read(uint8_t slave_addr, uint8_t reg_addr, uint8_t* buffer, size_t size)
{
    // Establish connection with slave, keep connection opened
    if (i2c_write_blocking(i2c, slave_addr, &reg_addr, 1, true) != 1) {
        return -1;
    }

    return i2c_read_blocking(i2c, slave_addr, buffer, size, false);
}

/* @brief Read data from specified slave's
 * Use for reading without specifyng register
 * @param slave_addr Slave address
 * @param buffer     Buffer to read to
 * @param size       Size of buffer
 * @return           Number of bytes read, negative on error
 */
int I2C::read(uint8_t slave_addr, uint8_t* buffer, size_t size)
{
    return i2c_read_blocking(i2c, slave_addr, buffer, size, false);
}

/* @brief Write data to specified slave's register
 * @param slave_addr Slave address
 * @param reg_addr   Register address
 * @param buffer     Buffer to write from
 * @param size       Size of buffer
 * @return           Number of bytes read, negative on error
 */
int I2C::write(uint8_t slave_addr, uint8_t reg_addr, uint8_t* buffer, size_t size)
{
    uint8_t tx_buffer[I2C_BUFFER_MAX];

    // Construct transmit buffer
    tx_buffer[0] = reg_addr;
    for (int i = 0; i < size; i++) {
        tx_buffer[i + 1] = buffer[i];
    }

    // Establish connection with slave, keep connection opened
    return i2c_write_blocking(i2c, slave_addr, tx_buffer, size + 1, false);
}

int I2C::wake(uint8_t slave_addr)
{
    uint8_t wd = 0;
    return i2c_write_blocking(i2c, slave_addr, &wd, 1, false);
}