#include "../include/timer_millis.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/uptime.h"
#include "../include/date.h"

struct timer_millis {
    uint64_t start_time_ms;
    uint64_t running_time_ms;
    struct timer_millis_param_handler handler;
    bool is_running;
};

static void handler_call(struct timer_millis* timer, struct timer_millis_param_handler handler);

struct timer_millis* timer_millis_create(uint64_t running_time_ms) {
    struct timer_millis* timer = malloc(sizeof(struct timer_millis));
    *timer = (struct timer_millis) {
        .running_time_ms = running_time_ms,
        .handler = timer_millis_param_handler_create(NULL, NULL),
    };
    return timer;
}

bool timer_millis_start(struct timer_millis* timer) {
    if (timer->running_time_ms != 0) {
        timer->start_time_ms = millis();
        timer->is_running = true;
    }
    return timer->is_running;
}

bool timer_is_running(struct timer_millis* timer) {
    return timer->is_running;
}

void timer_millis_stop(struct timer_millis* timer) {
    timer->is_running = false;
}

void timer_millis_reconfig(struct timer_millis* timer, struct date new_time) {
    timer->running_time_ms = date_to_ms(new_time);
    timer_millis_stop(timer);
}

void timer_millis_set_handler(struct timer_millis* timer, struct timer_millis_param_handler handler) {
    timer->handler = handler;
}

void timer_millis_upd(struct timer_millis* timer) {
    if (!timer->is_running) return;
    uint64_t curr_millis = millis();
    if (curr_millis - timer->start_time_ms >= timer->running_time_ms && timer->handler.func) {
        timer->start_time_ms += timer->running_time_ms;
        handler_call(timer, timer->handler);
    }
}

struct timer_millis_param_handler timer_millis_param_handler_create(void* param, timer_millis_handler handler) {
    return (struct timer_millis_param_handler) {
        .param = param, .func = handler
    };

}

static void handler_call(struct timer_millis* timer, struct timer_millis_param_handler handler) {
    handler.func(timer, handler.param);
}
