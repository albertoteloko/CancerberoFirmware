#ifndef COMMON_PHOTON_NODE_H
#define COMMON_PHOTON_NODE_H

#include "common.h"

#define MASTER_PIN_NUMBER       12
#define CARD_NUMBER             20

#define SABOTAGE_OUT_PIN        D6
#define ALARM_PIN               D7

#define SPEAKER_PIN             D1
#define LED_RED_PIN             D0
#define LED_YELLOW_PIN          C5
#define LED_GREEN_PIN           C4
enum SPIPort {
    SPI_0,
    SPI_1,
    SPI_2,
    SPI_UNKNOWN
};

enum PinIds {
    PI_UNKNOWN = -0x01,
    PI_D2 = D2,
    PI_D3 = D3,
    PI_D4 = D4,
    PI_D5 = D5,
    PI_A0 = A0,
    PI_A1 = A1,
    PI_A2 = A2,
    PI_A3 = A3,
    PI_A4 = A4,
    PI_A5 = A5,
    PI_A6 = A6,
    PI_A7 = A7,
    PI_B0 = B0,
    PI_B1 = B1,
    PI_B2 = B2,
    PI_B3 = B3,
    PI_B4 = B4,
    PI_B5 = B5,
    PI_C0 = C0,
    PI_C1 = C1,
    PI_C2 = C2,
    PI_C3 = C3,
    PI_C4 = C4,
    PI_C5 = C5
};

namespace {
    static SPIClass getSPI(SPIPort input) {
        switch (input) {
            case SPI_0:
                return SPI;
            case SPI_1:
                return SPI1;
            case SPI_2:
                return SPI2;
            default:
                return SPI;
        }
    }

    SPIPort toSPIPort(String input) {
        if (compareIgnoringCase(input, "SPI0")) {
            return SPI_0;
        } else if (compareIgnoringCase(input, "SPI1")) {
            return SPI_1;
        } else if (compareIgnoringCase(input, "SPI2")) {
            return SPI_2;
        } else {
            return SPI_UNKNOWN;
        }
    }


    String fromSPIPort(SPIPort input) {
        switch (input) {
            case SPI_0:
                return "SPI0";
            case SPI_1:
                return "SPI1";
            case SPI_2:
                return "SPI2";
            default:
                return "Unknown";
        }
    }

    PinIds toPinIds(String input) {
        if (compareIgnoringCase(input, "D2")) {
            return PI_D2;
        } else if (compareIgnoringCase(input, "D3")) {
            return PI_D3;
        } else if (compareIgnoringCase(input, "D4")) {
            return PI_D4;
        } else if (compareIgnoringCase(input, "D5")) {
            return PI_D5;
        } else if (compareIgnoringCase(input, "A0")) {
            return PI_A0;
        } else if (compareIgnoringCase(input, "A1")) {
            return PI_A1;
        } else if (compareIgnoringCase(input, "A2")) {
            return PI_A2;
        } else if (compareIgnoringCase(input, "A3")) {
            return PI_A3;
        } else if (compareIgnoringCase(input, "A4")) {
            return PI_A4;
        } else if (compareIgnoringCase(input, "A5")) {
            return PI_A5;
        } else if (compareIgnoringCase(input, "A6")) {
            return PI_A6;
        } else if (compareIgnoringCase(input, "A7")) {
            return PI_A7;
        } else if (compareIgnoringCase(input, "B0")) {
            return PI_B0;
        } else if (compareIgnoringCase(input, "B1")) {
            return PI_B1;
        } else if (compareIgnoringCase(input, "B2")) {
            return PI_B2;
        } else if (compareIgnoringCase(input, "B3")) {
            return PI_B3;
        } else if (compareIgnoringCase(input, "B4")) {
            return PI_B4;
        } else if (compareIgnoringCase(input, "B5")) {
            return PI_B5;
        } else if (compareIgnoringCase(input, "C0")) {
            return PI_C0;
        } else if (compareIgnoringCase(input, "C1")) {
            return PI_C1;
        } else if (compareIgnoringCase(input, "C2")) {
            return PI_C2;
        } else if (compareIgnoringCase(input, "C3")) {
            return PI_C3;
        } else if (compareIgnoringCase(input, "C4")) {
            return PI_C4;
        } else if (compareIgnoringCase(input, "C5")) {
            return PI_C5;
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
            case PI_A6:
                return "A6";
            case PI_A7:
                return "A7";
            case PI_B0:
                return "B0";
            case PI_B1:
                return "B1";
            case PI_B2:
                return "B2";
            case PI_B3:
                return "B3";
            case PI_B4:
                return "B4";
            case PI_B5:
                return "B5";
            case PI_C0:
                return "C0";
            case PI_C1:
                return "C1";
            case PI_C2:
                return "C2";
            case PI_C3:
                return "C3";
            case PI_C4:
                return "C4";
            case PI_C5:
                return "C5";
            default:
                return "UNKNOWN";
        }
    }
}
#endif