#ifndef ALARM_ENGINE_H
#define ALARM_ENGINE_H

#include "../common.hpp"
#include "../events.hpp"
#include "config.hpp"

#define NO_SOURCE               "Unknown"


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
                #if(LOG_LEVEL<=WARN)
                    Serial.println("Alarm - Unknown status");
                #endif
                return AS_UNKNOWN;
            } else if ((newStatus == currentStatus) && !forceChange) {
                #if(LOG_LEVEL<=WARN)
                    Serial.print("Alarm - Unchanged status: ");
                    Serial.println(fromAlarmStatus(newStatus));
                #endif
                return AS_UNCHANGED;
            } else {
                AlarmConfig::setStatus(newStatus, source.c_str());
                #if(LOG_LEVEL<=INFO)
                    Serial.print("Alarm - Changed status to: ");
                    Serial.print(fromAlarmStatus(newStatus));
                    Serial.print(", source: ");
                    Serial.println(source);
                #endif

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
                #if(LOG_LEVEL<=DEBUG)
                    Serial.print("Alarm - Informing pin ");
                    Serial.print(pinName);
                    Serial.print(" has value of ");
                    Serial.println(currentValue);
                #endif
                informValue = true;
                pinTimes[pinIndex] = time;
            }

            if (currentActivation != lastActivation) {
                String change = (currentActivation ? "enabled" : "disabled");
                #if(LOG_LEVEL<=DEBUG)
                    Serial.print("Alarm - Pin activation changed ");
                    Serial.print(pinName);
                    Serial.print(" ");
                    Serial.print(change);
                    Serial.print(" to value: ");
                    Serial.println(currentValue);
                #endif
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
                    #if(LOG_LEVEL<=WARN)
                        Serial.println("Alarm - The node is under sabotage!");
                    #endif
                    setStatus(AS_SABOTAGE, "P:" + fromPinIds(pin.id));
                } else if (pin.type == PT_SAFETY) {
                    #if(LOG_LEVEL<=WARN)
                        Serial.println("Alarm - The node is under safety!");
                    #endif
                    setStatus(AS_SAFETY, "P:" + fromPinIds(pin.id));
                } else if ((pin.type == PT_SENSOR) && (currentStatus == AS_ACTIVATED)) {
                    #if(LOG_LEVEL<=DEBUG)
                        Serial.println("Alarm - Sensor trigger!");
                    #endif
                    setStatus(AS_SUSPICIOUS, "P:" + fromPinIds(pin.id));
                } else if (pin.type == PT_KEY) {
                    if (currentStatus != AS_SABOTAGE) {
                        key(pin.id);
                    }
                }
            }
        }

      static void key(PinIds source) {
            #if(LOG_LEVEL<=DEBUG)
                Serial.println("Alarm - KEY trigger!");
            #endif
            if (AlarmConfig::getStatus() != AS_IDLE) {
                setStatus(AS_IDLE, "P:" + fromPinIds(source));
            } else {
                setStatus(AS_ACTIVATING, "P:" + fromPinIds(source));
            }
        }

        static void setPinInput(int index, AlarmPin pin) {
            #if(LOG_LEVEL<=DEBUG)
                Serial.print("Alarm - Pin ");
                Serial.print(fromPinIds(pin.id));
                Serial.print(" is type ");
                Serial.print(fromPinType(pin.type));
                Serial.print("  and mode ");
                Serial.println(fromPinMode(pin.mode));
            #endif
            pinMode(pin.id, INPUT);
        }
    };

    bool Alarm::activations[PIN_NUMBER];
    unsigned long Alarm::pinTimes[PIN_NUMBER];
    unsigned long Alarm::statusTime = millis();
#endif
