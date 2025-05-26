#include "traffic_light.h"

#include <Arduino.h>

#define INVALID_PIN -1
#define DEFECT_THRESHOLD 1000

// constructor
TrafficLight::TrafficLight(int red_pin, int yellow_pin, int green_pin)
    : light_pins{red_pin, yellow_pin, green_pin},
      test_pins{INVALID_PIN, INVALID_PIN, INVALID_PIN},
      intact_lights{true, true, true},
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
    Phase* phase = cycle.get_current_phase();
    if (phase != nullptr) {
        set_pattern(phase->pattern[0], phase->pattern[1], phase->pattern[2]);
    }
}

void TrafficLight::disable_cycle() {
    cycle.disable();
    set_pattern(false, false, false);
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
    // update activity cycle
    activity_cycle.update();

    // enable/disable cycle and emit activity cycle events
    if (activity_cycle.has_state_changed()) {
        on_activity_cycle_state_changed();
    }

    // update cycle
    cycle.update();

    // emit cycle events
    if (cycle.has_phase_changed()) {
        on_cycle_phase_changed();
    }
    if (cycle.has_restarted()) {
        on_cycle_restarted();
    }
    if (cycle.has_reached_repetitions_limit()) {
        on_cycle_reached_repetitions_limit();
    }

    // power lights
    for (int i = 0; i < 3; i++) {
        digitalWrite(light_pins[i], pattern[i] ? HIGH : LOW);
    }

    // test pins
    test_for_defekt_lights();
}

void TrafficLight::on_activity_cycle_state_changed() {
    ActivityCycleState state = activity_cycle.get_state();

    // enable/disable cycle and emit events
    if (state == ActivityCycleState::ACTIVE) {
        enable_cycle();
        event_manager.emit(EventName::ACTIVITY_CYCLE_TO_ACTIVE);
    } else if (state == ActivityCycleState::INACTIVE) {
        disable_cycle();
        event_manager.emit(EventName::ACTIVITY_CYCLE_TO_INACTIVE);
    }

    event_manager.emit(EventName::ACTIVITY_CYCLE_STATE_CHANGED);
}

void TrafficLight::on_cycle_phase_changed() {
    Phase* phase = cycle.get_current_phase();

    if (phase == nullptr) return;

    set_pattern(phase->pattern[0], phase->pattern[1], phase->pattern[2]);

    event_manager.emit(EventName::CYCLE_PHASE_CHANGED);
}

void TrafficLight::on_cycle_restarted() {
    event_manager.emit(EventName::CYCLE_RESTARTED);
}

void TrafficLight::on_cycle_reached_repetitions_limit() {
    event_manager.emit(EventName::CYCLE_REACHED_REPETITIONS_LIMIT);
}

void TrafficLight::test_for_defekt_lights() {
    EventName event_names[] = {
        EventName::RED_LIGHT_DEFECT,
        EventName::YELLOW_LIGHT_DEFECT,
        EventName::GREEN_LIGHT_DEFECT,
    };

    // test for defekt lights and emit events
    for (int i = 0; i < 3; i++) {
        if (test_pins[i] != INVALID_PIN && analogRead(test_pins[i]) > DEFECT_THRESHOLD) {
            if (!intact_lights[i]) continue;

            intact_lights[i] = false;
            event_manager.emit(event_names[i]);
        }
    }
}