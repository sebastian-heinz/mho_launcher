#include "log.h"
#include "win_util.h"
#include "str_util.h"
#include "blockingconcurrentqueue.h"

#include <windows.h>
#include <atomic>
#include <string>
#include <thread>
#include <iostream>
#include <fstream>
#include <cstdarg>
#include <ctime>
#include <cstdio>

struct LogEvent {
    std::string msg;
};

static std::atomic<bool> is_running{false};
static moodycamel::BlockingConcurrentQueue<LogEvent> *events = nullptr;
static std::ofstream log_file;

static void run_events() {
    LogEvent event{};
    while (is_running) {
        if (!events->wait_dequeue_timed(event, 500 * 1000)) {
            continue;
        }
        std::cout << event.msg;
        if (log_file.is_open()) {
            log_file << event.msg;
            log_file.flush();
        }
    }
}

void log_init() {
    // Build absolute path next to the exe: <exe_dir>/mho_launcher_YYYYMMDD_HHMMSS.log
    time_t now = time(nullptr);
    struct tm *t = localtime(&now);
    char timestamp[64];
    snprintf(timestamp, sizeof(timestamp), "mho_launcher_%04d%02d%02d_%02d%02d%02d.log",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);

    std::wstring dir = get_exe_dir();
    std::string path = ws_2_s(dir) + timestamp;
    log_file.open(path, std::ios::out | std::ios::trunc);

    events = new moodycamel::BlockingConcurrentQueue<LogEvent>(100);
    is_running = true;
    std::thread(run_events).detach();
}

void log_shutdown() {
    is_running = false;
    if (log_file.is_open()) {
        log_file.close();
    }
}

void log(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    std::string buf = vformat(fmt, ap);
    va_end(ap);
    events->enqueue({buf});
}
