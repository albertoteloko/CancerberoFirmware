#ifndef GATEWAY_ENGINE_H
#define GATEWAY_ENGINE_H


#include "common.h"
#include "log.h"
#include "command-handler.h"

#include <Ethernet.h>

#define ETHERNET_TAG "Ethernet"

namespace {

    class EthernetGateway {

    public:

        static void start() {
            byte mac[] = NODE_MAC;
            byte ip[] = NODE_IP;
            byte masterIp[] = MASTER_IP;

            Ethernet.begin(mac, ip);
            server.begin();

            char macBuffer[17];
            sprintf(macBuffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            info(ETHERNET_TAG, "MAC:      " + String(macBuffer));
            char ipBuffer[15];
            sprintf(ipBuffer, "%i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
            info(ETHERNET_TAG, "IP:       "  + String(ipBuffer));
            sprintf(ipBuffer, "%i.%i.%i.%i", masterIp[0], masterIp[1], masterIp[2], masterIp[3]);
            info(ETHERNET_TAG, "Master IP:"  + String(ipBuffer));
            IPAddress serverIp = Ethernet.localIP();
            sprintf(ipBuffer, "%i.%i.%i.%i", serverIp[0], serverIp[1], serverIp[2], serverIp[3]);
            info(ETHERNET_TAG, "Card IP:  "  + String(ipBuffer));
        }

        static void loop() {
            EthernetClient client = server.available();
            if (client) {
                memset(&buffer[0], 0, sizeof(buffer));
                info(ETHERNET_TAG, "Client connected");
                unsigned int index = 0;
                while (client.connected()) {
                    if (client.available()) {
                        char c = client.read();
                        if (c == '\n') {
                            String input = String(buffer);
//                            debug(ETHERNET_TAG, "Received: %s");
                            client.println(CommandHandler::processInput(input));
                            break;
                        }else if (c != '\r') {
                            buffer[index++] = c;
                        }
                    }
                }
                client.flush();
                delay(10);
                client.stop();
                info(ETHERNET_TAG, "Client disconnected");
            }
        }

    private:
        static EthernetServer server;
        static EthernetClient client;
        static char buffer[64];
    };

    EthernetServer EthernetGateway::server = EthernetServer(NODE_PORT);
    char EthernetGateway::buffer[64];
}
#endif
