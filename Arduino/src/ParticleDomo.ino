#include "common.h"
#include "log.h"
#include "node-engine.h"

#define TAG  "ParticleDomo.ino"


void setup() {
    Serial.begin(9600);

    while (!Serial) {  // wait for serial port to connect. Needed for native USB
    }

    info(TAG, "Starting Node " + String(NODE_NAME) + ": v" + String(VERSION));
    NodeEngine::start();

}

void loop() {
    NodeEngine::loop();
}
