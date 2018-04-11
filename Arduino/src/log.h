#ifndef LOG_H
#define LOG_H

#include "printf.h"

enum LoggerLevel {
    LL_UNKNOWN = -1,
    LL_TRACE = 1,
    LL_DEBUG = 2,
    LL_INFO =  3,
    LL_WARN =  4,
    LL_ERROR = 5,
    LL_FATAL = 6
};

String fromLevel(LoggerLevel level) {
    switch (level) {
        case LL_TRACE:
            return "TRACE";
        case LL_DEBUG:
            return "DEBUG";
        case LL_INFO:
            return "INFO";
        case LL_WARN:
            return "WARN";
        case LL_ERROR:
            return "ERROR";
        case LL_FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
    }
}

#define log(traceLevel, tag, message, ...) {																		\
        String traceLevelName = fromLevel(traceLevel);                                                              \
        Serial.print(traceLevelName);                                                                  \
        Serial.print(" - ");                                                                  \
        Serial.print(tag);                                                                  \
        Serial.print(" - ");                                                                  \
        printfn(message,  ##__VA_ARGS__);                                                                           \
}

#define trace(tag, message,...) { log(LL_TRACE, tag, message, ##__VA_ARGS__); }

#define debug(tag, message,...) { log(LL_DEBUG, tag, message, ##__VA_ARGS__); }

#define info(tag, message,...) { log(LL_INFO, tag, message, ##__VA_ARGS__); }

#define warn(tag, message,...) { log(LL_WARN, tag, message, ##__VA_ARGS__); }

#define error(tag, message,...) { log(LL_ERROR, tag, message, ##__VA_ARGS__); }

#define fatal(tag, message,...) { log(LL_FATAL, tag, message, ##__VA_ARGS__); }

#endif