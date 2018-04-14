#ifndef NODE_DEFINITION_H
#define NODE_DEFINITION_H

#include "../common.h"

#define NODE_NAME                       "Cocina"
#define NODE_ID                         "ASDFGHJ"
#define VERSION                         "1.0"

#define NODE_MAC                        { 0xCB, 0xCB, 0xCB, 0xCB, 0xCB, 0xCB }
#define NODE_IP                         { 10, 10, 0, 70 }

#define ACTIVATING_TIME                 15000
#define SUSPICIOUS_TIME                 15000

#define PIN_INFORM_INTERVAL             5 * 60 * 1000
#define MASTER_PIN_NUMBER               10

#define ALARM_PIN                       9

//#define LED_RED_PIN                   LED_BUILTIN
#define LED_RED_PIN                     6
#define SPEAKER_PIN                     7

#endif
