#include "cycle.h"

#include <Arduino.h>

Cycle::Cycle()
    : enabled(false),
      phases(nullptr),
      phase_count(0),
      phase_index(0),
      repetitions_limit(0),
      repetitions_count(0),
      last_time_ms(0) {}

bool Cycle::is_enabled() {
    return enabled;
}

Phase* Cycle::get_current_phase() {
    return &phases[phase_index];
}

bool Cycle::has_phase_changed() {
    bool result = phase_changed;
    phase_changed = false;
    return result;
}

bool Cycle::has_restarted() {
    bool result = restarted;
    restarted = false;
    return result;
}

void Cycle::set_repetitions_limit(unsigned long repetitions_limit) {
    this->repetitions_limit = repetitions_limit;
}

void Cycle::set_phases(Phase phases[], int phase_count) {
    this->phases = phases;
    this->phase_count = phase_count;
}

void Cycle::enable() {
    enabled = true;
    phase_changed = false;
    restarted = false;
    phase_index = 0;
    last_time_ms = millis();
}

void Cycle::disable() {
    enabled = false;
    phase_changed = false;
    restarted = false;
    phase_index = 0;
}

void Cycle::update() {
    phase_changed = false;
    restarted = false;

    if (!enabled) return;

    unsigned long now = millis();
    unsigned long elapsed = now - last_time_ms;

    if (elapsed >= phases[phase_index].duration_ms) {
        phase_changed = true;
        phase_index++;
        last_time_ms = now;
    }

    if (phase_index >= phase_count) {
        restarted = true;
        phase_index = 0;
        repetitions_count++;

        if (repetitions_limit > 0 && repetitions_count >= repetitions_limit) {
            disable();
        }
    }
}