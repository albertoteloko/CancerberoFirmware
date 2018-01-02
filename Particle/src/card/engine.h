#ifndef CARD_ENGINE_H
#define CARD_ENGINE_H

#include "../log.h"
#include "../events.h"
#include "../common.h"
#include "config.h"
#include "../alarm/engine.h"

#include <SPI.h>

namespace {

    class CardConfig;

    class Card {

    public:
        static void enable(SPIPort spi, PinIds ssPin, byte masterId[7]) {
            CardConfig::set(spi, ssPin, masterId);
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

        static int toggleSetup(){
            return false;
        }

        static int disableSetup(){
            return false;
        }

        static void setup() {
            if (CardConfig::enabled) {
                if (CardConfig::spi != SPI_UNKNOWN) {
//                    server.end();
//                    W5100.end();

                    log.info("Starting card");
                    log.info("SPI: " + fromSPIPort(CardConfig::spi));
                    log.info("SS Pin: " + fromPinIds(CardConfig::ssPin));
                    log.info("Master ID: " + fromCardId(CardConfig::masterId));

//                    server.setPort(EthernetGatewayConfig::port);
//                    W5100.spi(getSPI(EthernetGatewayConfig::spi), EthernetGatewayConfig::ssPin);
//                    Ethernet.begin(EthernetGatewayConfig::mac, EthernetGatewayConfig::ip);
//                    server.begin();
                }
            } else {
                log.info("Card is disabled");
            }
        }

        static void loop() {
            if (CardConfig::enabled) {
            }
        }

    private:
        static RemoteLog log;
    };

    RemoteLog Card::log = RemoteLog("card");
}
#endif
