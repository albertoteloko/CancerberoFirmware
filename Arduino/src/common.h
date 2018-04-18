#ifndef COMMON_H
#define COMMON_H

// Arduino versioning.
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// See http://code.google.com/p/arduino/issues/detail?id=246
#if defined(ARDUINO) && ARDUINO < 101
#define INPUT_PULLUP INPUT
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
    PT_SABOTAGE = 0x03,
    PT_SAFETY = 0x04
};

enum DPinMode {
    PM_UNKNOWN = -0x01,
    PM_HIGH = 1,
    PM_LOW = 0
};

enum DPinInput {
    PIN_UNKNOWN = -0x01,
    PIN_DIGITAL = 0x01,
    PIN_ANALOG = 0x02
};

enum PinIds {
    PI_UNKNOWN = -0x01,
    PI_A0 = 0,
    PI_A1 = 1,
    PI_A2 = 2,
    PI_A3 = 3,
    PI_A4 = 4,
    PI_A5 = 5,
    PI_D2 = 12,
    PI_D3 = 13,
    PI_D4 = 14,
    PI_D5 = 15
};

struct AlarmPin {
    PinIds id = PI_UNKNOWN;
    PinType type = PT_UNKNOWN;
    DPinMode mode = PM_UNKNOWN;
    DPinInput input = PIN_UNKNOWN;
    int threshold;

    AlarmPin(PinIds _id, PinType _type, DPinMode _mode, DPinInput _input, int _threshold){
        id =_id;
        type =_type;
        mode =_mode;
        input =_input;
        threshold =_threshold;
    }
};


namespace {
    String fromPinIds(PinIds input) {
        switch (input) {
            case PI_D2:
                return "D2";
            case PI_D3:
                return "D3";
            case PI_D4:
                return "D4";
            case PI_D5:
                return "D5";
            case PI_A0:
                return "A0";
            case PI_A1:
                return "A1";
            case PI_A2:
                return "A2";
            case PI_A3:
                return "A3";
            case PI_A4:
                return "A4";
            case PI_A5:
                return "A5";
            default:
                return "UNKNOWN";
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
        if (input.equalsIgnoreCase("IDLE")) {
            return AS_IDLE;
        } else if (input.equalsIgnoreCase("ACTIVATING")) {
            return AS_ACTIVATING;
        } else if (input.equalsIgnoreCase("ACTIVATED")) {
            return AS_ACTIVATED;
        } else if (input.equalsIgnoreCase("SUSPICIOUS")) {
            return AS_SUSPICIOUS;
        } else if (input.equalsIgnoreCase("ALARMED")) {
            return AS_ALARMED;
        } else if (input.equalsIgnoreCase("SAFETY")) {
            return AS_SAFETY;
        } else if (input.equalsIgnoreCase("SABOTAGE")) {
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


    String fromPinType(PinType input) {
        switch (input) {
            case PT_SENSOR:
                return "SENSOR";
            case PT_KEY:
                return "KEY";
            case PT_SABOTAGE:
                return "SABOTAGE";
            case PT_SAFETY:
                return "SAFETY";
            default:
                return "UNKNOWN";
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


