#ifndef EVENTS_H
#define EVENTS_H

#include "common/common.h"

#define EVENTS_TAG  "Events"

namespace {

    class EventDispatcher {
    public:
        static void publishAlarmStatusChange(AlarmStatus status, PinIds source) {
            String json = "{\"type\":\"alarm-status-changed\",\"value\":\"" + fromAlarmStatus(status) + "\",\"source\":\"" + fromPinIds(source)+"\"}";
            publish(json);
        }

        static void publishPinValue(String pinName, int currentValue) {
            String json = "{\"type\":\"alarm-pin-value\",\"pinId\":\"" + pinName + "\",\"value\": " + currentValue + "}";
            publish(json);
        }

    private:
        static void publish(String message) {
            info(EVENTS_TAG, "Output event: %s", message.c_str());
        //            Particle.publish(eventName, payload);
        }
    };
}

#endif