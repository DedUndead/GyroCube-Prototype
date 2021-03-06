#include "interface/XBee.h"
#include <stdio.h>

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
void XBee::send_data(const char *s){
    if (!is_active()) wake_up();
    uart_puts(uart, s);
}

/**
 * @brief Send a character to the uart
 * Wake up the device if needed
 * @param c character to be sent
 */
void XBee::send_data(char c)
{
    if (!is_active()) wake_up();
    uart_putc(uart, c);
}

/**
 * @brief Read data from uart to local buffer
 * @param buffer Pointer to buffer
 * @param len    Maximum length of buffer
 * @return       Number of characters received
 */
int XBee::get_data(uint8_t* buffer, size_t len)
{
    int num = 0;
    int timer = 0;
    
    while (num < len) {
        while (!uart_is_readable(uart)) {
            if (timer == TIMEOUT) return num;
            timer++;
            sleep_ms(1);
        }
        buffer[num] = uart_getc(uart);

        timer = 0;
        num++;
    }
    buffer[num] = '\0';

    return num;
}

/**
 * @brief Wake up the device by driving pin low->high
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
bool XBee::is_active()
{
    return gpio_get(statuspin);
}