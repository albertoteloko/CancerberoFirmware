#ifndef EVENTS_H
#define EVENTS_H

#include "common.h"
#include <Ethernet.h>

#define EVENTS_TAG  "Events"

namespace {

    class EventDispatcher {
    public:
        static void publishAlarmStatusChange(AlarmStatus status, String source) {
            String json = "{\"type\":\"alarm-status-changed\",\"value\":\"" + fromAlarmStatus(status) + "\",\"source\":\"" + source+"\"}";
            publish(json);
        }

        static void publishPinValue(String pinName, int currentValue) {
            String json = "{\"type\":\"alarm-pin-value\",\"pinId\":\"" + pinName + "\",\"value\": " + currentValue + "}";
            publish(json);
        }

    private:
        static EthernetClient client;

        static void publish(String message) {
            byte ip[] = MASTER_IP;
            char ipBuffer[15];
            sprintf(ipBuffer, "%i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
            info(EVENTS_TAG, "Output event: " +  message);
            int result = -1;
            info(EVENTS_TAG, "Sending event to master: " + message);

            if (client.connect(ip, MASTER_PORT)) {
                debug(EVENTS_TAG, "Connected to " + String(ipBuffer) + ":" + String(MASTER_PORT));
                client.println(message);


                long ti = millis();

                while ((millis() - ti) < 100) {
                    if (client.available()) {
                        result = client.read();
                        break;
                    }
                }

                if (result == -1) {
                    error(EVENTS_TAG, "No response");
                    result = -2;
                }
            } else {
                error(EVENTS_TAG, "Unable to connect to " + String(ipBuffer) + ":" + String(MASTER_PORT));
            }
            client.stop();
            return result;
        }
    };

    EthernetClient EventDispatcher::client;
}

#endif