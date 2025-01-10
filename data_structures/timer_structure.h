#pragma once

#include <stdio.h>
#include <sys/time.h>

struct timer_structure {
    struct timeval start_time;
    double elapsed;
    int running;
};

void start_timer(struct timer_structure * timer);

void stop_timer(struct timer_structure * timer);

double get_time(struct timer_structure * timer);