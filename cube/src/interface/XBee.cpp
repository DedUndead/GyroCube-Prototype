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
 */
void XBee::sendData(const char *s){
    if(checkStatus()){
        uart_puts(uart, s);
    }
    else{
        wakeUp();
        uart_puts(uart, s);
    }
}
/**
 * @brief Send a character to the uart
 * Wake up the device if needed
 * @param c character to be sent
 */
void XBee::sendData(char c){
    if(checkStatus()){
        uart_putc(uart, c);
    }
    else{
        wakeUp();
        uart_putc(uart, c);
    }
}
/**
 * @brief read a character from the UART
 * @return char read or 0 if uart is empty
 */
char XBee::getData(){
    if(uart_is_readable(uart)){
        return uart_getc(uart);
    }
    else return 0;
}
/**
 * @brief Wake up the device by driving pin high->low
 */
void XBee::wakeUp(){
    gpio_put(wokepin, false);
    sleep_ms(1);
    gpio_put(wokepin, true);
}
/**
 * @brief Check if device is sleeping
 * @return bool 
 */bool XBee::checkStatus(){
    return gpio_get(statuspin);
}