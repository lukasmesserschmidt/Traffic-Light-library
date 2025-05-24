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

    bool is_enabled();
    Phase* get_current_phase();
    bool has_phase_changed();
    bool has_restarted();
    bool has_reached_repetitions_limit();

    void set_repetitions_limit(unsigned long repetitions_limit);
    void set_phases(Phase phases[], int phase_count);

    void enable();
    void disable();

    void update();
};

#endif