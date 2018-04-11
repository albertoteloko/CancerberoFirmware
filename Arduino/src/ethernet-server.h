#ifndef GATEWAY_ENGINE_H
#define GATEWAY_ENGINE_H

#include "common/common.h"
#include "log.h"
#include "command-handler.h"

#include <Ethernet.h>

#define ETHERNET_TAG "Ethernet"

byte mac[] = { 0xCB, 0xCB, 0xCB, 0xCB, 0xCB, 0xCB };
byte ip[] = { 10, 10, 0, 70 };
//byte ip[] = { 192, 168, 1, 70 };

namespace {

    class EthernetGateway {

    public:

        static void start() {
            pinMode(ADDRESS_SELECT_PIN, OUTPUT);

             if(digitalRead(ADDRESS_SELECT_PIN)){
                mac[5] = 0xEE;
                ip[3] = 71;
              }

            Ethernet.begin(mac, ip);
            server.begin();

            info(ETHERNET_TAG, "MAC: %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            info(ETHERNET_TAG, "IP: %u.%u.%u.%u", ip[0],ip[1],ip[2],ip[3]);
            IPAddress serverIp = Ethernet.localIP();
            info(ETHERNET_TAG, "Server real IP: %u.%u.%u.%u", serverIp[0],serverIp[1],serverIp[2],serverIp[3]);
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
                            debug(ETHERNET_TAG, "Received: %s");
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
        static char buffer[1024];

        static String getMAC(){
          String result = "";
          for (int i=0; i <= 5; i++){
            if(i!=0){
              result += ":";
            }
            result += sprintf("%02X", mac[i]);
          }
          return result;
        }

        static String getIP(){
          String result = "";
          for (int i=0; i <= 3; i++){
            if(i!=0){
              result += ".";
            }
            result += ip[i];
          }
          return result;
        }

    };

    EthernetServer EthernetGateway::server = EthernetServer(5555);
    char EthernetGateway::buffer[1024];
}
#endif
