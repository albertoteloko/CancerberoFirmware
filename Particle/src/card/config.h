#ifndef CARD_CONFIG_H
#define CARD_CONFIG_H

#include "../common.h"
#include "../utils.h"

#define CARD_CONFIG_EEPROM_ADDRESS SERVER_DEFINITION_EEPROM_ADDRESS +  sizeof(EthernetGatewayConfigStruct) + 10
#define CARD_NUMBER 20
#define CARD_ID_BYTES 7

#define NO_ID { 0,0,0,0,0,0,0};

namespace {

    String fromCardId(byte mac[CARD_ID_BYTES]) {
        char hex_digit;
        String result = "";
        for (int i = 0; i < CARD_ID_BYTES; ++i) {
            if (i != 0) {
                result += ":";
            }
            hex_digit = 48 + (mac[i] >> 4);
            if (57 < hex_digit) {
                hex_digit += 39;
            }
            result = result + hex_digit;
            hex_digit = 48 + (mac[i] & 0xf);
            if (57 < hex_digit) {
                hex_digit += 39;
            }
            result = result + hex_digit;
        }
        return result.toUpperCase();
    }

    bool toCardId(String value, byte input[CARD_ID_BYTES]) {
        for (int i = 0; i < CARD_ID_BYTES; ++i) {
            byte b1 = convertCharToHex(value.charAt(3 * i));
            byte b2 = convertCharToHex(value.charAt(3 * i + 1));

            if (b1 > 16) {
                return false;
            }

            if (b2 > 16) {
                return false;
            }

            input[i] = b1 << 4 | b2;
        }
        return true;
    }

    struct CardConfigStruct {
        bool enabled = false;
        bool setupMode = false;
        SPIPort spi = SPI_UNKNOWN;
        PinIds ssPin = PI_UNKNOWN;
        byte masterId[CARD_ID_BYTES] = NO_ID;
        byte cards[CARD_NUMBER][CARD_ID_BYTES];
    };

    class CardConfig {
    public:
        static RemoteLog log;
        static bool enabled;
        static bool setupMode;
        static SPIPort spi;
        static PinIds ssPin;
        static byte masterId[CARD_ID_BYTES];
        static byte cards[CARD_NUMBER][CARD_ID_BYTES];

        static void set(SPIPort newSpi, PinIds newSSPin, byte newMasterId[CARD_ID_BYTES]) {
            enabled = true;
            setupMode = false;
            spi = newSpi;
            ssPin = newSSPin;

            for (int i = 0; i < CARD_ID_BYTES; ++i) {
                masterId[i] = newMasterId[i];
            }

            save();
        }

        static void clear() {
            CardConfigStruct tmp = CardConfigStruct();
            EEPROM.put(CARD_CONFIG_EEPROM_ADDRESS, tmp);
            loadFromConfig(tmp);
        }

        static void save() {
            CardConfigStruct tmp = CardConfigStruct();

            for (int i = 0; i < CARD_ID_BYTES; ++i) {
                tmp.masterId[i] = masterId[i];
            }

            tmp.enabled = enabled;
            tmp.setupMode = setupMode;
            tmp.spi = spi;
            tmp.ssPin = ssPin;

            for (int j = 0; j < CARD_NUMBER; ++j) {
                for (int i = 0; i < CARD_ID_BYTES; ++i) {
                    tmp.cards[j][i] = cards[j][i];
                }
            }

            EEPROM.put(CARD_CONFIG_EEPROM_ADDRESS, tmp);
        }

        static void load() {
            CardConfigStruct tmp = CardConfigStruct();
            EEPROM.get(CARD_CONFIG_EEPROM_ADDRESS, tmp);
            loadFromConfig(tmp);
        }

    private:
        static void loadFromConfig(CardConfigStruct config) {
            for (int i = 0; i < CARD_ID_BYTES; ++i) {
                masterId[i] = config.masterId[i];
            }

            enabled = config.enabled;
            setupMode = config.setupMode;
            spi = config.spi;
            ssPin = config.ssPin;

            for (int j = 0; j < CARD_NUMBER; ++j) {
                for (int i = 0; i < CARD_ID_BYTES; ++i) {
                    cards[j][i] = config.cards[j][i];
                }
            }
        }
    };

    bool CardConfig::enabled = false;
    bool CardConfig::setupMode = false;
    SPIPort CardConfig::spi = SPI_UNKNOWN;
    PinIds CardConfig::ssPin = PI_UNKNOWN;
    byte CardConfig::masterId[CARD_ID_BYTES] = NO_ID;
    byte CardConfig::cards[CARD_NUMBER][CARD_ID_BYTES];
}
#endif
