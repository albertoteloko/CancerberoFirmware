#ifndef ALARM_ENGINE_H
#define ALARM_ENGINE_H

#include "../log.h"
#include "../events.h"
#include "../common.h"
#include "config.h"

#define NO_SOURCE               "Unknown"
#define KEY                     "Key"

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
            if (AlarmConfig::enabled()) {
                pinMode(ALARM_PIN, OUTPUT);

                #ifdef SABOTAGE_OUT_PIN
                pinMode(SABOTAGE_OUT_PIN, OUTPUT);
                #endif

                AlarmConfig::forEachDefinedPin(setPinInput);
                setStatus(AlarmConfig::getStatus(), true, AlarmConfig::getStatusSource());
            } else {
                log.info("Alarm disabled");
            }
        }

        static void loop() {
            if (AlarmConfig::enabled()) {
                AlarmConfig::forEachDefinedPin(readSensor);

                if ((AlarmConfig::getStatus() == AS_ACTIVATING) &&
                    (millis() >= statusTime + AlarmConfig::activatingTime())) {
                    setStatus(AS_ACTIVATED, AlarmConfig::getStatusSource());
                }
                if ((AlarmConfig::getStatus() == AS_SUSPICIOUS) &&
                    (millis() >= statusTime + AlarmConfig::suspiciousTime())) {
                    setStatus(AS_ALARMED, AlarmConfig::getStatusSource());
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
        static bool activations[MASTER_PIN_NUMBER];
        static unsigned long pinTimes[MASTER_PIN_NUMBER];
        static unsigned long statusTime;

        static void enableOutput(int pin) {
            digitalWrite(pin, (AlarmConfig::outputMode() == PM_HIGH) ? HIGH : LOW);
        }

        static void disableOutput(int pin) {
            digitalWrite(pin, (AlarmConfig::outputMode() == PM_HIGH) ? LOW : HIGH);
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
                AlarmConfig::setStatus(newStatus, source.c_str());
                log.info("Changed status to: " + AlarmConfig::statusName + ", source: " + source);

                statusTime = millis();
                EventDispatcher::publishAlarmStatusChange(AlarmConfig::getStatus(), source);

                if ((newStatus == AS_ALARMED) || (newStatus == AS_SAFETY) || (newStatus == AS_SABOTAGE)) {
                    enableOutput(ALARM_PIN);
                } else {
                    disableOutput(ALARM_PIN);
                }

                #ifdef SABOTAGE_OUT_PIN
                setSabotageOutput();
                #endif

                return newStatus;
            }
        }

        #ifdef SABOTAGE_OUT_PIN
        static void setSabotageOutput( ) {
            if (AlarmConfig::getStatus() == AS_SABOTAGE) {
                enableOutput(SABOTAGE_OUT_PIN);
            } else {
                disableOutput(SABOTAGE_OUT_PIN);
            }
        }
        #endif

        static void readSensor(int pinIndex, AlarmPin pin) {
            bool informValue = false;
            String pinName = fromPinIds(pin.id);
            unsigned long time = millis();
            bool lastActivation = activations[pinIndex];
            int currentValue;

            if(pin.input  == PIN_ANALOG) {
                currentValue = analogRead(pin.id);
            } else {
                currentValue = digitalRead(pin.id);
            }

            bool currentActivation = isPinActivated(pin, currentValue);

            if((pinTimes[pinIndex] == 0) || ((pinTimes[pinIndex] + PIN_INFORM_INTERVAL) < time)){
                log.info("Informing pin " + pinName + " has value of " + currentValue);
                informValue = true;
                pinTimes[pinIndex] = time;
            }

            if (currentActivation != lastActivation) {
                log.info("Pin activation changed " + pinName + " to " + (currentActivation ? "enabled" : "disabled") + " value: " + currentValue);
                informValue = true;
                pinActivatedChange(pin, currentActivation);
                activations[pinIndex] = currentActivation;
            }

            if (informValue){
                EventDispatcher::publishPinValue(pinName, currentValue);
            }
        }

        static bool isPinActivated(AlarmPin pin, int value){
            if(pin.input == PIN_DIGITAL){
                return (value == pin.mode);
            }else{
                if(pin.mode == PM_HIGH){
                    return (value >= pin.threshold);
                }else{
                    return (value <= pin.threshold);
                }
            }
        }

        static void pinActivatedChange(AlarmPin pin, bool currentActivation) {
            AlarmStatus currentStatus = AlarmConfig::getStatus();

            if(currentActivation){
                if (pin.type == PT_SABOTAGE) {
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
            log.debug("Pin " + fromPinIds(pin.id) + " is type " + fromPinType(pin.type) + " and mode " + fromPinMode(pin.mode));
            pinMode(pin.id, INPUT);
        }
    };

    RemoteLog Alarm::log = RemoteLog("alarm-engine");
    bool Alarm::activations[MASTER_PIN_NUMBER];
    unsigned long Alarm::pinTimes[MASTER_PIN_NUMBER];
    unsigned long Alarm::statusTime = millis();
}
#endif
