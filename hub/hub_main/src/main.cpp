/*
===============================================================================
 Name        : main.c
 Author      : Arefyev Pavel, Daniel Liberman, Alex Franko
 Version     : X
 License     : GPL-2.0 License
 Description : https://github.com/DedUndead/ABB-Vent-Control
===============================================================================
*/

#include "chip.h"
#include <string>
#include <atomic>
#include "board.h"
#include "delay.h"
#include "uart/lpcuart.h"
#include "mqtt/mqtt.h"

void set_systick(const int& freq);
void mqtt_message_handler(MessageData* data);
void ERROR_CONDITION();

static volatile std::atomic_int delay(0);
static volatile uint32_t systicks(0);
static std::string mqtt_message("");
static bool mqtt_message_arrived(false);

extern "C" {
	void SysTick_Handler(void)
	{
		systicks++;
		if (delay > 0) delay--;
	}
}

/* MAIN PROGRAM BODY */
int main(void) {

#if defined (__USE_LPCOPEN)
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    Board_Init();
#endif
#endif

    /* Remove after debugging finished */
	LpcPinMap none = {-1, -1}; // unused pin has negative values in it
	LpcPinMap txpin = { 0, 18 }; // transmit pin that goes to debugger's UART->USB converter
	LpcPinMap rxpin = { 0, 13 }; // receive pin that goes to debugger's UART->USB converter
	LpcUartConfig cfg = { LPC_USART0, 115200, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1, false, txpin, rxpin, none, none };
	LpcUart uart(cfg);

    return 0;
}

/* SUPPORT FUNCTIONS DEFINITIONS */

/**
 * @brief Configure systick to a specific frequency
 * @param freq Target frequency
 */
void set_systick(const int& freq)
{
    uint32_t sys_tick_rate;
    Chip_Clock_SetSysTickClockDiv(1);
    sys_tick_rate = Chip_Clock_GetSysTickClockRate();
    SysTick_Config(sys_tick_rate / freq);
}

/**
 * @brief Delay execution by amount of systicks
 * @param ticks Number of ticks for delay
 */
void delay_systick(const int ticks)
{
	delay = ticks;
	while (delay > 0) {
		__WFI();
	}
}

/**
 * @brief Return time elapsed from program start
 * @return Time in ms
 */
uint32_t millis()
{
	return systicks;
}

/**
 * @brief Handle message that aarrived via MQTT
 * Parse the message and store it in shared buffer
 * @param data Internal MQTT data typr
 */
void mqtt_message_handler(MessageData* data)
{
	mqtt_message_arrived = true;
	mqtt_message = "";

	// Parse message from payload
	char payload_parsed[READ_BUF_LENGTH];
	snprintf(
			payload_parsed,
			data->message->payloadlen + 1,
			"%.*s",
			data->message->payloadlen,
			(char *)data->message->payload
	);
	mqtt_message = payload_parsed;
}

void ERROR_CONDITION()
{
	while (true) delay_systick(100);
}
