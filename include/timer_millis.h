#ifndef ARDUINO_AUTOPUMP_TIMER_MILLIS_H
#define ARDUINO_AUTOPUMP_TIMER_MILLIS_H

#include <stdint.h>
#include <stdbool.h>

#include "uptime.h"
#include "date.h"

struct timer_millis;

typedef void timer_millis_handler(struct timer_millis* timer, void*);

struct timer_millis_param_handler {
    void* param;
    timer_millis_handler* func;
};

struct timer_millis;

struct timer_millis* timer_millis_create(uint64_t running_time_ms);
bool timer_millis_start(struct timer_millis* timer);
bool timer_is_running(struct timer_millis* timer);
void timer_millis_stop(struct timer_millis* timer);
void timer_millis_reconfig(struct timer_millis* timer, struct date new_time);
void timer_millis_set_handler(struct timer_millis* timer, struct timer_millis_param_handler handler);
void timer_millis_upd(struct timer_millis* timer);
struct timer_millis_param_handler timer_millis_param_handler_create(void* param, timer_millis_handler handler);


#endif //ARDUINO_AUTOPUMP_TIMER_MILLIS_H
