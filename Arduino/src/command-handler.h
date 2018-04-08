#ifndef COMMAND_FACADE_H
#define COMMAND_FACADE_H

#include "common/common.h"
#include "log.h"
#include <EEPROM.h>
//#include "alarm/config.h"
//#include "alarm/engine.h"

#define COMMAND_NOT_FOUND               -404


const String COMMAND_TAG = "Command";

namespace {

    class CommandHandler {
    public:
        static int processInput(String message) {
            Logger::info(COMMAND_TAG, "Incoming command: " + message);
            if(message.startsWith("clear#")){
                return clear();
            } else if(message.startsWith("L.level#")){
                return Logger::setLevel(getArgs(message));
            }else {
                Logger::error(COMMAND_TAG, "Unknown command: " + message);
                return COMMAND_NOT_FOUND;
            }

//                       Particle.function("A.status", &setAlarmStatus);
//                        Particle.function("A.enable", &enableAlarm);
//                        Particle.function("A.disable", &disableAlarm);
        }

    private:
//        static AlarmPin NO_PIN;

        static int clear(){
            Logger::info(COMMAND_TAG, "Clear node");
            for (int i = 0 ; i < EEPROM.length() ; i++) {
                EEPROM.write(i, 0);
            }
//            disableAlarm(input);
            return true;
        }

        static String getArgs(String input){
            return input.substring(input.indexOf("#") + 1);
        }
    };

//    AlarmPin CommandHandler::NO_PIN = AlarmPin();
}
#endif
