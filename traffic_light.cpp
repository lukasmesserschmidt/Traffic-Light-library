#include "traffic_light.h"

#include <Arduino.h>

#define INVALID_PIN -1
#define DEFECT_THRESHOLD 1000

// constructor
TrafficLight::TrafficLight(int red_pin, int yellow_pin, int green_pin)
    : light_pins{red_pin, yellow_pin, green_pin},
      test_pins{INVALID_PIN, INVALID_PIN, INVALID_PIN},
      pattern{false, false, false},
      cycle(),
      activity_cycle(),
      event_manager() {
    pinMode(red_pin, OUTPUT);
    pinMode(yellow_pin, OUTPUT);
    pinMode(green_pin, OUTPUT);
}

// getters
bool TrafficLight::is_cycle_enabled() {
    return cycle.is_enabled();
}

bool TrafficLight::is_activity_cycle_enabled() {
    return activity_cycle.is_enabled();
}

bool* TrafficLight::get_current_pattern() {
    return pattern;
}

// setters
void TrafficLight::set_test_pin(int index, int pin) {
    test_pins[index] = pin;
    pinMode(pin, INPUT);
}

void TrafficLight::set_test_pins(int red_pin, int yellow_pin, int green_pin) {
    set_test_pin(0, red_pin);
    set_test_pin(1, yellow_pin);
    set_test_pin(2, green_pin);
}

void TrafficLight::set_pattern(bool red_light, bool yellow_light, bool green_light) {
    pattern[0] = red_light;
    pattern[1] = yellow_light;
    pattern[2] = green_light;
}

void TrafficLight::set_cycle_repetitions_limit(unsigned long repetitions_limit) {
    cycle.set_repetitions_limit(repetitions_limit);
}

void TrafficLight::set_cycle_phases(Phase* phases, int phase_count) {
    cycle.set_phases(phases, phase_count);
}

void TrafficLight::set_activity_cycle_times(unsigned long active_time_ms, unsigned long inactive_time_ms) {
    activity_cycle.set_times(active_time_ms, inactive_time_ms);
}

// controls
void TrafficLight::enable_cycle() {
    cycle.enable();
}

void TrafficLight::disable_cycle() {
    cycle.disable();
}

void TrafficLight::enable_activity_cycle() {
    activity_cycle.enable();
}

void TrafficLight::disable_activity_cycle() {
    activity_cycle.disable();
}

// events
void TrafficLight::register_event(EventName name, void (*callback)()) {
    event_manager.connect(name, callback);
}

void TrafficLight::unregister_event(EventName name) {
    event_manager.disconnect(name);
}

// update
void TrafficLight::update() {
    if (activity_cycle.is_enabled()) {
        // update activity cycle
        activity_cycle.update();

        // enable/disable cycle and emit events
        if (activity_cycle.has_state_changed()) {
            if (activity_cycle.get_state() == ActivityCycleState::ACTIVE) {
                cycle.enable();
                event_manager.emit(ACTIVITY_CYCLE_TO_ACTIVE);
            } else if (activity_cycle.get_state() == ActivityCycleState::INACTIVE) {
                cycle.disable();
                event_manager.emit(ACTIVITY_CYCLE_TO_INACTIVE);
            }

            event_manager.emit(ACTIVITY_CYCLE_STATE_CHANGED);
        }
    }

    if (cycle.is_enabled()) {
        // update cycle
        cycle.update();

        // update pattern
        for (int i = 0; i < 3; i++) {
            pattern[i] = cycle.get_current_phase()->pattern[i];
        }

        // emit events
        if (cycle.has_phase_changed()) {
            event_manager.emit(CYCLE_PHASE_CHANGED);
        }
        if (cycle.has_restarted()) {
            event_manager.emit(CYCLE_RESTARTED);
        }
    }

    // power lights
    for (int i = 0; i < 3; i++) {
        digitalWrite(light_pins[i], pattern[i] ? HIGH : LOW);
    }

    // test pins
}

void TrafficLight::test_light_pins() {
    EventName event_names[] = {
        RED_LIGHT_DEFECT,
        YELLOW_LIGHT_DEFECT,
        GREEN_LIGHT_DEFECT,
    };

    for (int i = 0; i < 3; i++) {
        if (test_pins[i] != INVALID_PIN && analogRead(test_pins[i]) > DEFECT_THRESHOLD) {
            event_manager.emit(event_names[i]);
        }
    }
}