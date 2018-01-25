#ifndef ALARM_ENGINE_H
#define ALARM_ENGINE_H

#include "../log.h"
#include "../events.h"
#include "../common.h"
#include "config.h"

#define NO_SOURCE             "Alarm"
#define KEY             "Key"

namespace {

    class Alarm {

    public:
        static void enable(long activatingTime, long suspiciousTime, AlarmPin pins[MASTER_PIN_NUMBER]) {
            AlarmConfig::set(activatingTime, suspiciousTime, pins);
            setup();
        }

        static void disable() {
            AlarmConfig::clear();
            setup();
        }

        static void start() {
            AlarmConfig::load();
            setup();
        }

        static void stop() {
            AlarmConfig::save();
        }

        static void setup() {
            if (AlarmConfig::enabled) {
                pinMode(ALARM_PIN, OUTPUT);
                pinMode(SABOTAGE_OUT_PIN, OUTPUT);

                AlarmConfig::forEachDefinedPin(setPinInput);
                setStatus(AlarmConfig::getStatus(), true, NO_SOURCE);
            } else {
                log.info("Alarm disabled");
            }
        }

        static void loop() {
            if (AlarmConfig::enabled) {
                AlarmConfig::forEachDefinedPin(readSensor);

                if ((AlarmConfig::getStatus() == AS_ACTIVATING) &&
                    (millis() >= statusTime + AlarmConfig::activatingTime)) {
                    setStatus(AS_ACTIVATED);
                }
                if ((AlarmConfig::getStatus() == AS_SUSPICIOUS) &&
                    (millis() >= statusTime + AlarmConfig::suspiciousTime)) {
                    setStatus(AS_ALARMED);
                }
            }
        }

        static int setStatus(AlarmStatus newStatus) {
            return setStatus(newStatus, NO_SOURCE);
        }

        static int setStatus(AlarmStatus newStatus, String source) {
            return setStatus(newStatus, false, source);
        }

        static void key(String source) {
            log.info("KEY trigger!");
            if (AlarmConfig::getStatus() != AS_IDLE) {
                setStatus(AS_IDLE, source);
            } else {
                setStatus(AS_ACTIVATING, source);
            }
        }

    private:
        static RemoteLog log;
        static int pinValues[MASTER_PIN_NUMBER];
        static unsigned long statusTime;

        static void enableOutput(int pin) {
            digitalWrite(pin, (AlarmConfig::outputMode == PM_HIGH) ? HIGH : LOW);
        }

        static void disableOutput(int pin) {
            digitalWrite(pin, (AlarmConfig::outputMode == PM_HIGH) ? LOW : HIGH);
        }

        static int setStatus(AlarmStatus newStatus, bool forceChange, String source) {
            AlarmStatus currentStatus = AlarmConfig::getStatus();
            if (newStatus == AS_UNKNOWN) {
                log.debug("Unknown status");
                return AS_UNKNOWN;
            } else if ((newStatus == currentStatus) && !forceChange) {
                log.debug("Unchanged status: " + fromAlarmStatus(newStatus));
                return AS_UNCHANGED;
            } else {
                AlarmConfig::setStatus(newStatus);
                log.info("Changed status to: " + AlarmConfig::statusName + ", source: " + source);

                statusTime = millis();
                EventDispatcher::publishAlarmStatusChange(AlarmConfig::getStatus(), source);

                if ((newStatus == AS_ALARMED) || (newStatus == AS_SAFETY) || (newStatus == AS_SABOTAGE)) {
                    enableOutput(ALARM_PIN);
                } else {
                    disableOutput(ALARM_PIN);
                }

                AlarmConfig::forEachDefinedPin(setSabotageOutput);

                return newStatus;
            }
        }

        static void setSabotageOutput(int pinIndex, AlarmPin pin) {
            if (pin.type == PT_SABOTAGE_OUT) {
                if (AlarmConfig::getStatus() == AS_SABOTAGE) {
                    enableOutput(SABOTAGE_OUT_PIN);
                } else {
                    disableOutput(SABOTAGE_OUT_PIN);
                }
            }
        }

        static void readSensor(int pinIndex, AlarmPin pin) {
            int lastValue = pinValues[pinIndex];
            int currentValue = digitalRead(pin.id);

            if (currentValue != lastValue) {
                pinChanged(pin, currentValue);
                pinValues[pinIndex] = currentValue;
            }
        }

        static void pinChanged(AlarmPin pin, int value) {
            String pinName = fromPinIds(pin.id);
            AlarmStatus currentStatus = AlarmConfig::getStatus();
            log.info("Changed " + pinName + " to " + (value ? "true" : "false") + " - Mode: " + fromPinMode(pin.mode));

            if (RemoteLog::getLevel() < LL_INFO) {
                EventDispatcher::publishPinChange(pinName, value);
            }

            if (pin.mode == value) {
                log.info("Activated " + pinName + " - " + fromPinType(pin.type));

                EventDispatcher::publishPinActivated(pinName, value);

                if (pin.type == PT_SABOTAGE_IN) {
                    log.warn("The node is under sabotage!");
                    setStatus(AS_SABOTAGE, "P:" + fromPinIds(pin.id));
                } else if (pin.type == PT_SAFETY) {
                    log.warn("The node is under safety!");
                    setStatus(AS_SAFETY, "P:" + fromPinIds(pin.id));
                } else if ((pin.type == PT_SENSOR) && (currentStatus == AS_ACTIVATED)) {
                    log.info("Sensor trigger!");
                    setStatus(AS_SUSPICIOUS, "P:" + fromPinIds(pin.id));
                } else if (pin.type == PT_KEY) {
                    if (currentStatus != AS_SABOTAGE) {
                        key("P:" + fromPinIds(pin.id));
                    }
                }
            }
        }

        static void setPinInput(int index, AlarmPin pin) {
            log.debug("Pin " + fromPinIds(pin.id) + " is type " + fromPinType(pin.type) + " and mode " +
                      fromPinMode(pin.mode));

            if (pin.type == PT_SABOTAGE_OUT) {
                pinMode(pin.id, OUTPUT);
            } else {
                pinMode(pin.id, INPUT);
            };
        }
    };

    RemoteLog Alarm::log = RemoteLog("alarm-engine");
    int Alarm::pinValues[MASTER_PIN_NUMBER];
    unsigned long Alarm::statusTime = millis();
}
#endif
