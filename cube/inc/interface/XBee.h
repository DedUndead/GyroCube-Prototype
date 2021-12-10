#ifndef XBEE_H
#define XBEE_H

#include "pico/stdlib.h"

class XBee {
    public:
        XBee(uart_inst_t *uart, uint8_t rxpin, uint8_t txpin, uint8_t ctspin, uint8_t wokepin, uint8_t statuspin);
        char getData();
        void sendData(const char *c);
        void sendData(char c);
    private:
        void wakeUp();
        bool checkStatus();
        uint8_t statuspin;
        uint8_t wokepin;
        uart_inst_t *uart;

};

#endif