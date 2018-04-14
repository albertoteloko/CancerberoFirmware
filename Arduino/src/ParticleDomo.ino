#include "common/common.h"
#include "log.h"
#include "node-engine.h"

#define TAG  "ParticleDomo.ino"

void setup() {
    Serial.begin(9600);

    while (!Serial) { ; // wait for serial port to connect. Needed for native USB
    }

    info(TAG, "Particle Domo v1.0");
    NodeEngine::start();

    Serial.print("a->");
    Serial.println(StrToHex("a"));
}


int StrToHex(char str[])
{
  return (int) strtol(str, 0, 16);
}

void loop() {
    NodeEngine::loop();
}
