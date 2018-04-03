#ifndef NODE_ENGINE_H
#define NODE_ENGINE_H

#include "../common.h"
#include "../log.h"
#include "../command-handler.h"
#include "../alarm/engine.h"
#include "../ethernet-gateway/engine.h"
#include "../card/engine.h"

#define DEFAULT_BLINK_DURATION                          1000
#define DEFAULT_BLINK_PERIOD                            30000

#define DEFAULT_ACTIVATING_BEEP_DURATION                400
#define DEFAULT_ACTIVATING_BEEP_FREQUENCY               400
#define DEFAULT_ACTIVATING_BEEP_PERIOD_MIN              1200
#define DEFAULT_ACTIVATING_BEEP_PERIOD_MAX              5000

#define DEFAULT_SUSPICIOUS_BEEP_DURATION                300
#define DEFAULT_SUSPICIOUS_BEEP_FREQUENCY               1500
#define DEFAULT_SUSPICIOUS_BEEP_PERIOD_MIN              900
#define DEFAULT_SUSPICIOUS_BEEP_PERIOD_MAX              3000

namespace {


    class NodeEngine {

    public:
        static void start() {
            pinMode(LED_RED_PIN, OUTPUT);
            pinMode(LED_GREEN_PIN, OUTPUT);
            #ifdef LED_YELLOW_PIN
                pinMode(LED_YELLOW_PIN, OUTPUT);
            #endif
            pinMode(SPEAKER_PIN, OUTPUT);

            CommandHandler::start();
            Alarm::start();
            EthernetGateway::start();
            Card::start();
        }

        static void loop() {
            Alarm::loop();
            EthernetGateway::loop();
            BeepMode result = Card::loop();

            if (result == BM_GRANTED) {
                beepGranted();
            }else if (result == BM_NOT_GRANTED) {
                beepNotGranted();
            }

            NodeStatus currentStatus = getCurrentStatus();

            if (currentStatus != status) {
                setStatus(currentStatus);
            }

            ledsLoop();
            speakerLoop();
        }

        static void beepGranted() {
            digitalWrite(LED_RED_PIN, LOW);
            digitalWrite(LED_GREEN_PIN, LOW);
            digitalWrite(LED_GREEN_PIN, HIGH);
            noTone(SPEAKER_PIN);
            tone(SPEAKER_PIN, 1200, 600);
            delay(600);
            tone(SPEAKER_PIN, 2000, 600);
            delay(600);
            digitalWrite(LED_GREEN_PIN, LOW);
            setSpeakerTone();
        }

        static void beepNotGranted() {
            digitalWrite(LED_RED_PIN, LOW);
            digitalWrite(LED_GREEN_PIN, LOW);
            digitalWrite(LED_RED_PIN, HIGH);
            noTone(SPEAKER_PIN);
            tone(SPEAKER_PIN, 600, 500);
            delay(500);
            digitalWrite(LED_RED_PIN, LOW);
            setSpeakerTone();
        }

    private:
        static RemoteLog log;
        static NodeStatus status;
        static bool ledOn;
        static bool beeping;
        static unsigned long step;
        static unsigned long statusTime;
        static unsigned long beepChange;
        static unsigned long ledChange;

        static NodeStatus getCurrentStatus() {
            switch (AlarmConfig::getStatus()) {
                case AS_IDLE:
                    return NS_IDLE;
                case AS_ACTIVATING:
                    return NS_ACTIVATING;
                case AS_ACTIVATED:
                    return NS_ACTIVATED;
                case AS_SUSPICIOUS:
                    return NS_SUSPICIOUS;
                case AS_ALARMED:
                    return NS_ALARMED;
                case AS_SAFETY:
                    return NS_SAFETY;
                case AS_SABOTAGE:
                    return NS_SABOTAGE;
                default:
                    return NS_IDLE;
            }
        }

        static void setStatus(NodeStatus newStatus) {
            if (newStatus == NS_UNKNOWN) {
                log.debug("Unknown status");
            } else {
                status = newStatus;
                String statusName = fromNodeStatus(status);
                log.info("Changed status to: " + statusName);

                ledOn = false;
                beeping = false;
                step = 0;
                statusTime = millis();
                beepChange = millis();
                ledChange = millis();

                digitalWrite(LED_RED_PIN, LOW);
                digitalWrite(LED_GREEN_PIN, LOW);
                #ifdef LED_YELLOW_PIN
                    digitalWrite(LED_YELLOW_PIN, LOW);
                #endif


                setSpeakerTone();
            }
        }

        static void setSpeakerTone() {
            noTone(SPEAKER_PIN);
            if (status == NS_ALARMED) {
                tone(SPEAKER_PIN, 3000);
            } else if (status == NS_SAFETY) {
                tone(SPEAKER_PIN, 2000);
            } else if (status == NS_SABOTAGE) {
                tone(SPEAKER_PIN, 4000);
            }
        }

        static void ledsLoop() {
            if (status == NS_IDLE) {
                loopStaticLedBlink(
                        DEFAULT_BLINK_DURATION,
                        DEFAULT_BLINK_PERIOD
                );
            } else if (status == NS_ACTIVATING) {
                loopLedBlink(
                        AlarmConfig::activatingTime(),
                        DEFAULT_ACTIVATING_BEEP_DURATION,
                        DEFAULT_ACTIVATING_BEEP_PERIOD_MIN,
                        DEFAULT_ACTIVATING_BEEP_PERIOD_MAX
                );
            } else if (status == NS_ACTIVATED) {
                #ifndef LED_YELLOW_PIN
                digitalWrite(LED_RED_PIN, HIGH);
                digitalWrite(LED_GREEN_PIN, HIGH);
                #endif
                #ifdef LED_YELLOW_PIN
                digitalWrite(LED_YELLOW_PIN, HIGH);
                #endif
            } else if (status == NS_SUSPICIOUS) {
                loopLedBlink(
                        AlarmConfig::suspiciousTime(),
                        DEFAULT_SUSPICIOUS_BEEP_DURATION,
                        DEFAULT_SUSPICIOUS_BEEP_PERIOD_MIN,
                        DEFAULT_SUSPICIOUS_BEEP_PERIOD_MAX
                );
            } else {
                digitalWrite(LED_RED_PIN, HIGH);
            }
        }

        static void speakerLoop() {
            if (status == NS_IDLE) {
                loopSpeakerIdle();
            } else if (status == NS_ACTIVATING) {
                loopSpeakerBeep(
                        AlarmConfig::activatingTime(),
                        DEFAULT_ACTIVATING_BEEP_FREQUENCY,
                        DEFAULT_ACTIVATING_BEEP_DURATION,
                        DEFAULT_ACTIVATING_BEEP_PERIOD_MIN,
                        DEFAULT_ACTIVATING_BEEP_PERIOD_MAX
                );
            } else if (status == NS_ACTIVATED) {
                loopSpeakerActivated();
            } else if (status == NS_SUSPICIOUS) {
                loopSpeakerBeep(
                        AlarmConfig::suspiciousTime(),
                        DEFAULT_SUSPICIOUS_BEEP_FREQUENCY,
                        DEFAULT_SUSPICIOUS_BEEP_DURATION,
                        DEFAULT_SUSPICIOUS_BEEP_PERIOD_MIN,
                        DEFAULT_SUSPICIOUS_BEEP_PERIOD_MAX
                );
            }
        }


        static void loopSpeakerIdle() {
            if (beepChange <= millis()) {
                if (step == 0) {
                    beepChange = millis() + 200;
                    tone(SPEAKER_PIN, 3200);
                    step += 1;
                } else if (step == 1) {
                    beepChange = millis() + 200;
                    tone(SPEAKER_PIN, 2400);
                    step += 1;
                } else if (step == 2) {
                    beepChange = millis() + 200;
                    tone(SPEAKER_PIN, 2000);
                    step += 1;
                } else {
                    noTone(SPEAKER_PIN);
                }
            }
        }

        static void loopSpeakerActivated() {
            if (beepChange <= millis()) {
                if (step == 0) {
                    beepChange = millis() + 200;
                    tone(SPEAKER_PIN, 400);
                    step += 1;
                } else if (step == 1) {
                    beepChange = millis() + 200;
                    tone(SPEAKER_PIN, 1200);
                    step += 1;
                } else if (step == 2) {
                    beepChange = millis() + 200;
                    tone(SPEAKER_PIN, 2800);
                    step += 1;
                } else {
                    noTone(SPEAKER_PIN);
                }
            }
        }


        static long getCycleTime(long statusDuration, long minCycleTime, long maxCycleTime) {
            unsigned long elapsedMillis = millis() - statusTime;
            float factor = (float) (statusDuration - elapsedMillis) / (float) statusDuration;

            long cycleTime = maxCycleTime * factor;
            cycleTime = max(cycleTime, minCycleTime);

            return cycleTime;
        }

        static void
        loopLedBlink(long statusDuration, long blinkDuration, long minCycleTime, long maxCycleTime) {
            if (ledChange <= millis()) {
                if (!ledOn) {
                    if (status == NS_ACTIVATING) {
                        #ifndef LED_YELLOW_PIN
                            digitalWrite(LED_RED_PIN, HIGH);
                            digitalWrite(LED_GREEN_PIN, HIGH);
                        #endif
                        #ifdef LED_YELLOW_PIN
                            digitalWrite(LED_YELLOW_PIN, HIGH);
                        #endif
                    }else{
                        digitalWrite(LED_RED_PIN, HIGH);
                    }
                    ledOn = true;
                    ledChange = millis() + blinkDuration;
                } else {
                    if (status == NS_ACTIVATING) {
                        #ifndef LED_YELLOW_PIN
                            digitalWrite(LED_RED_PIN, LOW);
                            digitalWrite(LED_GREEN_PIN, LOW);
                        #endif
                        #ifdef LED_YELLOW_PIN
                            digitalWrite(LED_YELLOW_PIN, LOW);
                        #endif
                    }else{
                        digitalWrite(LED_RED_PIN, LOW);
                    }
                    ledOn = false;
                    long cycleTime = getCycleTime(statusDuration, minCycleTime, maxCycleTime);
                    long silenceTime = (cycleTime - blinkDuration);
                    ledChange = millis() + silenceTime;
                }
            }
        }

        static void
        loopStaticLedBlink(long blinkDuration, long cycleTime) {
            if (ledChange <= millis()) {
                if (!ledOn) {
                    digitalWrite(LED_GREEN_PIN, HIGH);
                    ledOn = true;
                    ledChange = millis() + blinkDuration;
                } else {
                    digitalWrite(LED_GREEN_PIN, LOW);
                    ledOn = false;
                    long silenceTime = (cycleTime - blinkDuration);
                    ledChange = millis() + silenceTime;
                }
            }
        }

        static void
        loopSpeakerBeep(long statusDuration, long frequency, long beepDuration, long minCycleTime, long maxCycleTime) {
            if (beepChange <= millis()) {
                if (!beeping) {
                    tone(SPEAKER_PIN, frequency);
                    beeping = true;
                    beepChange = millis() + beepDuration;
                } else {
                    noTone(SPEAKER_PIN);
                    beeping = false;
                    long cycleTime = getCycleTime(statusDuration, minCycleTime, maxCycleTime);
                    long silenceTime = (cycleTime - beepDuration);
                    beepChange = millis() + silenceTime;
                }
            }
        }
    };

    RemoteLog NodeEngine::log = RemoteLog("node-engine");
    NodeStatus NodeEngine::status = NS_UNKNOWN;
    bool NodeEngine::ledOn = false;
    bool NodeEngine::beeping = false;
    unsigned long NodeEngine::step = 0;
    unsigned long NodeEngine::statusTime = 0;
    unsigned long NodeEngine::beepChange = 0;
    unsigned long NodeEngine::ledChange = 0;
}
#endif
