#ifndef ALARM_CONFIG_H
#define ALARM_CONFIG_H

#include <EEPROM.h>
#include "../common/common.h"
#include "../log.h"

#define ALARM_DEFINITION_EEPROM_ADDRESS     0

#define DEFAULT_STATUS_SOURCE               "Setup"
#define DEFAULT_ACTIVATING_TIME             60000
#define DEFAULT_SUSPICIOUS_TIME             30000
#define SOURCE_LENGTH                       50

const String ALARM_CONFIG_TAG = "AlarmConfig";

namespace {

    struct AlarmPin {
        PinIds id = PI_UNKNOWN;
        PinType type = PT_UNKNOWN;
        DPinMode mode = PM_UNKNOWN;
        DPinInput input = PIN_UNKNOWN;
        unsigned char threshold;
    };

    struct AlarmConfigStruct {
        bool enabled = false;
        AlarmStatus status = AS_IDLE;
        char statusSource[SOURCE_LENGTH] = DEFAULT_STATUS_SOURCE;
        int activatingTime = DEFAULT_ACTIVATING_TIME;
        int suspiciousTime = DEFAULT_SUSPICIOUS_TIME;
        AlarmPin pins[MASTER_PIN_NUMBER];
    };


    class AlarmConfig {
    private:
        static AlarmConfigStruct config;
    public:
        static void set(long newActivatingTime, long newSuspiciousTime, AlarmPin newPins[MASTER_PIN_NUMBER]) {
            config.enabled = true;
            config.status = AS_ACTIVATED;
            config.activatingTime = newActivatingTime;
            config.suspiciousTime = newSuspiciousTime;

            for (int i = 0; i < MASTER_PIN_NUMBER; ++i) {
                config.pins[i] = newPins[i];
            }

            save();
        }

        static void forEachPin(void (*f)(int, AlarmPin)) {
            for (int i = 0; i < MASTER_PIN_NUMBER; ++i) {
                (*f)(i, config.pins[i]);
            }
        }

        static void forEachDefinedPin(void (*f)(int, AlarmPin)) {
            for (int i = 0; i < MASTER_PIN_NUMBER; ++i) {
                if ((config.pins[i].id != PI_UNKNOWN) && (config.pins[i].type != PT_UNKNOWN) && (config.pins[i].mode != PM_UNKNOWN)) {
                    (*f)(i, config.pins[i]);
                }
            }
        }

        static bool enabled() {
            return config.enabled;
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


        static String getStatusSource() {
            return String(config.statusSource);
        }


        static void setStatus(AlarmStatus newStatus, const char source[SOURCE_LENGTH]) {
            config.status = newStatus;

            for (int i = 0; i < SOURCE_LENGTH; ++i) {
                config.statusSource[i] = source[i];
            }
            save();
        }

        static void clear() {
            config.enabled = false;
            config.status = AS_IDLE;

            for( int i = 0; i < SOURCE_LENGTH;  ++i ){
                config.statusSource[i] = 0;
            }

            for( int i = 0; i < sizeof(DEFAULT_STATUS_SOURCE);  ++i ){
                config.statusSource[i] = DEFAULT_STATUS_SOURCE[i];
            }
            config.activatingTime = DEFAULT_ACTIVATING_TIME;
            config.suspiciousTime = DEFAULT_SUSPICIOUS_TIME;

            for( int i = 0; i < MASTER_PIN_NUMBER;  ++i ){
                config.pins[i].id = PI_UNKNOWN;
                config.pins[i].type = PT_UNKNOWN;
                config.pins[i].mode = PM_UNKNOWN;
                config.pins[i].input = PIN_UNKNOWN;;
                config.pins[i].threshold = -1;
            }

            save();
        }

        static void save() {
            EEPROM.put(ALARM_DEFINITION_EEPROM_ADDRESS, config);
        }

        static void load() {
            EEPROM.get(ALARM_DEFINITION_EEPROM_ADDRESS, config);

            if (toAlarmStatus(fromAlarmStatus(config.status))  == AS_UNKNOWN) {
                error(ALARM_CONFIG_TAG, "No status, setting idle one");
                config.status = AS_IDLE;
            }

            setStatus(config.status, config.statusSource);
        }

    };

    AlarmConfigStruct AlarmConfig::config = AlarmConfigStruct();
}
#endif
