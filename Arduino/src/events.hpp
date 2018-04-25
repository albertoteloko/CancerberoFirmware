#ifndef EVENTS_H
#define EVENTS_H

#include "printf.hpp"
#include "common.hpp"
#include <Ethernet.h>

#define EVENT_BUFFER_LENGTH		128

#if 1
static char EVENT_BUFFER[EVENT_BUFFER_LENGTH];
#else
extern char EVENT_BUFFER[EVENT_BUFFER_LENGTH];
#endif


    class EventDispatcher {
    public:
        static void publishAlarmStatusChange(AlarmStatus status, String source) {
            snprintf(EVENT_BUFFER, sizeof(EVENT_BUFFER), "{\"type\":\"alarm-status-changed\",\"value\":\"%s\",\"source\":\"%s\"}", fromAlarmStatus(status).c_str(), source.c_str());
            publish();
        }

        static void publishPinValue(String pinName, int currentValue) {
            String json = "{\"type\":\"alarm-pin-value\",\"pinId\":\"" + pinName + "\",\"value\": " + currentValue + "}";
            publish();
        }

    private:
        static EthernetClient client;

        static int publish() {
            byte ip[] = MASTER_IP;
            int port = MASTER_PORT;
            int result = -1;

            #if(LOG_LEVEL<=DEBUG)
                Serial.print("Events - Sending event to master: ");
                Serial.println(EVENT_BUFFER);
            #endif

            if (client.connect(ip, port)) {
                #if(LOG_LEVEL<=DEBUG)
                    printfn("Events - Connected to %i.%i.%i.%i:%i", ip[0], ip[1], ip[2], ip[3], port);
                #endif
                client.println(EVENT_BUFFER);


                long ti = millis();

                while ((millis() - ti) < 100) {
                    if (client.available()) {
                        result = client.read();
                        break;
                    }
                }

                if (result == -1) {
                    #if(LOG_LEVEL<=ERROR)
                        Serial.println("Events - No response");
                    #endif
                    result = -2;
                }
            } else {
                #if(LOG_LEVEL<=ERROR)
                    printfn("Events - Unable to connect to %i.%i.%i.%i:%i", ip[0], ip[1], ip[2], ip[3], port);
                #endif
            }
            client.stop();
            return result;
        }
    };

    EthernetClient EventDispatcher::client;

#endif