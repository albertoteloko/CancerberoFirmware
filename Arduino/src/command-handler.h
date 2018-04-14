#ifndef COMMAND_FACADE_H
#define COMMAND_FACADE_H

#include "common.h"
#include "log.h"
#include "alarm/config.h"
#include "alarm/engine.h"

#define COMMAND_NOT_FOUND               -404


#define COMMAND_TAG                     "Command"

namespace {

    class CommandHandler {
    public:
        static int processInput(String message) {
            info(COMMAND_TAG, "Incoming command: " + message);
            if(message.startsWith("A.status#")){
                return setAlarmStatus(getArgs(message));
            }else {
                error(COMMAND_TAG, "Unknown command: " + message);
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
                debug(COMMAND_TAG, "Unknown new master status: " + input);
                return NS_UNKNOWN;
            }
        }


        static String getArgs(String input){
            return input.substring(input.indexOf("#") + 1);
        }
    };
}
#endif
