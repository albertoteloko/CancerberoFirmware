#ifndef LOG_H
#define LOG_H

//#include "events.h"

enum LoggerLevel {
    LL_UNKNOWN = 0x00,
    LL_TRACE = 0x01,
    LL_DEBUG = 0x02,
    LL_INFO = 0x03,
    LL_WARN = 0x04,
    LL_ERROR = 0x05,
    LL_FATAL = 0x06
};

namespace {
    static String fromLevel(LoggerLevel level) {
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

    class Logger {
    public:
        static void trace(String tag, String message) { log(LL_TRACE, tag, message); }

        static void debug(String tag, String message) { log(LL_DEBUG, tag, message); }

        static void info(String tag, String message) { log(LL_INFO, tag, message); }

        static void warn(String tag, String message) { log(LL_WARN, tag, message); }

        static void error(String tag, String message) { log(LL_ERROR, tag, message); }

        static void fatal(String tag, String message) { log(LL_FATAL, tag, message); }

    private:
        static LoggerLevel level;

        static void log(LoggerLevel traceLevel, String tag, String message) {
            if (level <= traceLevel) {
                String traceLevelName = fromLevel(traceLevel);
                Serial.print(traceLevelName);
                Serial.print(" - ");
                Serial.print(tag);
                Serial.print(" - ");
                Serial.println(message);

                if (traceLevel >= LL_WARN) {
//                    EventDispatcher::publishLog(traceLevelName, _className, message);
                }
            }
        }
    };

    static LoggerLevel Logger::level = LL_DEBUG;
}
#endif