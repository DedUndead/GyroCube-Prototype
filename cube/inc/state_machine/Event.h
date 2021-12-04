#ifndef EVENT_H
#define EVENT_H


/**
 * @brief Quick event class for state machine handling
 * eEnter, eExit - enter and exit events
 * eTick         - state machine tick event
 * eReconfigure  - reconfiguration request event
 * eReconnect    - reconnection request event 
 */
class Event {
public: 
    enum event_type { eEnter, eExit, eTick, eReconfigure, eReconnect };
    Event(event_type e, int v = 0) : type(e), value(v) {};
    event_type type;
    int value;
};


#endif