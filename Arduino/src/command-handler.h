#ifndef COMMAND_FACADE_H
#define COMMAND_FACADE_H

#include "common/common.h"
#include "log.h"
#include <EEPROM.h>
#include "alarm/config.h"
#include "alarm/engine.h"

#define COMMAND_NOT_FOUND               -404


#define COMMAND_TAG                     "Command"

namespace {

    class CommandHandler {
    public:
        static int processInput(String message) {
            info(COMMAND_TAG, "Incoming command: %s", message.c_str());
            if(message.startsWith("clear#")){
                return clear();
            } else if(message.startsWith("A.status#")){
                return setAlarmStatus(getArgs(message));
            } else if(message.startsWith("A.enabled#")){
                return enableAlarm(getArgs(message));
            } else if(message.startsWith("A.disable#")){
                return disableAlarm();
            }else {
                error(COMMAND_TAG, "Unknown command: %s", message.c_str());
                return COMMAND_NOT_FOUND;
            }
        }

    private:
        static AlarmPin NO_PIN;

        static int clear(){
            info(COMMAND_TAG, "Clear node");
            for (int i = 0 ; i < EEPROM.length() ; i++) {
                EEPROM.write(i, 0);
            }
            disableAlarm();
            return true;
        }

        static int setAlarmStatus(String input){
            String value = input;
            String user = NO_SOURCE;

            if (value.indexOf(",") > -1) {
                value = input.substring(0, input.indexOf(","));
                user = input.substring(input.indexOf(",") + 1);
            }
            AlarmStatus status = toAlarmStatus(value);

            if (status != AS_UNKNOWN) {
                return Alarm::setStatus(status, user);
            } else {
                debug(COMMAND_TAG, "Unknown new master status: %s", input.c_str());
                return NS_UNKNOWN;
            }
        }

        static int disableAlarm() {
            Alarm::disable();
            return true;
        }

        static int enableAlarm(String input) {
            AlarmPin pins[MASTER_PIN_NUMBER];

            if (!input.endsWith(",")) {
                input += ",";
            }

            int start = 0;
            int end = 0;

            int node = 0;
            int index = 0;

            long activatingTime = DEFAULT_ACTIVATING_TIME;
            long suspiciousTime = DEFAULT_SUSPICIOUS_TIME;

            while ((end = input.indexOf(",", start)) > -1) {
                String part = input.substring(start, end);
                info(COMMAND_TAG, "Part: %s", part.c_str());
                if (index == 0) {
                    int input = atoi(part.c_str());

                    if (input > 0) {
                        activatingTime = input;
                    } else {
                        info(COMMAND_TAG, "Invalid activating time: %i", input);
                        return false;
                    }
                } else if (index == 1) {
                    int input = atoi(part.c_str());

                    if (input > 0) {
                        suspiciousTime = input;
                    } else {
                        info(COMMAND_TAG, "Invalid suspicious time:  %i", input);
                        return false;
                    }
                } else {
                    AlarmPin pin = loadAlarmPin(part);

                    if (pin.id != PI_UNKNOWN) {
                        if (node++ == MASTER_PIN_NUMBER) {
                            error(COMMAND_TAG, "Too much pin, max number are:  %i", MASTER_PIN_NUMBER);
                            return false;
                        } else {
                            pins[node] = pin;
                        }
                    } else {
                        error(COMMAND_TAG, "Unknown pin definition: '%s'", part.c_str());
                        return false;
                    }
                }
                index += 1;
                start = end + 1;
            }

            Alarm::enable(activatingTime, suspiciousTime, pins);
            return true;
        }

        static const AlarmPin loadAlarmPin(String pinDefinition) {
            info(COMMAND_TAG, "Pin input: %s", pinDefinition.c_str());

            if(pinDefinition.length() != 6){
                error(COMMAND_TAG, "Invalid pin Definition");
                return NO_PIN;
            }

            PinIds id = toPinIds(fromPinIds(strToHex(pinDefinition.charAt(0))));

            if (id == PI_UNKNOWN) {
                error(COMMAND_TAG, "Unknown id: %s", pinDefinition.charAt(0));
                return NO_PIN;
            }

            PinType type = toPinType(fromPinType(strToHex(pinDefinition.charAt(1))));

            if (type == PT_UNKNOWN) {
                error(COMMAND_TAG, "Unknown type: %s" , pinDefinition.charAt(1));
                return NO_PIN;
            }

            DPinInput input = toPinInput(fromPinInput(strToHex(pinDefinition.charAt(2))));

            if (input == PIN_UNKNOWN) {
                error(COMMAND_TAG, "Unknown input: %s", pinDefinition.charAt(2));
                return NO_PIN;
            }


            DPinMode mode = toPinMode(fromPinMode(strToHex(strToHex(pinDefinition.charAt(3)))));

            if (mode == PM_UNKNOWN) {
                error(COMMAND_TAG, "Unknown pin mode: %s",  pinDefinition.charAt(3));
                return NO_PIN;
            }

            int threshold = strToHex(pinDefinition.charAt(4) + pinDefinition.charAt(5));

            if (threshold < 0) {
                error(COMMAND_TAG,"Unknown threshold: %i", threshold);
                return NO_PIN;
            }



            AlarmPin pin;

            pin.id = id;
            pin.type = type;
            pin.mode = mode;
            pin.input = input;
            pin.threshold = threshold;

            return pin;
        }

        static int strToHex(char str[]) {
          return (int) strtol(str, 0, 16);
        }

        static String getArgs(String input){
            return input.substring(input.indexOf("#") + 1);
        }
    };

    AlarmPin CommandHandler::NO_PIN = AlarmPin();
}
#endif
