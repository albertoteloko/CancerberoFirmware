#ifndef GATEWAY_ENGINE_H
#define GATEWAY_ENGINE_H

#include "printf.hpp"
#include "common.hpp"
#include "command-handler.hpp"

#include <Ethernet.h>


    class EthernetGateway {

    public:

        static void start() {
            byte mac[] = NODE_MAC;
            byte ip[] = NODE_IP;
            byte masterIp[] = MASTER_IP;

            Ethernet.begin(mac, ip);
            server.begin();

            #if(LOG_LEVEL<=INFO)
                printfn("Ethernet - MAC:       %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                printfn("Ethernet - IP:        %i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
                printfn("Ethernet - Master IP: %i.%i.%i.%i", masterIp[0], masterIp[1], masterIp[2], masterIp[3]);
                IPAddress serverIp = Ethernet.localIP();
                printfn("Ethernet - Card IP:   %i.%i.%i.%i", serverIp[0], serverIp[1], serverIp[2], serverIp[3]);
            #endif

        }

        static void loop() {
            EthernetClient client = server.available();
            if (client) {
                memset(&buffer[0], 0, sizeof(buffer));

                #if(LOG_LEVEL<=DEBUG)
                    Serial.println("Ethernet - Client connected");
                #endif

                unsigned int index = 0;
                while (client.connected()) {
                    if (client.available()) {
                        char c = client.read();
                        if (c == '\n') {
                            String input = String(buffer);
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

                #if(LOG_LEVEL<=DEBUG)
                    Serial.println("Ethernet - Client disconnected");
                #endif
            }
        }

    private:
        static EthernetServer server;
        static EthernetClient client;
        static char buffer[64];
    };

    EthernetServer EthernetGateway::server = EthernetServer(NODE_PORT);
    char EthernetGateway::buffer[64];
#endif
