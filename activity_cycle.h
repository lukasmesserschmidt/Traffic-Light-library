#ifndef ACTIVITY_CYCLE_H
#define ACTIVITY_CYCLE_H

enum class ActivityCycleState {
    ACTIVE,
    INACTIVE
};

class ActivityCycle {
   private:
    bool enabled;
    ActivityCycleState state;
    bool state_changed;
    unsigned long active_time_ms;
    unsigned long inactive_time_ms;
    unsigned long last_time_ms;

   public:
    ActivityCycle();

    bool is_enabled();
    bool has_state_changed();
    ActivityCycleState get_state();

    void set_times(unsigned long active_time_ms, unsigned long inactive_time_ms);

    void enable();
    void disable();

    void update();
};

#endif