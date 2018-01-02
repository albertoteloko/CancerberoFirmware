#ifndef LOG_H
#define LOG_H

#include "utils.h"
#include "events.h"

enum RemoteLogLevel {
    LL_UNKNOWN = 0x00,
    LL_TRACE = 0x01,
    LL_DEBUG = 0x02,
    LL_INFO = 0x03,
    LL_WARN = 0x04,
    LL_ERROR = 0x05,
    LL_FATAL = 0x06
};

namespace {
    static String fromLevel(RemoteLogLevel level) {
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

    static RemoteLogLevel toLevel(String input) {
        RemoteLogLevel newLevel = LL_UNKNOWN;

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

        return newLevel;
    }

    class RemoteLog {
    public:
        static String levelName;

        static int setLevel(String input) {
            RemoteLogLevel newLevel = toLevel(input);

            if (newLevel != LL_UNKNOWN) {
                level = newLevel;
                levelName = fromLevel(level);
            }

            return newLevel;
        }

        static RemoteLogLevel getLevel(){
            return level;
        }

        inline static void init() {
            if (!started) {
                Serial.begin(9600);

                while (!Serial) { ; // wait for serial port to connect. Needed for native USB
                }
                started = true;
            }
        }

        inline void trace(String message) { log(LL_TRACE, message); }

        inline void debug(String message) { log(LL_DEBUG, message); }

        inline void info(String message) { log(LL_INFO, message); }

        inline void warn(String message) { log(LL_WARN, message); }

        inline void error(String message) { log(LL_ERROR, message); }

        inline void fatal(String message) { log(LL_FATAL, message); }

        inline RemoteLog(String className) {
            init();
            _className = className;
        }

    private:
        static RemoteLogLevel level;
        static bool started;

        String _className;

        inline void log(RemoteLogLevel traceLevel, String message) {
            if (level <= traceLevel) {
                String traceLevelName = fromLevel(traceLevel);
                Serial.print(traceLevelName);
                Serial.print(" - ");
                Serial.print(_className);
                Serial.print(" - ");
                Serial.println(message);

                if (traceLevel >= LL_WARN) {
                    EventDispatcher::publishLog(traceLevelName, _className, message);
                }
            }
        }
    };

    bool RemoteLog::started = false;
    RemoteLogLevel RemoteLog::level = LL_DEBUG;
    String RemoteLog::levelName = fromLevel(level);
}
#endif