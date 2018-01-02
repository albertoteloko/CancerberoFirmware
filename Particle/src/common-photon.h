#ifndef COMMON_PHOTON_NODE_H
#define COMMON_PHOTON_NODE_H

#include "common.h"

#define MASTER_PIN_NUMBER 10

#define SPEAKER_PIN         D0
#define SABOTAGE_OUT_PIN    D1
#define ALARM_PIN           D6
#define LED_PIN             D7

enum SPIPort {
    SPI_0,
    SPI_1,
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
    PI_A5 = A5
};

namespace {
    static SPIClass getSPI(SPIPort input){
        switch (input){
            case SPI_0:
                return SPI;
            case SPI_1:
                return SPI1;
            default:
                return SPI;
        }
    }

    SPIPort toSPIPort(String input) {
        if (compareIgnoringCase(input, "SPI0")) {
            return SPI_0;
        } else if (compareIgnoringCase(input, "SPI1")) {
            return SPI_1;
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