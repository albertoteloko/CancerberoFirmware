#include "common/common.h"
#include "log.h"
#include "node/engine.h"

const String TAG = "ParticleDomo.ino";

void setup() {
    Serial.begin(9600);

    while (!Serial) { ; // wait for serial port to connect. Needed for native USB
    }

    Logger::info(TAG, "Particle Domo v1.0");
    NodeEngine::start();
}

void loop() {
    NodeEngine::loop();
}
