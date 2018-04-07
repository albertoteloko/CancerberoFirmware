#ifndef GATEWAY_ENGINE_H
#define GATEWAY_ENGINE_H

#include "../common/common.h"
#include "../log.h"
#include "../events.h"
#include "../command-handler.h"

#include <Ethernet.h>
#include <ArduinoJson.h>

const String ETHERNET_TAG = "Ethernet";

byte mac[] = { 0xCB, 0xCB, 0xCB, 0xCB, 0xCB, 0xCB };
byte ip[] = { 10, 10, 0, 70 };
//byte ip[] = { 192, 168, 1, 70 };

namespace {

    class EthernetGatewayConfig;

    class EthernetGateway {

    public:

        static void publish(String message) {

//            Particle.publish(eventName, payload);
        };

        static void start() {
            pinMode(ADDRESS_SELECT_PIN, OUTPUT);

             if(digitalRead(ADDRESS_SELECT_PIN)){
                mac[5] = 0xEE;
                ip[3] = 71;
              }

            printMAC();
            printIP();

            Ethernet.begin(mac, ip);
            server.begin();

            Serial.print("Server is at ");
            Serial.println(Ethernet.localIP());
        }

        static void loop() {
            EthernetClient client = server.available();
            if (client) {
                memset(&buffer[0], 0, sizeof(buffer));
                DynamicJsonBuffer jsonBuffer(1024);
                Logger::info(ETHERNET_TAG, "Client connected");
                unsigned int index = 0;
                while (client.connected()) {
                    if (client.available()) {
                        char c = client.read();
                        buffer[index++] = c;
                        if (c == '\n') {
                            String input = String(buffer);
                            Logger::debug(ETHERNET_TAG, "Received: " + input);
                            client.println(CommandHandler::processInput(input));
                            break;

                        }
                    }
                }
                client.flush();
                delay(10);
                client.stop();
                Logger::info(ETHERNET_TAG, "Client disconnected");
            }
        }

    private:
        static EthernetServer server;
        static EthernetClient client;
        static char buffer[1024];

        static void sendSuccess(EthernetClient client, String message) {
            DynamicJsonBuffer jsonBuffer(1024);
            JsonObject &root = jsonBuffer.createObject();
            root["message"] = message;
            root.printTo(client);
            client.println(message);
        }

        static void printMAC(){
          Serial.print("MAC: ");
          for (int i=0; i <= 5; i++){
            if(i!=0){
              Serial.print(":");
            }
            Serial.print(mac[i], HEX);
          }
          Serial.println();
        }

        static void printIP(){
          Serial.print("IP:  ");
          for (int i=0; i <= 3; i++){
            if(i!=0){
              Serial.print(".");
            }
            Serial.print(ip[i]);
          }
          Serial.println();
        }

    };

    EthernetServer EthernetGateway::server = EthernetServer(5555);
    EthernetClient EthernetGateway::client;
    char EthernetGateway::buffer[1024];
}
#endif
