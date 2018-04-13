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
            } else if(message.startsWith("A.disable#")){
                return disableAlarm();
            }else {
//                error(COMMAND_TAG, "Unknown command: %s", message.c_str());
                return COMMAND_NOT_FOUND;
            }

//                       Particle.function("A.status", &setAlarmStatus);
//                        Particle.function("A.enable", &enableAlarm);
//                        Particle.function("A.disable", &disableAlarm);
        }

    private:
//        static AlarmPin NO_PIN;

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

        static String getArgs(String input){
            return input.substring(input.indexOf("#") + 1);
        }
    };

//    AlarmPin CommandHandler::NO_PIN = AlarmPin();
}
#endif
