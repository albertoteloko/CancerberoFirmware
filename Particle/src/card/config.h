#ifndef CARD_CONFIG_H
#define CARD_CONFIG_H

#include "../common.h"
#include "../utils.h"

#define CARD_CONFIG_EEPROM_ADDRESS SERVER_DEFINITION_EEPROM_ADDRESS +  sizeof(EthernetGatewayConfigStruct) + 10
#define CARD_ID_BYTES 10

#define NOT_FOUND -5
#define NOT_SPACE_LEFT -10
#define ALREADY_ADDED -15

static byte NO_ID[CARD_ID_BYTES] = {0, 0, 0, 0, 0, 0, 0};

namespace {

    String fromCardId(byte mac[CARD_ID_BYTES]) {
        char hex_digit;
        String result = "";

        int bytes = CARD_ID_BYTES;

        for (int i = CARD_ID_BYTES - 1; i >= 0; i--) {
            if (mac[i] != 0) {
                break;
            } else {
                bytes--;
            }
        }
        for (int i = 0; i < bytes; ++i) {
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
        SPIPort spi = SPI_UNKNOWN;
        PinIds ssPin = PI_UNKNOWN;
        byte cards[CARD_NUMBER][CARD_ID_BYTES];
    };

    class CardConfig {
    public:
        static CardConfigStruct config;

        static bool enabled() {
            return config.enabled;
        }

        static SPIPort spi() {
            return config.spi;
        }

        static PinIds ssPin() {
            return config.ssPin;
        }

        static void set(SPIPort newSpi, PinIds newSSPin) {
            config.enabled = true;
            config.spi = newSpi;
            config.ssPin = newSSPin;

            save();
        }


        static int add(byte cardId[CARD_ID_BYTES]) {
            int result = ALREADY_ADDED;

            if (!contains(cardId)) {
                result = NOT_SPACE_LEFT;

                for (int i = 0; i < CARD_NUMBER; ++i) {
                    byte* item= config.cards[i];
                    if (empty(item)) {
                        copy(cardId, item);
                        result = i;
                        save();
                        break;
                    }
                }
            }

            return result;
        }

        static int remove(byte cardId[CARD_ID_BYTES]) {
            int result = NOT_FOUND;

            for (int i = 0; i < CARD_NUMBER; ++i) {
                byte* item = config.cards[i];
                if (equals(cardId, item)) {
                    copy(NO_ID, item);
                    result = i;
                    save();
                    break;
                }
            }

            return result;
        }

        static bool contains(byte cardId[CARD_ID_BYTES]) {
            bool result = false;

            for (int i = 0; i < CARD_NUMBER; ++i) {
                if (equals(cardId, config.cards[i])) {
                    result = true;
                    break;
                }
            }

            return result;
        }

        static void clear() {
            config = CardConfigStruct();
            save();
            load();
        }

        static void save() {
            EEPROM.put(CARD_CONFIG_EEPROM_ADDRESS, config);
        }

        static void load() {
            EEPROM.get(CARD_CONFIG_EEPROM_ADDRESS, config);
        }

    private:
        static bool empty(byte cardId1[CARD_ID_BYTES]) {
            return equals(cardId1, NO_ID);
        }

        static bool equals(byte cardId1[CARD_ID_BYTES], byte cardId2[CARD_ID_BYTES]) {
            bool result = true;

            for (int i = 0; i < CARD_ID_BYTES; ++i) {
                if (cardId1[i] != cardId2[i]) {
                    result = false;
                    break;
                }
            }

            return result;
        }

        static bool copy(byte src[CARD_ID_BYTES], byte dst[CARD_ID_BYTES]) {
            bool result = true;

            for (int i = 0; i < CARD_ID_BYTES; ++i) {
                dst[i] = src[i];
            }

            return result;
        }
    };

    CardConfigStruct CardConfig::config = CardConfigStruct();
}
#endif
