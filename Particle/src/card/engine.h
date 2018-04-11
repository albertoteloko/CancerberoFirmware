#ifndef CARD_ENGINE_H
#define CARD_ENGINE_H

#include "../log.h"
#include "../events.h"
#include "../common.h"
#include "config.h"
#include "../alarm/engine.h"
#include "../node/engine.h"

#include <SPI.h>
#include <MFRC522.h>

#define UNUSED_PIN  UINT8_MAX

namespace {

    enum BeepMode {
        BM_NODE,
        BM_GRANTED,
        BM_NOT_GRANTED
    };

    class CardConfig;

    class Card {

    public:
        static void enable(SPIPort spi, PinIds ssPin) {
            CardConfig::set(spi, ssPin);
            setup();
        }

        static void disable() {
            CardConfig::clear();
            setup();
        }

        static void start() {
            CardConfig::load();
            setup();
        }

        static void stop() {
            CardConfig::save();
        }

        static void setup() {
            if (CardConfig::enabled()) {
                if (CardConfig::spi() != SPI_UNKNOWN) {
                    mfrc522.PCD_End();

                    log.info("Starting card");
                    log.info("SPI: " + fromSPIPort(CardConfig::spi()));
                    log.info("SS Pin: " + fromPinIds(CardConfig::ssPin()));

                    mfrc522.PCD_Init(CardConfig::ssPin(), UNUSED_PIN, getSPI(CardConfig::spi()));
                    showReaderDetails();
                }
            } else {
                log.debug("Card is disabled");
            }
        }

        static BeepMode loop() {
            BeepMode result= BM_NODE;

            if (CardConfig::enabled()) {
                bool successRead = getID();  // sets successRead to 1 when we get read from reader otherwise 0

                if (successRead) {
                    String id = fromCardId(readCard);
                    if (CardConfig::contains(readCard)) {
                        log.debug("Read valid card id: " + id);
                        result = BM_GRANTED;
                        Alarm::key("C:" + id);
                    } else {
                        log.warn("Read invalid card id: " + id);
                        result = BM_NOT_GRANTED;
                    }
                } else {
                    log.trace("No ID :(");
                }
            }

            return result;
        }

    private:
        static RemoteLog log;
        static MFRC522 mfrc522;
        static byte readCard[CARD_ID_BYTES];

        static void showReaderDetails() {
            // Get the MFRC522 software version
            byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);

            if ((v == 0x00) || (v == 0xFF)) {
                log.error("Communication failure, is the MFRC522 properly connected?");
            } else if (v == 0x91) {
                log.info("MFRC522 Software Version: v1.0");
            } else if (v == 0x92) {
                log.info("MFRC522 Software Version: v2.0");
            }
        }

        static bool getID() {
            if (!mfrc522.PICC_IsNewCardPresent()) {
                return false;
            }
            if (!mfrc522.PICC_ReadCardSerial()) {
                return false;
            }

            for (uint8_t i = 0; i < 10; i++) {
                readCard[i] = 0;
            }
            for (uint8_t i = 0; i < mfrc522.uid.size; i++) {  //
                readCard[i] = mfrc522.uid.uidByte[i];
            }
            mfrc522.PICC_HaltA();
            return true;
        }
    };

    RemoteLog Card::log = RemoteLog("card");
    MFRC522 Card::mfrc522 = MFRC522();
    byte Card::readCard[CARD_ID_BYTES];
}
#endif
