#ifndef NODE_DEFINITION_H
#define NODE_DEFINITION_H

#include "../common.h"

#define NODE_NAME                       "Cocina"
#define NODE_ID                         "39fdce3d-c97f-4513-b498-f62be9b1d772"
#define VERSION                         "1.0"

#define NODE_MAC                        { 0xCB, 0xCB, 0xCB, 0xCB, 0xCB, 0xCB }
#define NODE_IP                         { 10, 10, 0, 70 }
#define NODE_PORT                       5555
#define MASTER_IP                       { 10, 10, 0, 2 }
#define MASTER_PORT                     5555

#define ACTIVATING_TIME                 15000
#define SUSPICIOUS_TIME                 15000

#define PIN_INFORM_INTERVAL             300000

#define ALARM_PIN                       9

#define LED_RED_PIN                     6
#define SPEAKER_PIN                     7

static const AlarmPin PINS [] =         {
    AlarmPin(PI_A0,PT_KEY,PM_HIGH,PIN_ANALOG,510)
};

#endif
