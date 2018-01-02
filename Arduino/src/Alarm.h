#ifndef ALARM_H
#define ALARM_H

// Arduino versioning.
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// See http://code.google.com/p/arduino/issues/detail?id=246
#if defined(ARDUINO) && ARDUINO < 101
#define INPUT_PULLUP INPUT
#endif



#define LED_PIN 6
#define SPEAKER_PIN 7
#define ADDRESS_SELECT_PIN 8
#define ALARM_PIN 9

// STATUS 
#define IDDLE_STATE 1
#define ACTIVATING_STATE 2
#define ACTIVATED_STATE 3
#define SUSPICIUS_STATE 4
#define ALARMED_STATE 5
#define SAFETY_STATE 6

#define NULL_MELODY 200

#define GET_STATUS_COMMAND 1
#define SET_STATUS_COMMAND 2

#endif


