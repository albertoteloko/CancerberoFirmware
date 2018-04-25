#include "common.hpp"
#include "node-engine.hpp"

void setup() {
    Serial.begin(9600);

    while (!Serial) {  // wait for serial port to connect. Needed for native USB
    }

    Serial.print("Starting Node ");
    Serial.print(NODE_NAME);
    Serial.print(": v");
    Serial.println(VERSION);


    NodeEngine::start();

}

void loop() {
    NodeEngine::loop();
}
