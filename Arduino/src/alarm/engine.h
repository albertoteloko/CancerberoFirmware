#ifndef ALARM_ENGINE_H
#define ALARM_ENGINE_H

#include "../common/common.h"
#include "../log.h"
#include "../events.h"
#include "config.h"



#define ALARM_TAG               "Alarm"

namespace {

    class EventDispatcher;

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
//            setup();
        }

        static void stop() {
            AlarmConfig::save();
        }

        static void setup() {
            pinMode(ALARM_PIN, OUTPUT);

            AlarmConfig::forEachDefinedPin(setPinInput);
            setStatus(AlarmConfig::getStatus(), true, PI_UNKNOWN);
        }

        static void loop() {
            AlarmConfig::forEachDefinedPin(readSensor);

            if(mustInform()){
                latestNotifications = millis();
            }

            if ((AlarmConfig::getStatus() == AS_ACTIVATING) &&
                (millis() >= statusTime + AlarmConfig::activatingTime())) {
                setStatus(AS_ACTIVATED, AlarmConfig::getStatusSource());
            }
            if ((AlarmConfig::getStatus() == AS_SUSPICIOUS) &&
                (millis() >= statusTime + AlarmConfig::suspiciousTime())) {
                setStatus(AS_ALARMED, AlarmConfig::getStatusSource());
            }
        }

        static int setStatus(AlarmStatus newStatus) {
            return setStatus(newStatus, PI_UNKNOWN);
        }

        static int setStatus(AlarmStatus newStatus, PinIds source) {
            return setStatus(newStatus, false, source);
        }

    private:
        static bool activations[MASTER_PIN_NUMBER];
        static long pinTimes[MASTER_PIN_NUMBER];
        static unsigned long statusTime;
        static unsigned long latestNotifications;

        static int setStatus(AlarmStatus newStatus, bool forceChange, PinIds source) {
            AlarmStatus currentStatus = AlarmConfig::getStatus();
            if (newStatus == AS_UNKNOWN) {
                debug(ALARM_TAG,"Unknown status");
                return AS_UNKNOWN;
            } else if ((newStatus == currentStatus) && !forceChange) {
                debug(ALARM_TAG,"Unchanged status: %s", fromAlarmStatus(newStatus).c_str());
                return AS_UNCHANGED;
            } else {
                AlarmConfig::setStatus(newStatus, source);
                info(ALARM_TAG, "Changed status to: %s, source: %s",fromAlarmStatus(newStatus).c_str(), fromPinIds(source).c_str());

                statusTime = millis();
                EventDispatcher::publishAlarmStatusChange(AlarmConfig::getStatus(), source);

                if ((newStatus == AS_ALARMED) || (newStatus == AS_SAFETY) || (newStatus == AS_SABOTAGE)) {
                    digitalWrite(ALARM_PIN, HIGH);
                } else {
                    digitalWrite(ALARM_PIN, LOW);
                }

                return newStatus;
            }
        }

        static void readSensor(int pinIndex, AlarmPin pin) {
            bool informValue = false;
            String pinName = fromPinIds(pin.id);
            long time = millis();
            bool lastActivation = activations[pinIndex];
            int currentValue = 45;

//            if(pin.input  == PIN_ANALOG) {
//                currentValue = analogRead(pin.id);
//            } else {
//                currentValue = digitalRead(pin.id);
//            }

            bool currentActivation = isPinActivated(pin, currentValue);

            if(mustInform()){
                info(ALARM_TAG, "Informing pin %s has value of %i",pinName.c_str(), currentValue);
                informValue = true;
            }

            if (currentActivation != lastActivation) {
                info(ALARM_TAG, "Pin activation changed %s to value: %i", pinName.c_str(), (currentActivation ? "enabled" : "disabled"), currentValue);
                informValue = true;
                pinActivatedChange(pin, currentActivation);
                activations[pinIndex] = currentActivation;
            }

            if (informValue){
                EventDispatcher::publishPinValue(pinName, currentValue);
            }
        }

        static bool mustInform(){
            return (latestNotifications == 0) || (latestNotifications + PIN_INFORM_INTERVAL) < millis();
        }

        static bool isPinActivated(AlarmPin pin, int value){
//            if(pin.input == PIN_DIGITAL){
//                return (value == pin.mode);
//            }else{
//                if(pin.mode == PM_HIGH){
//                    return (value >= pin.threshold);
//                }else{
//                    return (value <= pin.threshold);
//                }
//            }
return false;
        }

        static void pinActivatedChange(AlarmPin pin, bool currentActivation) {
            AlarmStatus currentStatus = AlarmConfig::getStatus();

            if(currentActivation){
                if (pin.type == PT_SABOTAGE) {
                    warn(ALARM_TAG,"The node is under sabotage!");
                    setStatus(AS_SABOTAGE, pin.id);
                } else if (pin.type == PT_SAFETY) {
                    warn(ALARM_TAG,"The node is under safety!");
                    setStatus(AS_SAFETY, pin.id);
                } else if ((pin.type == PT_SENSOR) && (currentStatus == AS_ACTIVATED)) {
                    info(ALARM_TAG, "Sensor trigger!");
                    setStatus(AS_SUSPICIOUS, pin.id);
                } else if (pin.type == PT_KEY) {
                    if (currentStatus != AS_SABOTAGE) {
                        key(pin.id);
                    }
                }
            }
        }

      static void key(PinIds source) {
            info(ALARM_TAG, "KEY trigger!");
            if (AlarmConfig::getStatus() != AS_IDLE) {
                setStatus(AS_IDLE, source);
            } else {
                setStatus(AS_ACTIVATING, source);
            }
        }

        static void setPinInput(int index, AlarmPin pin) {
            debug(ALARM_TAG,"Pin %s is type %s and mode %s",fromPinIds(pin.id).c_str(), fromPinType(pin.type).c_str(), fromPinMode(pin.mode).c_str());
            pinMode(pin.id, INPUT);
        }
    };

    bool Alarm::activations[MASTER_PIN_NUMBER];
    unsigned long Alarm::statusTime = millis();
    unsigned long Alarm::latestNotifications = millis();
}
#endif
