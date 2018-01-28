#ifndef ALARM_CONFIG_H
#define ALARM_CONFIG_H

#include "../common.h"

#define ALARM_DEFINITION_EEPROM_ADDRESS 0

#define DEFAULT_ACTIVATING_TIME             60000
#define DEFAULT_SUSPICIOUS_TIME             30000

namespace {

    struct AlarmPin {
        PinIds id;
        PinType type = PT_UNKNOWN;
        DPinMode mode = PM_UNKNOWN;
        DPinInput input = PIN_UNKNOWN;
        int threshold;
    };

    struct AlarmConfigStruct {
        bool enabled = false;
        AlarmStatus status = AS_UNKNOWN;
        DPinMode outputMode = PM_HIGH;
        int activatingTime = DEFAULT_ACTIVATING_TIME;
        int suspiciousTime = DEFAULT_SUSPICIOUS_TIME;
        AlarmPin pins[MASTER_PIN_NUMBER];
    };


    class AlarmConfig {
    private:
        static RemoteLog log;
        static AlarmStatus status;
        static AlarmPin pins[MASTER_PIN_NUMBER];
    public:
        static bool enabled;
        static String statusName;
        static DPinMode outputMode;
        static int activatingTime;
        static int suspiciousTime;


        static void set(long newActivatingTime, long newSuspiciousTime, AlarmPin newPins[MASTER_PIN_NUMBER]) {
            enabled = true;
            activatingTime = newActivatingTime;
            suspiciousTime = newSuspiciousTime;

            for (int i = 0; i < MASTER_PIN_NUMBER; ++i) {
                pins[i] = newPins[i];
            }

            save();
        }

        static void forEachPin(void (*f)(int, AlarmPin)) {
            for (int i = 0; i < MASTER_PIN_NUMBER; ++i) {
                (*f)(i, pins[i]);
            }
        }

        static void forEachDefinedPin(void (*f)(int, AlarmPin)) {
            for (int i = 0; i < MASTER_PIN_NUMBER; ++i) {
                if ((pins[i].id != PI_UNKNOWN) && (pins[i].type != PT_UNKNOWN) && (pins[i].mode != PM_UNKNOWN)) {
                    (*f)(i, pins[i]);
                }
            }
        }

        static AlarmStatus getStatus() {
            return status;
        }

        static void setStatus(AlarmStatus newStatus) {
            status = newStatus;
            statusName = fromAlarmStatus(newStatus);
            EEPROM.put(ALARM_DEFINITION_EEPROM_ADDRESS + 1, status);
        }

        static void clear() {
            AlarmConfigStruct tmp = AlarmConfigStruct();
            EEPROM.put(ALARM_DEFINITION_EEPROM_ADDRESS, tmp);
            loadFromConfig(tmp);
        }

        static void save() {
            AlarmConfigStruct tmp = AlarmConfigStruct();
            tmp.enabled = enabled;
            tmp.status = status;
            tmp.activatingTime = activatingTime;
            tmp.suspiciousTime = suspiciousTime;

            for (int i = 0; i < MASTER_PIN_NUMBER; ++i) {
                tmp.pins[i] = pins[i];
            }
            EEPROM.put(ALARM_DEFINITION_EEPROM_ADDRESS, tmp);
        }

        static void load() {
            AlarmConfigStruct tmp = AlarmConfigStruct();
            EEPROM.get(ALARM_DEFINITION_EEPROM_ADDRESS, tmp);
            loadFromConfig(tmp);
        }


    private:
        static void loadFromConfig(AlarmConfigStruct config) {
            for (int i = 0; i < MASTER_PIN_NUMBER; ++i) {
                pins[i] = config.pins[i];
            }

            if (config.status == AS_UNKNOWN) {
                log.error("No status, setting idle one");
                config.status = AS_IDLE;
            }
            enabled = config.enabled;
            outputMode = config.outputMode;
            activatingTime = config.activatingTime;
            suspiciousTime = config.suspiciousTime;

            setStatus(config.status);
        }

    };

    RemoteLog AlarmConfig::log = RemoteLog("alarm-definition");
    bool AlarmConfig::enabled = false;
    String AlarmConfig::statusName = "UNKNOWN";
    AlarmStatus AlarmConfig::status = AS_UNKNOWN;
    DPinMode AlarmConfig::outputMode = PM_HIGH;
    AlarmPin  AlarmConfig::pins[MASTER_PIN_NUMBER];
    int AlarmConfig::activatingTime = DEFAULT_ACTIVATING_TIME;
    int AlarmConfig::suspiciousTime = DEFAULT_SUSPICIOUS_TIME;
}
#endif
