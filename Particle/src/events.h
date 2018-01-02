#ifndef EVENTS_H
#define EVENTS_H

#include "common.h"
#include <ArduinoJson.h>

namespace {
    class EventDispatcher {
    public:
        static void publishLog(String level, String className, String message) {
            StaticJsonBuffer<2048> jsonBuffer;

            String name = "log";

            JsonObject &root = jsonBuffer.createObject();
            root["name"] = name;
            root["nodeId"] = nodeId;
            root["level"] = level;
            root["class"] = className;
            root["message"] = message;
            root["timestamp"] = Time.format(TIME_FORMAT_ISO8601_FULL);

            String jsonStr;
            root.printTo(jsonStr);
            publish(name, jsonStr);
        }

        static void publishAlarmStatusChange(AlarmStatus status, String source) {
            StaticJsonBuffer<256> jsonBuffer;
            String name = "alarm-status-change";

            JsonObject &root = jsonBuffer.createObject();
            root["name"] = name;
            root["nodeId"] = nodeId;
            root["status"] = fromAlarmStatus(status);
            root["source"] = source;
            root["timestamp"] = Time.format(TIME_FORMAT_ISO8601_FULL);

            String jsonStr;
            root.printTo(jsonStr);
            publish(name, jsonStr);
        }

        static void publishNodeStatusChange(NodeStatus status) {
            StaticJsonBuffer<256> jsonBuffer;
            String name = "node-status-change";

            JsonObject &root = jsonBuffer.createObject();
            root["name"] = name;
            root["nodeId"] = nodeId;
            root["status"] = fromNodeStatus(status);
            root["timestamp"] = Time.format(TIME_FORMAT_ISO8601_FULL);

            String jsonStr;
            root.printTo(jsonStr);
            publish(name, jsonStr);
        }

        static void publishPinChange(String pinName, bool currentValue) {
            StaticJsonBuffer<256> jsonBuffer;
            String name = "pin-value-change";

            JsonObject &root = jsonBuffer.createObject();
            root["name"] = name;
            root["nodeId"] = nodeId;
            root["pinId"] = pinName;
            root["value"] = currentValue ? "true" : "false";
            root["timestamp"] = Time.format(TIME_FORMAT_ISO8601_FULL);

            String jsonStr;
            root.printTo(jsonStr);
            publish(name, jsonStr);
        }

        static void publishPinActivated(String pinName) {
            StaticJsonBuffer<256> jsonBuffer;
            String name = "pin-activated";

            JsonObject &root = jsonBuffer.createObject();
            root["name"] = name;
            root["nodeId"] = nodeId;
            root["pinId"] = pinName;
            root["timestamp"] = Time.format(TIME_FORMAT_ISO8601_FULL);

            String jsonStr;
            root.printTo(jsonStr);
            publish(name, jsonStr);
        }

        static void publishPing() {
            StaticJsonBuffer<256> jsonBuffer;
            String name = "ping";

            JsonObject &root = jsonBuffer.createObject();
            root["name"] = name;
            root["nodeId"] = nodeId;
            root["timestamp"] = Time.format(TIME_FORMAT_ISO8601_FULL);

            String jsonStr;
            root.printTo(jsonStr);
            publish(name, jsonStr);
        }

        static void publish(String eventName) {
            StaticJsonBuffer<256> jsonBuffer;

            JsonObject &root = jsonBuffer.createObject();
            root["name"] = eventName;

            String jsonStr;
            root.printTo(jsonStr);
            publish(eventName, jsonStr);
        }

        static void publish(String eventName, String payload) {
            Particle.publish(eventName, payload);
        };

    private:
        static String nodeId;
    };

    String EventDispatcher::nodeId = getCoreID();
}

#endif