#ifndef COMMON_UNO_H
#define COMMON_UNO_H


#define PIN_INFORM_INTERVAL     5 * 60 * 1000
#define MASTER_PIN_NUMBER       10

#define ALARM_PIN               9

#define LED_RED_PIN             6
#define SPEAKER_PIN             7
#define ADDRESS_SELECT_PIN      8


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

namespace {
    PinIds toPinIds(String input) {
        if (input.equalsIgnoreCase("D2")) {
            return PI_D2;
        } else if (input.equalsIgnoreCase("D3")) {
            return PI_D3;
        } else if (input.equalsIgnoreCase("D4")) {
            return PI_D4;
        } else if (input.equalsIgnoreCase("D5")) {
            return PI_D5;
        } else if (input.equalsIgnoreCase("A0")) {
            return PI_A0;
        } else if (input.equalsIgnoreCase("A1")) {
            return PI_A1;
        } else if (input.equalsIgnoreCase("A2")) {
            return PI_A2;
        } else if (input.equalsIgnoreCase("A3")) {
            return PI_A3;
        } else if (input.equalsIgnoreCase("A4")) {
            return PI_A4;
        } else if (input.equalsIgnoreCase("A5")) {
            return PI_A5;
        } else {
            return PI_UNKNOWN;
        }
    }

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
}
#endif