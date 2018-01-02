#include "log.h"
#include "node/engine.h"

RemoteLog logger("ParticleDomo.ino");

void setup() {
    logger.info("Particle Domo v1.0");
    NodeEngine::start();
}

void loop() {
    NodeEngine::loop();
}
