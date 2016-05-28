#include "Arduino.h"
#include "idx_axis.h"
#include "stdlib.h" // for abs()

#if defined (__SAM3X8E__)
  // Defines Fastwrite functions for Arduino Due
  #define fastSet(pin) (digitalPinToPort(pin)->PIO_SODR |= digitalPinToBitMask(pin) ) 
  #define fastClear(pin) (digitalPinToPort(pin)->PIO_CODR |= digitalPinToBitMask(pin) )
#else
#import "digitalWriteFast.h"
#define fastSet(P) \
    if (__builtin_constant_p(P) && __builtin_constant_p(HIGH)) { \
        bitWrite(*digitalPinToPortReg(P), digitalPinToBit(P), (HIGH)); \
    } else { digitalWrite((P), (HIGH)); }
#define fastClear(P) \
    if (__builtin_constant_p(P) && __builtin_constant_p(LOW)) { \
        bitWrite(*digitalPinToPortReg(P), digitalPinToBit(P), (LOW)); \
    } else { digitalWrite((P), (LOW)); }


#endif

const uint8_t IDXAxis::stepPatterns[32][32] = { 
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // 0
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, // 1
            {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}, // 2
            {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1}, // 3
            {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1}, // 4
            {0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1}, // 5
            {0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}, // 6
            {0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,1}, // 7
            {0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1}, // 8
            {0,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,1,0,0,0,1,0,0,1}, // 9
            {0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1}, // 10
            {0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,1}, // 11
            {0,0,1,0,0,1,0,1,0,0,1,0,0,1,0,1,0,0,1,0,0,1,0,1,0,0,1,0,0,1,0,1}, // 12
            {0,0,1,0,1,0,0,1,0,1,0,0,1,0,1,0,0,1,0,1,0,0,1,0,1,0,0,1,0,1,0,1}, // 13
            {0,0,1,0,1,0,1,0,0,1,0,1,0,1,0,1,0,0,1,0,1,0,1,0,0,1,0,1,0,1,0,1}, // 14
            {0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1}, // 15
            {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1}, // 16
            {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1}, // 17
            {0,1,0,1,0,1,0,1,1,0,1,0,1,0,1,1,0,1,0,1,0,1,0,1,1,0,1,0,1,0,1,1}, // 18
            {0,1,0,1,0,1,1,0,1,0,1,1,0,1,0,1,1,0,1,0,1,1,0,1,0,1,1,0,1,0,1,1}, // 19
            {0,1,0,1,1,0,1,1,0,1,0,1,1,0,1,1,0,1,0,1,1,0,1,1,0,1,0,1,1,0,1,1}, // 20
            {0,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1}, // 21
            {0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,1}, // 22
            {0,1,1,0,1,1,1,0,1,1,0,1,1,1,0,1,1,0,1,1,1,0,1,1,0,1,1,1,0,1,1,1}, // 23
            {0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1}, // 24
            {0,1,1,1,0,1,1,1,1,0,1,1,1,0,1,1,1,1,0,1,1,1,0,1,1,1,1,0,1,1,1,1}, // 25
            {0,1,1,1,1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1,1,1,1,0,1,1,1,1,1}, // 26
            {0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1}, // 27
            {0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1}, // 28
            {0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1}, // 29
            {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // 30
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // 31
        };

IDXAxis::IDXAxis(int stepPin, int directionPin, int acceleration) {
    
    this->stepPin = stepPin;
    this->directionPin = directionPin;
    this->position = 0;
    this->velocity = 0;
    this->targetVelocity = 0;
    this->acceleration = acceleration;
    this->patternLine = 0;
    
}

void IDXAxis::begin() {
    
    pinMode(this->stepPin, OUTPUT);
    pinMode(this->directionPin, OUTPUT);
}

void IDXAxis::setVelocity(int targetVelocity) {
    
    this->targetVelocity = targetVelocity;
}


void IDXAxis::startTick(int tick) {
    
    this->tick = tick;
    
    if (this->stepPatterns[this->patternLine][this->tick & B11111]){
        fastSet(this->stepPin);
    }

        
    if (this->direction){
        fastSet(this->directionPin);
    } else {
        fastClear(this->directionPin);
    }
    
}

void IDXAxis::endTick() {
    
    fastClear(this->stepPin);
}

void IDXAxis::updateVelocity(){

    if (this->velocity < this->targetVelocity) {
        this->velocity += this->acceleration;
        
    } else if (this->velocity > this->targetVelocity) {
        this->velocity -= this->acceleration;
    }

    this->patternLine = abs(this->velocity);
    
    // patternLine either == velocity or it == -velocity,
    // so the equality check tells us the direction. 
    this->direction = this->patternLine == this->velocity ? IDXAxis::FORWARD : IDXAxis::REVERSE;
    
}


