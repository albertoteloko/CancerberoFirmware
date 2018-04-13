#ifndef ALARM_CONFIG_H
#define ALARM_CONFIG_H

#include "../common/common.h"
#include "../log.h"
#include <EEPROM.h>

#define ALARM_DEFINITION_EEPROM_ADDRESS     0

#define DEFAULT_ACTIVATING_TIME             60000
#define DEFAULT_SUSPICIOUS_TIME             30000

#define ALARM_CONFIG_TAG                    "AlarmConfig"

namespace {

    struct AlarmPin {
        PinIds id;
        PinType type = PT_UNKNOWN;
        DPinMode mode = PM_UNKNOWN;
    };

    struct AlarmConfigStruct {
        AlarmStatus status = AS_IDLE;
        PinIds statusSource = PI_UNKNOWN;
        int activatingTime = DEFAULT_ACTIVATING_TIME;
        int suspiciousTime = DEFAULT_SUSPICIOUS_TIME;
        AlarmPin pins[MASTER_PIN_NUMBER];
    };


    class AlarmConfig {
    private:
        static AlarmConfigStruct config;
    public:
        static void set(long newActivatingTime, long newSuspiciousTime, AlarmPin newPins[MASTER_PIN_NUMBER]) {
            config.activatingTime = newActivatingTime;
            config.suspiciousTime = newSuspiciousTime;

            for (int i = 0; i < MASTER_PIN_NUMBER; ++i) {
                config.pins[i] = newPins[i];
            }

            save();
        }

        static void forEachPin(void (*f)(int, AlarmPin)) {
//            for (int i = 0; i < MASTER_PIN_NUMBER; ++i) {
//                (*f)(i, config.pins[i]);
//            }
        }

        static void forEachDefinedPin(void (*f)(int, AlarmPin)) {
//            for (int i = 0; i < MASTER_PIN_NUMBER; ++i) {
//                if ((config.pins[i].id != PI_UNKNOWN) && (config.pins[i].type != PT_UNKNOWN) && (config.pins[i].mode != PM_UNKNOWN)) {
//                    (*f)(i, config.pins[i]);
//                }
//            }
        }

        static int activatingTime() {
            return config.activatingTime;
        }

        static int suspiciousTime() {
            return config.suspiciousTime;
        }

        static AlarmStatus getStatus() {
            return config.status;
        }


        static PinIds getStatusSource() {
            return config.statusSource;
        }


        static void setStatus(AlarmStatus newStatus, PinIds source) {
            config.status = newStatus;
            config.statusSource = source;

            EEPROM.put(0, config.status);
            EEPROM.put(1, config.statusSource);
        }

        static void clear() {
            config = AlarmConfigStruct();
            save();
            load();
        }

        static void save() {
            EEPROM.put(0, config);
        }

        static void load() {
            EEPROM.get(0, config);

            if (config.status == AS_UNKNOWN) {
                error(ALARM_CONFIG_TAG, "No status, setting idle one");
                config.status = AS_IDLE;
            }

            setStatus(config.status, config.statusSource);
        }

    };

    AlarmConfigStruct AlarmConfig::config = AlarmConfigStruct();
}
#endif
