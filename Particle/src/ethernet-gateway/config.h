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
    public:
        static RemoteLog log;
        static bool enabled;
        static SPIPort spi;
        static PinIds ssPin;
        static byte mac[6];
        static IPAddress ip;
        static int port;

        static void set(SPIPort newSpi, PinIds newSSPin, byte newMac[6], IPAddress newIP, int newPort) {
            enabled = true;
            spi = newSpi;
            ssPin = newSSPin;

            for (int i = 0; i < 6; ++i) {
                mac[i] = newMac[i];
            }

            ip = newIP;
            port = newPort;

            save();
        }

        static void clear() {
            EthernetGatewayConfigStruct tmp = EthernetGatewayConfigStruct();
            EEPROM.put(SERVER_DEFINITION_EEPROM_ADDRESS, tmp);
            loadFromConfig(tmp);
        }

        static void save() {
            EthernetGatewayConfigStruct tmp = EthernetGatewayConfigStruct();

            for (int i = 0; i < 6; ++i) {
                tmp.mac[i] = mac[i];
            }

            tmp.enabled = enabled;
            tmp.spi = spi;
            tmp.ssPin = ssPin;
            tmp.ip = ip;
            tmp.port = port;

            EEPROM.put(SERVER_DEFINITION_EEPROM_ADDRESS, tmp);
        }

        static void load() {
            EthernetGatewayConfigStruct tmp = EthernetGatewayConfigStruct();
            EEPROM.get(SERVER_DEFINITION_EEPROM_ADDRESS, tmp);
            loadFromConfig(tmp);
        }

    private:
        static void loadFromConfig(EthernetGatewayConfigStruct config) {
            for (int i = 0; i < 6; ++i) {
                mac[i] = config.mac[i];
            }

            enabled = config.enabled;
            spi = config.spi;
            ssPin = config.ssPin;
            ip = config.ip;
            port = config.port;

        }

    };

    bool EthernetGatewayConfig::enabled = false;
    SPIPort EthernetGatewayConfig::spi = SPI_UNKNOWN;
    PinIds EthernetGatewayConfig::ssPin = PI_UNKNOWN;
    byte EthernetGatewayConfig::mac[6] = DEFAULT_MAC;
    IPAddress EthernetGatewayConfig::ip = DEFAULT_IP;
    int EthernetGatewayConfig::port = DEFAULT_PORT;
}
#endif
