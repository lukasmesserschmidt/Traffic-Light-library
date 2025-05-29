#include "cycle.h"
#include <Arduino.h>

Cycle::Cycle()
    : phases(nullptr), phase_count(0), phase_index(0), repetitions_limit(0),
      repetitions_count(0), last_time_ms(0), flags(0) {}

Cycle::~Cycle() { delete[] phases; }

bool Cycle::is_enabled() { return flags & (1 << FLAG_ENABLED); }

Phase *Cycle::get_phase() {
  if (!is_enabled() || phase_index >= phase_count)
    return nullptr;
  return &phases[phase_index];
}

int Cycle::get_phase_count() { return phase_count; }

bool Cycle::has_phase_changed() {
  bool result = flags & (1 << FLAG_PHASE_CHANGED);
  flags &= ~(1 << FLAG_PHASE_CHANGED);
  return result;
}

bool Cycle::has_finished() {
  bool result = flags & (1 << FLAG_FINISHED);
  flags &= ~(1 << FLAG_FINISHED);
  return result;
}

bool Cycle::has_reached_repetitions_limit() {
  bool result = flags & (1 << FLAG_REACHED_LIMIT);
  flags &= ~(1 << FLAG_REACHED_LIMIT);
  return result;
}

void Cycle::set_repetitions_limit(unsigned long repetitions_limit) {
  this->repetitions_limit = repetitions_limit;
}

void Cycle::set_phases(Phase phases[], int phase_count) {
  if (phase_count <= 0 || phases == nullptr) {
    // Disable the cycle if invalid input
    this->phase_count = 0;
    delete[] this->phases;
    this->phases = nullptr;
    return;
  }

  // Delete old phases
  delete[] this->phases;

  // Allocate and copy new phases
  this->phases = new Phase[phase_count];
  for (int i = 0; i < phase_count; i++) {
    this->phases[i] = phases[i];
  }

  this->phase_count = phase_count;
  phase_index = 0;

  // Reset timing
  if (is_enabled()) {
    last_time_ms = millis();
  }
}

void Cycle::enable() {
  flags = (1 << FLAG_ENABLED); // Set enabled, clear all other flags
  repetitions_count = 0;
  phase_index = 0;
  last_time_ms = millis();
}

void Cycle::disable() {
  flags &= ~(1 << FLAG_ENABLED);
  phase_index = 0;
}

void Cycle::update() {
  // Clear one-time flags
  flags &= ~((1 << FLAG_PHASE_CHANGED) | (1 << FLAG_FINISHED) |
             (1 << FLAG_REACHED_LIMIT));

  if (!is_enabled() || phases == nullptr || phase_count == 0) {
    return;
  }

  unsigned long now = millis();
  unsigned long elapsed = (now - last_time_ms);

  // Check if current phase duration has elapsed
  if (elapsed < phases[phase_index].duration_ms) {
    return;
  }

  // Phase change logic
  flags |= (1 << FLAG_PHASE_CHANGED);
  phase_index++;
  last_time_ms = now;

  // Check if cycle finished
  if (phase_index >= phase_count) {
    flags |= (1 << FLAG_FINISHED);
    phase_index = 0;
    repetitions_count++;

    // Check repetitions limit
    if (repetitions_limit > 0 && repetitions_count >= repetitions_limit) {
      flags |= (1 << FLAG_REACHED_LIMIT);
      disable();
    }
  }
}