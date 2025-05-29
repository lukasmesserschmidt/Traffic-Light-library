#ifndef ACTIVITY_CYCLE_H
#define ACTIVITY_CYCLE_H

#include <stdint.h>

enum class ActivityCycleState { ACTIVE, INACTIVE };

class ActivityCycle {
private:
  // Bit positions for flags
  static constexpr uint8_t FLAG_ENABLED = 0;
  static constexpr uint8_t FLAG_STATE_CHANGED = 1;

  ActivityCycleState state;
  unsigned long active_time_ms;
  unsigned long inactive_time_ms;
  unsigned long last_time_ms;
  uint8_t flags; // Bitfield for boolean flags

  // Disable copy constructor and assignment
  ActivityCycle(const ActivityCycle &) = delete;
  ActivityCycle &operator=(const ActivityCycle &) = delete;

public:
  ActivityCycle();
  ~ActivityCycle() = default;

  /**
   * Checks if the activity cycle is enabled.
   * @return True if the activity cycle is enabled, false otherwise.
   */
  bool is_enabled();

  /**
   * Checks if the state of the activity cycle has changed.
   * @return True if the state has changed, false otherwise.
   */
  bool has_state_changed();

  /**
   * Gets the current activity cycle state.
   * @return The current state of the activity cycle.
   */
  ActivityCycleState get_state();

  /**
   * Sets the times for the active and inactive states.
   * @param active_time_ms Time in milliseconds for the active state.
   * @param inactive_time_ms Time in milliseconds for the inactive state.
   */
  void set_times(unsigned long active_time_ms, unsigned long inactive_time_ms);

  /**
   * Enables the activity cycle.
   */
  void enable();

  /**
   * Disables the activity cycle.
   */
  void disable();

  /**
   * Updates the activity cycle state.
   */
  void update();
};

#endif