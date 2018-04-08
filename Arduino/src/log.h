#ifndef LOG_H
#define LOG_H

//#include "events.h"

enum LoggerLevel {
    LL_UNKNOWN = -1,
    LL_TRACE = 1,
    LL_DEBUG = 2,
    LL_INFO =  3,
    LL_WARN =  4,
    LL_ERROR = 5,
    LL_FATAL = 6
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
        static int setLevel(String input) {
            LoggerLevel newLevel = toLevel(input);

            if (newLevel != LL_UNKNOWN) {
                level = newLevel;
            }

            return newLevel;
        }

        static void trace(String tag, String message) { log(LL_TRACE, tag, message); }

        static void debug(String tag, String message) { log(LL_DEBUG, tag, message); }

        static void info(String tag, String message) { log(LL_INFO, tag, message); }

        static void warn(String tag, String message) { log(LL_WARN, tag, message); }

        static void error(String tag, String message) { log(LL_ERROR, tag, message); }

        static void fatal(String tag, String message) { log(LL_FATAL, tag, message); }

    private:
        static LoggerLevel level;

        static LoggerLevel toLevel(String input) {
            LoggerLevel newLevel = LL_UNKNOWN;

            if (compareIgnoringCase(input, "TRACE")) {
                newLevel = LL_TRACE;
            } else if (compareIgnoringCase(input, "DEBUG")) {
                newLevel = LL_DEBUG;
            } else if (compareIgnoringCase(input, "INFO")) {
                newLevel = LL_INFO;
            } else if (compareIgnoringCase(input, "WARN")) {
                newLevel = LL_WARN;
            } else if (compareIgnoringCase(input, "ERROR")) {
                newLevel = LL_ERROR;
            } else if (compareIgnoringCase(input, "FATAL")) {
                newLevel = LL_FATAL;
            } else {
                newLevel = LL_UNKNOWN;
            }

            if(newLevel != LL_UNKNOWN){
                info("Log", "New log level: " + fromLevel(newLevel));
            }

            return newLevel;
        }


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