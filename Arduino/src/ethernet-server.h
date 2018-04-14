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
            Ethernet.begin(mac, ip);
            server.begin();

            info(ETHERNET_TAG, "MAC: " + getMAC(mac));
            info(ETHERNET_TAG, "IP: " + getIPConf(ip));
            IPAddress serverIp = Ethernet.localIP();
            info(ETHERNET_TAG, "Server real IP: " + getIP(serverIp));
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

        static String getMAC(byte mac[]){
          String result = "";
          for (int i=0; i <= 5; i++){
            if(i!=0){
              result += ":";
            }
            result += sprintf("%02X", mac[i]);
          }
          return result;
        }

        static String getIPConf(byte ip[]){
          String result = "";
          for (int i=0; i <= 3; i++){
            if(i!=0){
              result += ".";
            }
            result += ip[i];
          }
          return result;
        }

        static String getIP(IPAddress ip){
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
    char EthernetGateway::buffer[64];
}
#endif
