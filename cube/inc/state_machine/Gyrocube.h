#ifndef GYROCUBE_H
#define GYROCUBE_H


#include "state_machine/Event.h"
#include "sensor/Hih6020.h"
#include "actuator/NeoPixel.h"

#define N_SIDES                6
#define COLOR_CODE_BUFFER_SIZE 8

class Gyrocube;

typedef void (Gyrocube::*state_ptr)(const Event &);
typedef struct side_settings {
    uint8_t function;
    char color[COLOR_CODE_BUFFER_SIZE];
    int target;
} side_settings;

/**
 * @brief Abstraction of gyrocube system in form of a state machine
 * State machine UML can be viewed from the documentation
 */
class Gyrocube {
public:
    Gyrocube(Hih6020* sensor_, NeoPixel* leds_, uint8_t side, bool standalone = false);
    void handle_state(const Event& e);
    void update_settings(uint8_t side, side_settings new_setting);
private:
    state_ptr current_state;
    void set_state(state_ptr new_state);
    void startup(const Event& e);
    void state_idle(const Event& e);
    void state_lamp(const Event& e);
    void state_temp(const Event& e);
    void state_humid(const Event& e);
    void state_weather(const Event& e);
    void state_notification(const Event& e);

    void clear();
    void set_state(uint8_t state_index);

    int timer;                       // Timer for filtering tick events
    bool update_required;            // Flag identifies external changes
    bool function_changed;           // Flag identifes change of the function
    uint8_t current_side;            // Current cube's placement
    side_settings settings[N_SIDES]; // Setting individual for each side

    Hih6020* sensor;
    NeoPixel* leds;

    // State pointer array to quickly call needed state based on side's setting
    void (Gyrocube::*functional_states[N_SIDES])(const Event&);
};


#endif