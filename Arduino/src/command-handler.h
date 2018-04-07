#ifndef COMMAND_FACADE_H
#define COMMAND_FACADE_H

#include "common/common.h"
#include "log.h"
//#include "alarm/config.h"
//#include "alarm/engine.h"



const String COMMAND_TAG = "Command";

namespace {

    class CommandHandler {
    public:
         static int processInput(String message) {
         Logger::info(COMMAND_TAG, "Hello!");
            return 42;
        }

    private:
//        static AlarmPin NO_PIN;
    };

//    AlarmPin CommandHandler::NO_PIN = AlarmPin();
}
#endif
