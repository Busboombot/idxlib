
#ifndef idx_stepper_h
#define idx_stepper_h

#include <Arduino.h>

#include "fastset.h"

#include <stdlib.h>     /* abs */
#include <math.h>       /* sqrt and fabs  */
#include <limits.h>     /* LONG_MAX */

#include <FixedPoints.h>
#include <FixedPointsCommon.h>

typedef enum
{
    CCW = -1,  ///< Clockwise
    STOP = 0,  ///< Clockwise
    CW  = 1   ///< Counter-Clockwise
} Direction;

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// Minimum non zero velocity of 1 RPM @ 10µsteps is 40 pulse/sec,
// or a delay of 6,400,000 𝛍sec, which requires 23 bits. A min of 80 pulse/sec
// requires 23 bits.  With 8 bits fixed, this leave 31 bits, so we can 
// fit into a signed long. 


#define MIN_VELOCITY 40 // Steps per second

// Number of bits in ca to use for the fixed-point fraction portion.  
#define FP_BITS 8

// 2,000 rpm for a 1.8deg stepper is 400,000 steps per min, 7K steps 
// per sec. For a 10 ustep driver, 70KHz step pulse. 

// A Big n for small a.
#define N_BIG LONG_MAX

#define TIMEBASE 1000000.0 // Microseconds

class IDXStepInterface {
    
protected:
    
    uint8_t axis_n;
    bool pinState = true;
    
    uint8_t stepPin;
    uint8_t directionPin;
    uint8_t enablePin;

public:
    
    inline IDXStepInterface(uint8_t axis_n, uint8_t stepPin, uint8_t directionPin, 
     uint8_t enablePin): axis_n(axis_n), stepPin(stepPin), directionPin(directionPin), enablePin(enablePin) {
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
    
    inline void setMaskBit(uint8_t &mask){
        mask |= 1<<axis_n;
    }
    
    inline void enable(){
        fastClear(enablePin);  // Active low 
    }
    
    inline void enable(Direction dir){
        if(dir != STOP){
            fastClear(enablePin);  // Active low 
        }
        setDirection(dir);
    }
    
    inline void disable(){
        fastSet(enablePin); // Active low
        setDirection(STOP);
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
    
    long n=0; // Step delay number, part of the step algorithm

    long ca=0;     // Step delay, in fixed point format
    
    unsigned long delay; // 1/2 Step delay, shifted portion ( ca<<(FP_BITS+1)) 
    
    Direction direction=STOP; 

    unsigned long stepsLeft=0;
    
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
        
    unsigned long stepMaybe(uint32_t now, IDXStepInterface& stepper, uint8_t& mask);
        
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
