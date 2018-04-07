#ifndef EVENTS_H
#define EVENTS_H

#include "common/common.h"
#include <ArduinoJson.h>


namespace {

    class EventDispatcher {
    public:
        static void publishLog(String level, String className, String message) {
            StaticJsonBuffer<2048> jsonBuffer;

            String name = "log";

            JsonObject &root = jsonBuffer.createObject();
            root["type"] = name;
            root["nodeId"] = nodeId;
            root["level"] = level;
            root["class"] = className;
            root["message"] = message;

            String jsonStr;
            root.printTo(jsonStr);
            publish(name, jsonStr);
        }

        static void publishAlarmStatusChange(AlarmStatus status, String source) {
            StaticJsonBuffer<256> jsonBuffer;
            String name = "alarm-status-changed";

            JsonObject &root = jsonBuffer.createObject();
            root["type"] = name;
            root["nodeId"] = nodeId;
            root["value"] = fromAlarmStatus(status);
            root["source"] = source;

            String jsonStr;
            root.printTo(jsonStr);
            publish(name, jsonStr);
        }

        static void publishPinValue(String pinName, int currentValue) {
            StaticJsonBuffer<256> jsonBuffer;
            String name = "alarm-pin-value";

            JsonObject &root = jsonBuffer.createObject();
            root["type"] = name;
            root["nodeId"] = nodeId;
            root["pinId"] = pinName;
            root["value"] = currentValue;

            String jsonStr;
            root.printTo(jsonStr);
            publish(name, jsonStr);
        }

        static void publish(String eventName) {
            StaticJsonBuffer<256> jsonBuffer;

            JsonObject &root = jsonBuffer.createObject();
            root["type"] = eventName;

            String jsonStr;
            root.printTo(jsonStr);
            publish(eventName, jsonStr);
        }

        static void publish(String eventName, String payload) {
//            Particle.publish(eventName, payload);
        };

    private:
        static String nodeId;
    };

    String EventDispatcher::nodeId = getCoreID();
}

#endif