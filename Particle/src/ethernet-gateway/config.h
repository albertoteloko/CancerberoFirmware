#ifndef GATEWAY_CONFIG_H
#define GATEWAY_CONFIG_H

#include "../common.h"
#include "../utils.h"

#define SERVER_DEFINITION_EEPROM_ADDRESS sizeof(AlarmConfigStruct) + 10

#define DEFAULT_MAC { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

static const IPAddress DEFAULT_IP(192, 168, 1, 66);

static const int DEFAULT_PORT = 6969;

namespace {

    struct EthernetGatewayConfigStruct {
        boolean enabled = false;
        SPIPort spi = SPI_UNKNOWN;
        PinIds ssPin = PI_UNKNOWN;
        byte mac[6] = DEFAULT_MAC;
        IPAddress ip = DEFAULT_IP;
        int port = DEFAULT_PORT;
    };

    class EthernetGatewayConfig {
    private:
    static EthernetGatewayConfigStruct config;
    public:
        static bool enabled(){
            return config.enabled;
        }

        static SPIPort spi(){
            return config.spi;
        }

        static PinIds ssPin(){
            return config.ssPin;
        }
        static byte* mac(){
            return config.mac;
        }

        static IPAddress ip(){
            return config.ip;
        }

        static int port(){
            return config.port;
        }

        static void set(SPIPort newSpi, PinIds newSSPin, byte newMac[6], IPAddress newIP, int newPort) {
            config.enabled = true;
            config.spi = newSpi;
            config.ssPin = newSSPin;

            for (int i = 0; i < 6; ++i) {
                config.mac[i] = newMac[i];
            }

            config.ip = newIP;
            config.port = newPort;

            save();
        }

        static void clear() {
            config = EthernetGatewayConfigStruct();
            save();
            load();
        }

        static void save() {
            EEPROM.put(SERVER_DEFINITION_EEPROM_ADDRESS, config);
        }

        static void load() {
            EEPROM.get(SERVER_DEFINITION_EEPROM_ADDRESS, config);
        }
    };

    EthernetGatewayConfigStruct EthernetGatewayConfig::config = EthernetGatewayConfigStruct();
}
#endif
