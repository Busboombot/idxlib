// Debugging configuration

#include "fastset.h"

#ifndef idx_debug_h
#define idx_debug_h

#if !defined(DEBUG_PRINT_ENABLED) 
#define DEBUG_PRINT_ENABLED false
#endif
#if !defined(DEBUG_TICK_ENABLED) 
#define DEBUG_TICK_ENABLED false
#endif

// Set to True to record the time required to read characters
// from the serial port
#define RECORD_CHAR_TIMES false

// Set to True to record the time required to run a setp loop
#define RECORD_LOOP_TIMES false

// Debug tick pins
#define LOOP_COMM_TICK_PIN 14 
#define LOOP_CLEAR_TICK_PIN 15 
#define LOOP_STEP_TICK_PIN 16 
#define PARAMS_TICK_PIN 17
#define STARVED_TICK_PIN 18
#define RESPONSE_TICK_PIN 19
#define CMD_RUN_TICK_PIN 20 
#define MSG_RECIEVE_TICK_PIN 21 

#define EXTRA_DEBUG_TICK_PIN 22

#define SP(x) Serial.print(x);Serial.print(' ');
#define SPT(x) Serial.print(x);Serial.print('\t');
#define NL Serial.println(' ');

#if(DEBUG_TICK_ENABLED)
  #define fastDebugSet(pin) fastSet(pin)  
  #define fastDebugClear(pin) fastClear(pin) 

inline void init_debug(){
    pinMode(LOOP_COMM_TICK_PIN, OUTPUT); 
    pinMode(LOOP_STEP_TICK_PIN, OUTPUT); 
    pinMode(LOOP_CLEAR_TICK_PIN, OUTPUT); 
    pinMode(PARAMS_TICK_PIN, OUTPUT); 
    pinMode(STARVED_TICK_PIN, OUTPUT); 
    pinMode(RESPONSE_TICK_PIN, OUTPUT); 
    pinMode(CMD_RUN_TICK_PIN, OUTPUT); 
    pinMode(MSG_RECIEVE_TICK_PIN, OUTPUT); 
    pinMode(EXTRA_DEBUG_TICK_PIN, OUTPUT);
}


#else
  #define fastDebugSet(pin) ;
  #define fastDebugClear(pin) ;
inline void init_debug(){}
#endif

#endif