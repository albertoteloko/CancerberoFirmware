#ifndef ALARM_ENGINE_H
#define ALARM_ENGINE_H

#include "../common.h"
#include "../log.h"
#include "../events.h"
#include "config.h"

#define NO_SOURCE               "Unknown"
#define ALARM_TAG               "Alarm"

namespace {

    class EventDispatcher;

    class Alarm {

    public:
        static void start() {
            AlarmConfig::load();
            setup();
        }

        static void setup() {
            pinMode(ALARM_PIN, OUTPUT);

            AlarmConfig::forEachDefinedPin(setPinInput);
            setStatus(AlarmConfig::getStatus(), true, AlarmConfig::getStatusSource());
        }

        static void loop() {
            AlarmConfig::forEachDefinedPin(readSensor);

            if ((AlarmConfig::getStatus() == AS_ACTIVATING) &&
                (millis() >= statusTime + ACTIVATING_TIME)) {
                setStatus(AS_ACTIVATED, AlarmConfig::getStatusSource());
            }
            if ((AlarmConfig::getStatus() == AS_SUSPICIOUS) &&
                (millis() >= statusTime + SUSPICIOUS_TIME)) {
                setStatus(AS_ALARMED, AlarmConfig::getStatusSource());
            }
        }

        static int setStatus(AlarmStatus newStatus) {
            return setStatus(newStatus, NO_SOURCE);
        }

        static int setStatus(AlarmStatus newStatus, String source) {
            return setStatus(newStatus, false, source);
        }

    private:
        static bool activations[PIN_NUMBER];
        static unsigned long pinTimes[PIN_NUMBER];
        static unsigned long statusTime;

        static int setStatus(AlarmStatus newStatus, bool forceChange, String source) {
            AlarmStatus currentStatus = AlarmConfig::getStatus();
            if (newStatus == AS_UNKNOWN) {
                debug(ALARM_TAG,"Unknown status");
                return AS_UNKNOWN;
            } else if ((newStatus == currentStatus) && !forceChange) {
                debug(ALARM_TAG,"Unchanged status: " +  fromAlarmStatus(newStatus));
                return AS_UNCHANGED;
            } else {
                AlarmConfig::setStatus(newStatus, source.c_str());
                info(ALARM_TAG, "Changed status to: " + fromAlarmStatus(newStatus) + ", source: " + source);

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
            unsigned long time = millis();
            unsigned long nextInformationTime = pinTimes[pinIndex] + PIN_INFORM_INTERVAL;
            bool lastActivation = activations[pinIndex];
            int currentValue = -1;

            DPinInput realPinInputType = getPinInputType(pin.id);

            if((pin.input  == PIN_ANALOG) && (realPinInputType == PIN_ANALOG)) {
                currentValue = analogRead(pin.id);
            } else if((pin.input  == PIN_DIGITAL) && (realPinInputType == PIN_ANALOG)) {
                currentValue = analogRead(pin.id) > 1000;
            } else if (realPinInputType == PIN_DIGITAL){
                currentValue = digitalRead(pin.id  - 10);
            }

            bool currentActivation = isPinActivated(pin, currentValue);


            if((pinTimes[pinIndex] == 0) || (nextInformationTime < time)){
                info(ALARM_TAG, "Informing pin " + pinName + " has value of "  + String(currentValue));
                informValue = true;
                pinTimes[pinIndex] = time;
            }

            if (currentActivation != lastActivation) {
                info(ALARM_TAG, "Pin activation changed " + pinName + " "+  (currentActivation ? "enabled" : "disabled") + " to value: " + String(currentValue));
                informValue = true;
                pinActivatedChange(pin, currentActivation);
                activations[pinIndex] = currentActivation;
            }

            if (informValue){
                EventDispatcher::publishPinValue(pinName, currentValue);
            }
        }

        static DPinInput getPinInputType(PinIds pinId){
            if(pinId == PI_UNKNOWN){
                return PIN_UNKNOWN;
            }else if(pinId > 10){
                return PIN_DIGITAL;
            }else {
                return PIN_ANALOG;
            }
        }


        static bool isPinActivated(AlarmPin pin, int value){
            DPinInput realPinInputType = getPinInputType(pin.id);
            if((pin.input == PIN_DIGITAL) || ((pin.input == PIN_ANALOG) && (realPinInputType == PIN_DIGITAL))){
                return (value == pin.mode);
            }else{
                if(pin.mode == PM_HIGH){
                    return (value >= pin.threshold);
                }else{
                    return (value <= pin.threshold);
                }
            }
return false;
        }

        static void pinActivatedChange(AlarmPin pin, bool currentActivation) {
            AlarmStatus currentStatus = AlarmConfig::getStatus();

            if(currentActivation){
                if (pin.type == PT_SABOTAGE) {
                    warn(ALARM_TAG,"The node is under sabotage!");
                    setStatus(AS_SABOTAGE, "P:" + fromPinIds(pin.id));
                } else if (pin.type == PT_SAFETY) {
                    warn(ALARM_TAG,"The node is under safety!");
                    setStatus(AS_SAFETY, "P:" + fromPinIds(pin.id));
                } else if ((pin.type == PT_SENSOR) && (currentStatus == AS_ACTIVATED)) {
                    info(ALARM_TAG, "Sensor trigger!");
                    setStatus(AS_SUSPICIOUS, "P:" + fromPinIds(pin.id));
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
                setStatus(AS_IDLE, "P:" + fromPinIds(source));
            } else {
                setStatus(AS_ACTIVATING, "P:" + fromPinIds(source));
            }
        }

        static void setPinInput(int index, AlarmPin pin) {
            debug(ALARM_TAG,"Pin " + fromPinIds(pin.id) + " is type " + fromPinType(pin.type) + " and mode " + fromPinMode(pin.mode));
            pinMode(pin.id, INPUT);
        }
    };

    bool Alarm::activations[PIN_NUMBER];
    unsigned long Alarm::pinTimes[PIN_NUMBER];
    unsigned long Alarm::statusTime = millis();
}
#endif
