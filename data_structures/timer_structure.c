#include <stdio.h>
#include <sys/time.h>

struct timer_structure {
    struct timeval start_time;
    double elapsed;
    int running;
};

void start_timer(struct timer_structure * timer) {
    /* If timer is not initialized or is running */
    if (!timer || timer->running)
        return;

    gettimeofday(&(timer->start_time), NULL);
    timer->elapsed = 0.0;
    timer->running = 1;
}

void stop_timer(struct timer_structure * timer) {
    struct timeval now;

    /* If timer is not initialized or is not running */
    if(!timer || !timer->running)
        return;
    
    gettimeofday(&now, NULL);
    timer->elapsed += (now.tv_sec - timer->start_time.tv_sec) * 1000.0;
    timer->elapsed += (now.tv_usec - timer->start_time.tv_usec) / 1000.0;
    timer->running = 0;
}

double get_time(struct timer_structure * timer) {

    double elapsed_time;

    if(!timer)
        return 0;

    if(!timer->running)
        return timer->elapsed;


    stop_timer(timer);
    elapsed_time = timer->elapsed;
    start_timer(timer);
    
    return elapsed_time;
}