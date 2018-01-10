#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "common.h"
#include "alarm/config.h"
#include "alarm/engine.h"
#include "ethernet-gateway/engine.h"
#include "card/engine.h"


namespace {

    class CommandHandler {
    public:
        static void start() {
            Particle.variable("L.level", &RemoteLog::levelName, STRING);
            Particle.variable("MN.status", &(AlarmConfig::statusName), STRING);

            Particle.function("clear", &clear);
            Particle.function("L.level", &setLogLevel);

            Particle.function("A.status", &setAlarmStatus);
            Particle.function("A.enable", &enableAlarm);
            Particle.function("A.disable", &disableAlarm);

            Particle.function("EG.enable", &enableEthernetGateway);
            Particle.function("EG.disable", &disableEthernetGateway);

            Particle.function("C.enable", &enableCard);
            Particle.function("C.disable", &disableCard);
            Particle.function("C.add", &addCard);
            Particle.function("C.del", &removeCard);
        }

        static bool isGatewayMessage(String input) {
            return input.indexOf("@") > -1;
        }

        static IPAddress getRemoteIp(String input) {
            String ipVal = input.substring(0, input.indexOf("@"));

            IPAddress ip = toIP(ipVal);
            bool ipResult = validIp(ip);
            if (!ipResult) {
                log.error("Unknown ip: " + ipVal);
            }
            return ip;
        }

        static String getMessage(String input) {
            return input.substring(input.indexOf("@") + 1);
        }

        static int setLogLevel(String input) {
            if (isGatewayMessage(input)) {
                IPAddress ip = getRemoteIp(input);
                String message = getMessage(input);
                return EthernetGateway::sendCommand(ip, "L.level", message);
            } else {
                return RemoteLog::setLevel(input);
            }
        }

        static int setAlarmStatus(String input) {
            if (isGatewayMessage(input)) {
                IPAddress ip = getRemoteIp(input);
                String message = getMessage(input);
                return EthernetGateway::sendCommand(ip, "A.status", message);
            } else {
                String value = input;
                String user = NO_SOURCE;

                if (value.indexOf(",") > -1) {
                    value = input.substring(0, input.indexOf(","));
                    user = input.substring(input.indexOf(",") + 1);
                }
                AlarmStatus status = toAlarmStatus(value);

                if (status != AS_UNKNOWN) {
                    return Alarm::setStatus(status, user);
                } else {
                    log.debug("Unknown new master status: " + value);
                    return NS_UNKNOWN;
                }
            }
        }

        static int enableAlarm(String input) {
            if (isGatewayMessage(input)) {
                IPAddress ip = getRemoteIp(input);
                String message = getMessage(input);
                return EthernetGateway::sendCommand(ip, "A.enable", message);
            } else {
                AlarmPin pins[MASTER_PIN_NUMBER];


                log.debug("Input: " + input);

                if (!input.endsWith(",")) {
                    input += ",";
                }

                int start = 0;
                int end = 0;

                int node = 0;
                int index = 0;

                long activatingTime = DEFAULT_ACTIVATING_TIME;
                long suspiciousTime = DEFAULT_SUSPICIOUS_TIME;

                while ((end = input.indexOf(",", start)) > -1) {
                    String part = input.substring(start, end);
                    if (index++ == 0) {
                        int input = atoi(part);

                        if (input > 0) {
                            activatingTime = input;
                        } else {
                            log.info("Invalid activating time: " + input);
                        }
                    } else if (index == 1) {
                        int input = atoi(part);

                        if (input > 0) {
                            suspiciousTime = input;
                        } else {
                            log.info("Invalid suspicious time: " + input);
                        }
                    }
                    AlarmPin pin = loadAlarmPin(part);

                    if (pin.id != PI_UNKNOWN) {
                        if (node++ == MASTER_PIN_NUMBER) {
                            log.error("Too much pin, max number are: " + String(MASTER_PIN_NUMBER));
                            return false;
                        } else {
                            log.info("Part: " + part);
                            pins[node] = pin;
                        }
                    } else {
                        log.error("Unknown pin definition: '" + part + "'");
                        return false;
                    }
                    start = end + 1;
                }

                Alarm::enable(activatingTime, suspiciousTime, pins);
                return true;
            }
        }

        static const AlarmPin loadAlarmPin(String pinDefinition) {
            int start = 0;
            int end = pinDefinition.indexOf("|");

            if (end == -1) {
                log.error("Id not found");
                return NO_PIN;
            }

            String idVal = pinDefinition.substring(start, end);
            PinIds id = toPinIds(idVal);

            if (id == PI_UNKNOWN) {
                log.error("Unknown id: " + idVal);
                return NO_PIN;
            }

            start = end + 1;
            end = pinDefinition.indexOf("|", start);

            if (end == -1) {
                log.error("Type not found");
                return NO_PIN;
            }

            String typeVal = pinDefinition.substring(start, end);
            PinType type = toPinType(typeVal);

            if (type == PT_UNKNOWN) {
                log.error("Unknown type: " + typeVal);
                return NO_PIN;
            }

            start = end + 1;

            String modeVal = pinDefinition.substring(start);
            DPinMode mode = toPinMode(modeVal);

            if (mode == PM_UNKNOWN) {
                log.error("Unknown pin mode: " + modeVal);
                return NO_PIN;
            }

            AlarmPin pin;

            pin.id = id;
            pin.type = type;
            pin.mode = mode;

            return pin;
        }

        static int disableAlarm(String input) {
            if (isGatewayMessage(input)) {
                IPAddress ip = getRemoteIp(input);
                String message = getMessage(input);
                return EthernetGateway::sendCommand(ip, "A.disable", message);
            } else {
                Alarm::disable();
                return true;
            }
        }

        static int disableEthernetGateway(String input) {
            if (isGatewayMessage(input)) {
                IPAddress ip = getRemoteIp(input);
                String message = getMessage(input);
                return EthernetGateway::sendCommand(ip, "EW.disable", message);
            } else {
                EthernetGateway::disable();
                return true;
            }
        }

        static int disableCard(String input) {
            if (isGatewayMessage(input)) {
                IPAddress ip = getRemoteIp(input);
                String message = getMessage(input);
                return EthernetGateway::sendCommand(ip, "C.disable", message);
            } else {
                Card::disable();
                return true;
            }
        }

        static int addCard(String input) {
            if (isGatewayMessage(input)) {
                IPAddress ip = getRemoteIp(input);
                String message = getMessage(input);
                return EthernetGateway::sendCommand(ip, "C.add", message);
            } else {
                byte cardId[CARD_ID_BYTES];
                bool parseResult = toCardId(input, cardId);

                if (!parseResult) {
                    log.error("Invalid card id: " + input);
                    return -1;
                }
                log.info("Adding card id: " + fromCardId(cardId));
                int result = CardConfig::add(cardId);

                if (result == ALREADY_ADDED) {
                    log.error("Unable to add card id: " + fromCardId(cardId) + " already added");
                } else if (result == NOT_SPACE_LEFT) {
                    log.error("Unable to add card id: " + fromCardId(cardId) + " not space left");
                }

                return result;
            }
        }

        static int removeCard(String input) {
            if (isGatewayMessage(input)) {
                IPAddress ip = getRemoteIp(input);
                String message = getMessage(input);
                return EthernetGateway::sendCommand(ip, "C.del", message);
            } else {
                byte cardId[CARD_ID_BYTES];
                bool parseResult = toCardId(input, cardId);

                if (!parseResult) {
                    log.error("Invalid card id: " + input);
                    return -1;
                }

                log.info("Remove card id: " + fromCardId(cardId));
                int result = CardConfig::remove(cardId);

                if (result == NOT_FOUND) {
                    log.error("Unable to remove card id: " + fromCardId(cardId) + " not found");
                }
                return result;
            }
        }

        static int clear(String input) {
            if (isGatewayMessage(input)) {
                IPAddress ip = getRemoteIp(input);
                String message = getMessage(input);
                return EthernetGateway::sendCommand(ip, "clear", message);
            } else {
                EEPROM.clear();
                disableAlarm(input);
                disableEthernetGateway(input);
                disableCard(input);
                return true;
            }
        }

        static int enableEthernetGateway(String input) {
            if (isGatewayMessage(input)) {
                IPAddress ip = getRemoteIp(input);
                String message = getMessage(input);
                return EthernetGateway::sendCommand(ip, "EW.enable", message);
            } else {
                int start = 0;
                int end = input.indexOf("|");

                if (end == -1) {
                    log.error("SPI port not found");
                    return false;
                }

                String spiVal = input.substring(start, end);
                SPIPort spi = toSPIPort(spiVal);

                if (spi == SPI_UNKNOWN) {
                    log.error("Unknown spi: " + spiVal);
                    return false;
                }

                start = end + 1;
                end = input.indexOf("|", start);

                if (end == -1) {
                    log.error("SS pin not found");
                    return false;
                }

                String pinIdsVal = input.substring(start, end);
                PinIds ssPin = toPinIds(pinIdsVal);

                if (ssPin == PI_UNKNOWN) {
                    log.error("Unknown pin ids: " + pinIdsVal);
                    return false;
                }

                start = end + 1;
                end = input.indexOf("|", start);

                if (end == -1) {
                    log.error("MAC not found");
                    return false;
                }

                String macVal = input.substring(start, end);
                byte mac[6];
                bool macResult = toMac(macVal, mac);

                if (!macResult) {
                    log.error("Unknown mac: " + macVal);
                    return false;
                }

                start = end + 1;
                end = input.indexOf("|", start);

                if (end == -1) {
                    log.error("IP not found");
                    return false;
                }

                String ipVal = input.substring(start, end);
                IPAddress ip = toIP(ipVal);
                bool ipResult = validIp(ip);

                if (!ipResult) {
                    log.error("Unknown ip: " + ipVal);
                    return false;
                }

                start = end + 1;

                String portVal = input.substring(start);
                int port = portVal.toInt();

                if ((port <= 0) || (port > 65535)) {
                    log.error("Unknown port: " + portVal);
                    return false;
                }


                log.info("New Values!");
                log.info("SPI: " + fromSPIPort(spi));
                log.info("SS Pin: " + fromPinIds(ssPin));
                log.info("Mac: " + fromMac(mac));
                log.info("IP: " + String(ip));
                log.info("Port: " + String(port));

                EthernetGateway::enable(spi, ssPin, mac, ip, port);
                return true;
            }
        }

        static int enableCard(String input) {
            if (isGatewayMessage(input)) {
                IPAddress ip = getRemoteIp(input);
                String message = getMessage(input);
                return EthernetGateway::sendCommand(ip, "C.enable", message);
            } else {
                int start = 0;
                int end = input.indexOf("|");

                if (end == -1) {
                    log.error("SPI port not found");
                    return false;
                }

                String spiVal = input.substring(start, end);
                SPIPort spi = toSPIPort(spiVal);

                if (spi == SPI_UNKNOWN) {
                    log.error("Unknown spi: " + spiVal);
                    return false;
                }

                start = end + 1;

                String pinIdsVal = input.substring(start);
                PinIds ssPin = toPinIds(pinIdsVal);

                if (ssPin == PI_UNKNOWN) {
                    log.error("Unknown pin ids: " + pinIdsVal);
                    return false;
                }

                log.info("New Values!");
                log.info("SPI: " + fromSPIPort(spi));
                log.info("SS Pin: " + fromPinIds(ssPin));

                Card::enable(spi, ssPin);
                return true;
            }
        }

    private:
        static RemoteLog log;
        static AlarmPin NO_PIN;
    };

    RemoteLog CommandHandler::log = RemoteLog("CommandHandler");
    AlarmPin CommandHandler::NO_PIN = AlarmPin();
}
#endif
