#include "interface/XBee.h"

/**
 * @brief XBee UART interface
 * @param uart          pointer to uart instance
 * @param txpin         uart TX pin
 * @param rxpin         uart RX pin
 * @param ctspin        uart CTS pin
 * @param wokepin       pin to wakeup the device
 * @param statuspin     pin to check the sleep status of the device
 */
XBee::XBee(uart_inst_t* uart_, uint8_t txpin, uint8_t rxpin, uint8_t ctspin, uint8_t wokepin_, uint8_t statuspin_) :
    uart(uart_),
    wokepin(wokepin_),
    statuspin(statuspin_)
{
    uart_init(uart, 9600);
    uart_set_hw_flow(uart, true, false);
    gpio_set_function(txpin, GPIO_FUNC_UART);
    gpio_set_function(rxpin, GPIO_FUNC_UART);
    gpio_set_function(ctspin, GPIO_FUNC_UART);
    
    gpio_init(wokepin);
    gpio_set_dir(wokepin, GPIO_OUT);
    gpio_init(statuspin);
    gpio_set_dir(statuspin, GPIO_IN);
    gpio_put(wokepin, true);
}

/**
 * @brief Send a string to the uart
 * Wake up the device if needed
 * @param s string to be sent
 * @param len Length of the tx buffer
 */
void XBee::send_data(const uint8_t *s, size_t len){
    if (check_status()) {
        uart_write_blocking(uart, s, len);
    }
    else {
        wake_up();
        uart_write_blocking(uart, s, len);
    }
}

/**
 * @brief Send a character to the uart
 * Wake up the device if needed
 * @param c character to be sent
 */
void XBee::send_data(char c)
{
    if (check_status()) {
        uart_putc(uart, c);
    }
    else {
        wake_up();
        uart_putc(uart, c);
    }
}

/**
 * @brief Read data from uart to local buffer
 * @param buffer Pointer to buffer
 * @param len    Length of buffer
 * @return       True if buffer filled, false otherwise
 */
bool XBee::get_data(uint8_t* buffer, size_t len)
{
    if (uart_is_readable(uart)) {
        uart_read_blocking(uart, buffer, len);
        return true;
    }

    return false;
}

/**
 * @brief Wake up the device by driving pin high->low
 */
void XBee::wake_up()
{
    gpio_put(wokepin, false);
    sleep_ms(1);
    gpio_put(wokepin, true);
}

/**
 * @brief Check if device is sleeping
 * @return bool 
 */
bool XBee::check_status()
{
    return gpio_get(statuspin);
}