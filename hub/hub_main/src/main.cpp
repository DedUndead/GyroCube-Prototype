/*
===============================================================================
 Name        : Gyrocube Prototype Project
 Author      : Arefyev Pavel, Daniel Liberman, Alex Franko, Joel Kontas
 Version     : X
 License     : GPL-2.0 License
 Description : https://github.com/DedUndead/GyroCube-Prototype
===============================================================================
*/

#include "chip.h"
#include <string>
#include <atomic>
#include "board.h"
#include "delay.h"
#include "uart/lpcuart.h"
#include "mqtt/mqtt.h"
#include "json/json.hpp"

/* Macros and global variables */
#define TICKRATE_HZ          1000
#define MQTT_IP              (char *)"18.198.188.151"
#define MQTT_PORT            21883
#define NETWORK_SSID         (char *)"V46D-1"
#define NETWORK_PASS         (char *)"2483124831"
#define MQTT_TOPIC_RECEIVE   (const char *)"/gyro/web"
#define MQTT_TOPIC_SEND      (const char *)"/gyro/hub"
#define MQTT_YIELD_TIME      100
#define ZIGBEE_BUFFER_LENGTH 100

#define MIN_TEMP             10
#define MAX_SIDE             5

static volatile std::atomic_int delay(0);
static volatile uint32_t systicks(0);
static std::string mqtt_message("");
static bool mqtt_message_arrived(false);

/* Function declarations */
std::string get_settings_notification();
std::string get_weather_notification();
std::string get_sample_json(char* sample_str);
void set_systick(const int& freq);
void mqtt_message_handler(MessageData* data);
void ERROR_CONDITION();

/* Interrupt handlers */
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

    set_systick(TICKRATE_HZ);

    /* Configure MQTT */
    MQTT mqtt(mqtt_message_handler);
    int mqtt_status;
    mqtt_status = mqtt.connect(NETWORK_SSID, NETWORK_PASS, MQTT_IP, MQTT_PORT);
    mqtt_status = mqtt.subscribe(MQTT_TOPIC_RECEIVE);

    /* Serial communication with zigbee module */
	LpcPinMap txpin = { 0, 0 };
	LpcPinMap rxpin = { 1, 3 };
	LpcPinMap cts   = { 0, 9 };
	LpcPinMap none  = { -1, - 1 };
	LpcUartConfig cfg = {
			LPC_USART1, 9600,
			UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1,
			false,
			txpin, rxpin, none, cts
	};
	LpcUart zigbee(cfg);

	char zigbee_buffer[ZIGBEE_BUFFER_LENGTH];

	while (true) {
		// Bridge the message from web to cube
		if (mqtt_message_arrived) {
			// Setting package simplification
			if (mqtt_message.find("side") != std::string::npos) {
				std::string to_cube = get_settings_notification();
				zigbee.write(to_cube);

				delay_systick(10); // Keep messages synced
			}
			// Notification package simplification
			else if (mqtt_message.find("notif") != std::string::npos) {
				zigbee.write("n");

				delay_systick(10); // Keep messages synced
			}
			// Weather package simplification
			else if (mqtt_message.find("weather") != std::string::npos) {
				std::string to_cube = get_weather_notification();
				zigbee.write(to_cube);

				delay_systick(10); // Keep messages synced
			}

			mqtt_message_arrived = false;
		}

		// Bridge message from cube to web
		int receive = zigbee.read(zigbee_buffer, ZIGBEE_BUFFER_LENGTH);
		if (receive > 0) {
			zigbee_buffer[receive] = '\0';

			std::string to_web = get_sample_json(zigbee_buffer);
			mqtt.publish(MQTT_TOPIC_SEND, to_web, to_web.length());
		}

		// Gather mqtt data
		mqtt_status = mqtt.yield(MQTT_YIELD_TIME);

		// Reconnect to mqtt
		while (mqtt_status != 0) {
			ERROR_CONDITION();
		}
	}

    return 0;
}

/* SUPPORT FUNCTIONS DEFINITIONS */

/**
 * @brief Construct message to cube containing settings update
 * @return String containing settings notification
 */
std::string get_settings_notification()
{
	// Simplify JSON into the correct format
	nlohmann::json settings = nlohmann::json::parse(mqtt_message);
	uint8_t  side     = settings.value("side", 0);
	uint8_t  function = settings.value("func", 0);
	uint32_t color    = settings.value("color", 0);
	int      target   = settings.value("target", 0);

	// Check if we need to add leading zero to target value
	std::string target_str("");
	if (target < 10) target_str += "0" + std::to_string(target);
	else             target_str += std::to_string(target);

	return "s" + std::to_string(side)      + "f" +
				 std::to_string(function)  + "c" +
				 std::to_string(color)     + "t" +
				 target_str;
}

/**
 * @brief Construct message to cube containing weather update
 * @return String containing weather notification
 */
std::string get_weather_notification()
{
	nlohmann::json weather = nlohmann::json::parse(mqtt_message);
	int index = weather.value("weather", 0);

	return "w" + std::to_string(index);
}

/**
 * @brief Construct sample from zigbee message
 * @param sample_str Zigbee message
 * @return           String in json format
 */
std::string get_sample_json(char* sample_str)
{
	int current_side;
	int humidity;
	int temperature;

	// Place error value if interference occured
	if (sscanf(sample_str, "s%dh%dt%d", &current_side, &humidity, &temperature) != 3 ||
		temperature < MIN_TEMP || current_side > MAX_SIDE) {
		current_side = -1;
		humidity = -1;
		temperature = -1;
	}

	nlohmann::json sample;
	sample["side"] = current_side;
	sample["humid"] = humidity;
	sample["temp"] = temperature;

	return sample.dump();
}

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
