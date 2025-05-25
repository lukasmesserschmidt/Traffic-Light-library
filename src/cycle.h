#ifndef CYCLE_H
#define CYCLE_H

#include "phase.h"

class Cycle {
   private:
    bool enabled;
    Phase* phases;
    int phase_count;
    int phase_index;
    unsigned long repetitions_limit;
    unsigned long repetitions_count;
    unsigned long last_time_ms;

    bool phase_changed;
    bool restarted;
    bool reached_repetitions_limit;

   public:
    Cycle();

    /**
     * Checks if the cycle is enabled.
     * @return True if the cycle is enabled, false otherwise.
     */
    bool is_enabled();

    /**
     * Gets the current phase.
     * @return Pointer to the current phase.
     */
    Phase* get_current_phase();

    /**
     * Gets the number of phases in the cycle.
     * @return Number of phases.
     */
    int get_phase_count();

    /**
     * Checks if the phase has changed since the last update.
     * @return True if the phase has changed, false otherwise.
     */
    bool has_phase_changed();

    /**
     * Checks if the cycle has restarted.
     * @return True if the cycle has restarted, false otherwise.
     */
    bool has_restarted();

    /**
     * Checks if the cycle has reached its repetitions limit.
     * @return True if the repetitions limit is reached, false otherwise.
     */
    bool has_reached_repetitions_limit();

    /**
     * Sets the limit for cycle repetitions.
     * @param repetitions_limit The number of repetitions allowed.
     */
    void set_repetitions_limit(unsigned long repetitions_limit);

    /**
     * Sets the phases for the cycle.
     * @param phases Array of phases.
     * @param phase_count Number of phases in the array.
     */
    void set_phases(Phase phases[], int phase_count);

    /**
     * Enables the cycle.
     */
    void enable();

    /**
     * Disables the cycle.
     */
    void disable();

    /**
     * Updates the cycle's state.
     */
    void update();
};

#endif