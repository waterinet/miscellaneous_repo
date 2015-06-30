#ifndef STOP_WATCH_H
#define STOP_WATCH_H

#include "stime.h"

namespace pgr {

class stop_watch
{
public:
    stop_watch() : _running(false) {}
    ~stop_watch() {}
    bool is_running() const {
        return _running;
    }
    void reset() {
        duration = stime_duration(0);
        _running = false;
    }
    void restart() {
        duration = stime_duration(0);
        time_point = stime::cur_time();
        _running = true;
    }
    void start() {
        if (!_running) {
            time_point = stime::cur_time();
            _running = true;
        }
    }
    void stop() {
        if (_running) {
            duration += stime::cur_time() - time_point;
            _running = false;
        }
    }
    long elapsed() const {
        return (long)duration.total_milliseconds();
    }

private:
    bool _running;
    stime_duration duration;
    stime time_point;
};

} //namespace pgr

#endif // STOP_WATCH_H
