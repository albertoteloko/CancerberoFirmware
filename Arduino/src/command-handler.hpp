#ifndef COMMAND_FACADE_H
#define COMMAND_FACADE_H

#include "common.hpp"
#include "alarm/config.hpp"
#include "alarm/engine.hpp"

#define COMMAND_NOT_FOUND               -404


    class CommandHandler {
    public:
        static int processInput(String message) {
            #if(LOG_LEVEL<=DEBUG)
                Serial.print("Command - Incoming command: ");
                Serial.println(message);
            #endif
            if(message.startsWith("A.status#")){
                return setAlarmStatus(getArgs(message));
            }else {
                #if(LOG_LEVEL<=ERROR)
                    Serial.print("Command - Unknown command: ");
                    Serial.println(message);
                #endif
                return COMMAND_NOT_FOUND;
            }
        }

    private:

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
                #if(LOG_LEVEL<=ERROR)
                    Serial.print("Command - Unknown new master status: ");
                    Serial.println(input);
                #endif
                return NS_UNKNOWN;
            }
        }

        static String getArgs(String input){
            return input.substring(input.indexOf("#") + 1);
        }
    };
#endif
