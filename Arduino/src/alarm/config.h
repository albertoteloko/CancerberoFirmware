#ifndef ALARM_CONFIG_H
#define ALARM_CONFIG_H

#include <EEPROM.h>
#include "../common.h"
#include "../log.h"

#define SOURCE_LENGTH                       50

#define DEFAULT_STATUS_SOURCE               "Unknown"
#define ALARM_CONFIG_TAG                    "AlarmConfig"


static const unsigned int PIN_NUMBER = sizeof(PINS) / sizeof(AlarmPin);

namespace {

    struct AlarmConfigStruct {
        AlarmStatus status = AS_IDLE;
        char statusSource[SOURCE_LENGTH] = DEFAULT_STATUS_SOURCE;
    };


    class AlarmConfig {
    private:
        static AlarmConfigStruct config;
    public:
        static void forEachPin(void (*f)(int, AlarmPin)) {
            for (int i = 0; i < PIN_NUMBER; ++i) {
                (*f)(i, PINS[i]);
            }
        }

        static void forEachDefinedPin(void (*f)(int, AlarmPin)) {
            for (int i = 0; i < PIN_NUMBER; ++i) {
                if ((PINS[i].id != PI_UNKNOWN) && (PINS[i].type != PT_UNKNOWN) && (PINS[i].mode != PM_UNKNOWN)) {
                    (*f)(i, PINS[i]);
                }
            }
        }

        static AlarmStatus getStatus() {
            return config.status;
        }


        static String getStatusSource() {
            return String(config.statusSource);
        }


        static void setStatus(AlarmStatus newStatus, const char source[SOURCE_LENGTH]) {
            config.status = newStatus;

            for (unsigned int i = 0; i < SOURCE_LENGTH; ++i) {
                config.statusSource[i] = source[i];
            }
            save();
        }

        static void clear() {
            config.status = AS_IDLE;

            for( unsigned int i = 0; i < SOURCE_LENGTH;  ++i ){
                config.statusSource[i] = 0;
            }

            for( unsigned int i = 0; i < sizeof(DEFAULT_STATUS_SOURCE);  ++i ){
                config.statusSource[i] = DEFAULT_STATUS_SOURCE[i];
            }

            save();
        }

        static void save() {
            EEPROM.put(0, config);
        }

        static void load() {
            EEPROM.get(0, config);

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
