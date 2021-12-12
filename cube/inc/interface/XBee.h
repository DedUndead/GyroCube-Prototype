#ifndef XBEE_H
#define XBEE_H

#include "pico/stdlib.h"

class XBee {
    public:
        XBee(uart_inst_t *uart, uint8_t rxpin, uint8_t txpin, uint8_t ctspin, uint8_t wokepin, uint8_t statuspin);
        bool get_data(uint8_t* buffer, size_t len);
        void send_data(const uint8_t *s, size_t len);
        void send_data(char c);
    private:
        void wake_up();
        bool check_status();
        uint8_t statuspin;
        uint8_t wokepin;
        uart_inst_t *uart;

};

#endif