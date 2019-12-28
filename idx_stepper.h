
#ifndef idx_stepper_h
#define idx_stepper_h

#include <Arduino.h>

#include "fastset.h"

#include <stdlib.h>     /* abs */
#include <math.h>       /* sqrt and fabs  */
#include <limits.h>     /* LONG_MAX */

typedef enum
{
    CCW = -1,  ///< Clockwise
    STOP = 0,  ///< Clockwise
    CW  = 1   ///< Counter-Clockwise
} Direction;

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// Minimum non zero velocity
// The largest values for  cn are at very low speeds. A speed of 1 pulse/sec
// results in an initial cn of about 1M for a timebase of microsecs, 
// or 1B for a timebase of nanoseconds.  If the time base is limited to 
// usecs, and the minimum speed is 1 RPM with 10usteps, the min pulse rate is 
// 33 pulse/sec, which results in a max cn of 30K and requires 15 bits.
// If the minimum rate for a usec timebase is 12 step/sec, we need to reserve 
// 16 bits. 

// TODO. 16 bits is sufficient to hold the base delay rate, so the command
// messages could have 16 bits. 

#define MIN_VELOCITY 40 // Steps per second

// Number of bits in ca to use for the fixed-point fraction portion.  
#define FP_BITS 8

// 2,000 rpm for a 1.8deg stepper is 400,000 steps per min, 7K steps per sec
// For a 10 ustep driver, 70KHz step pulse. 

// A Big n for small a.
#define N_BIG LONG_MAX

#define TIMEBASE 1000000.0 // Microseconds

class IDXStepInterface {
    
protected:
    
    bool pinState = true;
    
    uint8_t stepPin;
    uint8_t directionPin;
    uint8_t enablePin;

public:
    
    inline IDXStepInterface(uint8_t axis_n, uint8_t stepPin, uint8_t directionPin, uint8_t enablePin): 
    stepPin(stepPin), directionPin(directionPin), enablePin(enablePin) {
        pinMode(stepPin, OUTPUT);
        pinMode(directionPin, OUTPUT);
        pinMode(enablePin, OUTPUT);
    }
    
    inline void writeStep(){
        fastSet(stepPin);
    }
   
    inline void clearStep(){
        fastClear(stepPin);
    }
    
    
    inline void enable(){
        fastClear(enablePin);  // Active low 
    }
    
    inline void enable(Direction dir){
        fastClear(enablePin);  // Active low 
        setDirection(dir);
    }
    
    inline void disable(){
        fastSet(enablePin); // Active low
    }
    
    inline void setDirection(Direction dir){
        
        if (dir == CW){
            fastSet(directionPin);
        } else {
            fastClear(directionPin);
        }
    }
    
    
        
};

class IDXStepperState {
    
public: 
    
    long n=0;
    unsigned long ca=0;     
    
    Direction direction=STOP; 

    int32_t stepsLeft=0;
    
    float t=0;
   
    bool pinState=true;
    unsigned long lastTime = 0;
  
public:

    IDXStepperState() {}
    
    inline uint32_t getStepsLeft(){
        return stepsLeft;
    }
    
    void setParams(uint32_t segment_time, Direction direction, long n, unsigned long ca, int32_t x );
        
    void setParams(uint32_t segment_time, int32_t v0_, int32_t v1_, long x);
        
    long stepMaybe(uint32_t now, IDXStepInterface& stepper);
        
};


class Segment {
public:
    uint16_t seq = 0;
    uint16_t code = 0;
    uint32_t segment_time = 0; // total segment time, in microseconds 
    uint32_t lastTime;
    IDXStepperState axes[6];
public:
    Segment() {
        
    }
};



#endif 
