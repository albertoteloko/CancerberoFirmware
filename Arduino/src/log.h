#ifndef LOG_H
#define LOG_H

namespace {
    void log(String traceLevel, String className, String message) {
        Serial.print(traceLevel);
        Serial.print(" - ");
        Serial.print(className);
        Serial.print(" - ");
        Serial.println(message);
    }

    void debug(String className, String message) { log("DEBUG", className, message); }

    void info(String className, String message) { log("INFO", className, message); }

    void warn(String className, String message) { log("WARN", className, message); }

    void error(String className, String message) { log("ERROR", className, message); }
}
#endif