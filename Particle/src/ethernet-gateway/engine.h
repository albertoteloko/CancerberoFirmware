#ifndef GATEWAY_ENGINE_H
#define GATEWAY_ENGINE_H

#include "../log.h"
#include "../events.h"
#include "../common.h"
#include "config.h"
#include "../alarm/config.h"

#include <SPI.h>
#include <utility/w5100.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

namespace {

    class EthernetGatewayConfig;

    class EthernetGateway {

    public:
        static void enable(SPIPort spi, PinIds ssPin, byte mac[6], IPAddress ip, int port) {
            EthernetGatewayConfig::set(spi, ssPin, mac, ip, port);
            setup();
        }

        static void disable() {
            EthernetGatewayConfig::clear();
            setup();
        }

        static void start() {
            EthernetGatewayConfig::load();
            setup();
        }

        static void stop() {
            EthernetGatewayConfig::save();
        }

        static int sendCommand(IPAddress ip, String action, String message) {
            if (EthernetGatewayConfig::enabled()) {
                int result = -1;
                long port = 6969;
                log.info("Incoming remote action " + String(ip) + " -> " + String(action) + ": " + message);

                bool ipResult = validIp(ip);

                if (!ipResult) {
                    return false;
                }

                if (client.connect(ip, port)) {
                    log.info("Connected to " + String(ip) + ":" + String(port));
                    client.print(action);
                    client.print("#");
                    client.println(message);


                    long ti = millis();

                    while ((millis() - ti) < 100) {
                        if (client.available()) {
                            result = client.read();
                            break;
                        }
                    }

                    if (result == -1) {
                        log.info("No response");
                        result = -2;
                    }
                } else {
                    log.info("Unable to connect to " + String(ip) + ":" + String(port));
                }
                client.stop();
                return result;
            } else {
                log.info("Ethernet gateway is disabled");
                return -3;
            }
        }

        static void setup() {
            if (EthernetGatewayConfig::enabled()) {
                if (EthernetGatewayConfig::spi() != SPI_UNKNOWN) {
                    server.end();
                    W5100.end();

                    log.info("Starting ethernet-gateway");
                    log.info("SPI: " + fromSPIPort(EthernetGatewayConfig::spi()));
                    log.info("SS Pin: " + fromPinIds(EthernetGatewayConfig::ssPin()));
                    log.info("Mac: " + fromMac(EthernetGatewayConfig::mac()));
                    log.info("IP: " + String(EthernetGatewayConfig::ip()));
                    log.info("Port: " + String(EthernetGatewayConfig::port()));

                    server.setPort(EthernetGatewayConfig::port());
                    W5100.spi(getSPI(EthernetGatewayConfig::spi()), EthernetGatewayConfig::ssPin());
                    Ethernet.begin(EthernetGatewayConfig::mac(), EthernetGatewayConfig::ip());
                    server.begin();
                }
            } else {
                log.info("Ethernet gateway is disabled");
            }
        }

        static void loop() {
            if (EthernetGatewayConfig::enabled()) {
                EthernetClient client = server.available();
                if (client) {
                    IPAddress remoteIp = client.remoteIP();
                    memset(&buffer[0], 0, sizeof(buffer));
                    DynamicJsonBuffer jsonBuffer(1024);
                    log.info("Client " + String(remoteIp) + " connected");
                    unsigned int index = 0;
                    while (client.connected()) {
                        if (client.available()) {
                            char c = client.read();
                            buffer[index++] = c;
                            if (c == '}') {
                                String input = String(buffer);
                                log.debug("Received: " + input);
                                JsonObject &root = jsonBuffer.parseObject(input);

                                if (!root.success()) {
                                    log.info("Invalid json: " + input);
                                    sendError(client, "Invalid JSON");
                                } else {
                                    if (root.containsKey("name")) {
                                        String name = root["name"];
                                        EventDispatcher::publish(name, input);
                                        sendSuccess(client, "OK");
                                    } else {
                                        sendError(client, "Event name missing");
                                    }
                                }

                                break;

                            }
                        }
                    }
                    client.flush();
                    delay(10);
                    client.stop();
                    log.info("Client " + String(remoteIp) + " disconnected");
                    delay(10);
                }
            }
        }

    private:
        static RemoteLog log;
        static EthernetServer server;
        static EthernetClient client;
        static char buffer[1024];

        static void sendError(EthernetClient client, String message) {
            DynamicJsonBuffer jsonBuffer(1024);
            JsonObject &root = jsonBuffer.createObject();
            root["error"] = message;
            root.printTo(client);
        }

        static void sendSuccess(EthernetClient client, String message) {
            DynamicJsonBuffer jsonBuffer(1024);
            JsonObject &root = jsonBuffer.createObject();
            root["message"] = message;
            root.printTo(client);
        }

    };

    RemoteLog EthernetGateway::log = RemoteLog("ethernet-gateway");
    EthernetServer EthernetGateway::server = EthernetServer(0);
    EthernetClient EthernetGateway::client;
    char EthernetGateway::buffer[1024];
}
#endif
