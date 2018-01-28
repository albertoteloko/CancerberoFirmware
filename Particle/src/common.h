#ifndef COMMON_NODE_H
#define COMMON_NODE_H

#include <SPI.h>
#include <WString.h>
#include "utils.h"
// STATUS

#if PLATFORM_ID == 0 // Core
// your code
#elif PLATFORM_ID == 6 // Photon
#include "common-photon.h"
#elif PLATFORM_ID == 8 // P1
// your code
#elif PLATFORM_ID == 10 // Electron
#include "common-electron.h"
#else
#error "*** PLATFORM_ID not supported by this firmware. PLATFORM should be Core, Photon, P1 or Electron ***"
#endif

enum NodeStatus {
    NS_UNCHANGED = -0x02,
    NS_UNKNOWN = -0x01,
    NS_IDLE = 0x01,
    NS_ACTIVATING = 0x02,
    NS_ACTIVATED = 0x03,
    NS_SUSPICIOUS = 0x04,
    NS_ALARMED = 0x05,
    NS_SAFETY = 0x06,
    NS_SABOTAGE = 0x07
};

enum AlarmStatus {
    AS_UNCHANGED = -0x02,
    AS_UNKNOWN = -0x01,
    AS_IDLE = 0x01,
    AS_ACTIVATING = 0x02,
    AS_ACTIVATED = 0x03,
    AS_SUSPICIOUS = 0x04,
    AS_ALARMED = 0x05,
    AS_SAFETY = 0x06,
    AS_SABOTAGE = 0x07
};

enum PinType {
    PT_UNKNOWN = -0x01,
    PT_SENSOR = 0x01,
    PT_KEY = 0x02,
    PT_SABOTAGE_IN = 0x03,
    PT_SABOTAGE_OUT = 0x04,
    PT_SAFETY = 0x05
};

enum DPinMode {
    PM_UNKNOWN = -0x01,
    PM_HIGH = HIGH,
    PM_LOW = LOW
};

enum DPinInput {
    PIN_UNKNOWN = -0x01,
    PIN_DIGITAL = 0x01,
    PIN_ANALOG = 0x02
};

namespace {

    DPinInput toPinInput(String input) {
        if (compareIgnoringCase(input, "DIGITAL") || compareIgnoringCase(input, "D")) {
            return PIN_DIGITAL;
        } else if (compareIgnoringCase(input, "ANALOG") || compareIgnoringCase(input, "A")) {
            return PIN_ANALOG;
        } else {
            return PIN_UNKNOWN;
        }
    }

    String fromPinInput(DPinInput input) {
        switch (input) {
            case PIN_DIGITAL:
                return "DIGITAL";
            case PIN_ANALOG:
                return "ANALOG";
            default:
                return "UNKNOWN";
        }
    }

    AlarmStatus toAlarmStatus(String input) {
        if (compareIgnoringCase(input, "IDLE")) {
            return AS_IDLE;
        } else if (compareIgnoringCase(input, "ACTIVATING")) {
            return AS_ACTIVATING;
        } else if (compareIgnoringCase(input, "ACTIVATED")) {
            return AS_ACTIVATED;
        } else if (compareIgnoringCase(input, "SUSPICIOUS")) {
            return AS_SUSPICIOUS;
        } else if (compareIgnoringCase(input, "ALARMED")) {
            return AS_ALARMED;
        } else if (compareIgnoringCase(input, "SAFETY")) {
            return AS_SAFETY;
        } else if (compareIgnoringCase(input, "SABOTAGE")) {
            return AS_SABOTAGE;
        } else {
            return AS_UNKNOWN;
        }
    }

    String fromNodeStatus(NodeStatus input) {
        switch (input) {
            case NS_IDLE:
                return "IDLE";
            case NS_ACTIVATING:
                return "ACTIVATING";
            case NS_ACTIVATED:
                return "ACTIVATED";
            case NS_SUSPICIOUS:
                return "SUSPICIOUS";
            case NS_ALARMED:
                return "ALARMED";
            case NS_SAFETY:
                return "SAFETY";
            case NS_SABOTAGE:
                return "SABOTAGE";
            default:
                return "UNKNOWN";
        }
    }

    NodeStatus toNodeStatus(String input) {
        if (compareIgnoringCase(input, "IDLE")) {
            return NS_IDLE;
        } else if (compareIgnoringCase(input, "ACTIVATING")) {
            return NS_ACTIVATING;
        } else if (compareIgnoringCase(input, "ACTIVATED")) {
            return NS_ACTIVATED;
        } else if (compareIgnoringCase(input, "SUSPICIOUS")) {
            return NS_SUSPICIOUS;
        } else if (compareIgnoringCase(input, "ALARMED")) {
            return NS_ALARMED;
        } else if (compareIgnoringCase(input, "SAFETY")) {
            return NS_SAFETY;
        } else if (compareIgnoringCase(input, "SABOTAGE")) {
            return NS_SABOTAGE;
        } else {
            return NS_UNKNOWN;
        }
    }

    String fromAlarmStatus(AlarmStatus input) {
        switch (input) {
            case AS_IDLE:
                return "IDLE";
            case AS_ACTIVATING:
                return "ACTIVATING";
            case AS_ACTIVATED:
                return "ACTIVATED";
            case AS_SUSPICIOUS:
                return "SUSPICIOUS";
            case AS_ALARMED:
                return "ALARMED";
            case AS_SAFETY:
                return "SAFETY";
            case AS_SABOTAGE:
                return "SABOTAGE";
            default:
                return "UNKNOWN";
        }
    }

    PinType toPinType(String input) {
        if (compareIgnoringCase(input, "SENSOR") || compareIgnoringCase(input, "S")) {
            return PT_SENSOR;
        } else if (compareIgnoringCase(input, "KEY") || compareIgnoringCase(input, "K")) {
            return PT_KEY;
        } else if (compareIgnoringCase(input, "SABOTAGE_IN") || compareIgnoringCase(input, "SABI")) {
            return PT_SABOTAGE_IN;
        } else if (compareIgnoringCase(input, "SABOTAGE_OUT") || compareIgnoringCase(input, "SABO")) {
            return PT_SABOTAGE_OUT;
        } else if (compareIgnoringCase(input, "SAFETY") || compareIgnoringCase(input, "SAF")) {
            return PT_SAFETY;
        } else {
            return PT_UNKNOWN;
        }
    }


    String fromPinType(PinType input) {
        switch (input) {
            case PT_SENSOR:
                return "SENSOR";
            case PT_KEY:
                return "KEY";
            case PT_SABOTAGE_IN:
                return "SABOTAGE_IN";
            case PT_SABOTAGE_OUT:
                return "SABOTAGE_OUT";
            case PT_SAFETY:
                return "SAFETY";
            default:
                return "UNKNOWN";
        }
    }

    DPinMode toPinMode(String input) {
        if (compareIgnoringCase(input, "LOW") || compareIgnoringCase(input, "L")) {
            return PM_LOW;
        } else if (compareIgnoringCase(input, "HIGH") ||compareIgnoringCase(input, "H")) {
            return PM_HIGH;
        } else {
            return PM_UNKNOWN;
        }
    }

    String fromPinMode(DPinMode input) {
        switch (input) {
            case PM_LOW:
                return "LOW";
            case PM_HIGH:
                return "HIGH";
            default:
                return "UNKNOWN";
        }
    }
}
#endif