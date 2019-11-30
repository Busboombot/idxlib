
#ifndef AccelStepper_h
#define AccelStepper_h

#include <Arduino.h>

#include "fastset.h"

#include <stdlib.h>     /* abs */
#include <math.h>       /* sqrt and fabs  */
#include <limits.h>     /* LONG_MAX */


// A Big n for small a. Any really big number will do, since cn doesn't change much
// could be LONG_MAX, but a round number makes debugging easier. 
#define N_BIG 2000000000

class IDXStepGenerator {
    
    
protected:

    uint8_t axis_n; 
    
    int8_t direction; 
    
    int32_t stepsLeft;
    uint32_t lastTime;
   
    uint32_t startTime;
   
    uint32_t position = 0;
   
    float t=0;
    int16_t v0;
    int16_t v1;

    float a;
    long n;
    float cn;
    
public:

    typedef enum
    {
	    CCW = -1,  ///< Clockwise
        STOP = 0,  ///< Clockwise
        CW  = 1   ///< Counter-Clockwise
    } Direction;
    

    IDXStepGenerator(uint8_t axis_n)  : axis_n(axis_n) {
    
        direction = CW;
        lastTime = 0;

        t = 0;
        v0 =0;
        v1 = 0;
        stepsLeft = 0;
        
        a = 0;
        n = 0;
        cn = 0.0;

    }
    
    inline void setParams(uint32_t now, uint32_t segment_time, int16_t v0, int16_t v1, long x){
        
        #if(DEBUG_PRINT_ENABLED )
        if (axis_n == 0){
            Serial.print(" P1 n="); Serial.print(n); 
            Serial.print(" cn="); Serial.print(cn); 
            Serial.print(" a="); Serial.print(a); 
            Serial.print(" x="); Serial.print(stepsLeft); 
            Serial.print(" t="); Serial.print(t*1000000.0); 
            Serial.println(" ");
            
        }
        #endif
        
        this->v0 = v0;
        this->v1 = v1;
        
        stepsLeft = abs(x);
        
        lastTime = now;
        startTime = now;
        
        t = ((float)segment_time)/1000000.0;
        
        if (v0==0 && v1==0){
            a = 0;
            n = 0;
            cn = 0;
            stepsLeft = 0;
        } else if (v0==0) {
            a = fabs((float)v1) / t;
            n = 0; // n will always be positive, so accelerating
            cn = 0.676 * sqrt(2.0 / abs(a)) * 1000000.0; // c0 in Equation 15
        } else if (v0 == v1){
            a = 0;
            n = N_BIG;
            cn = 1000000.0 / abs(v0);
        } else {
            a = fabs((float)v1-(float)v0) / t;
            n = abs((long) ( ((float)v0 * (float)v0) / (2.0 * a))); // Equation 16
            cn = 1000000.0 / abs(v0);
            
            // Need to put the sign back on n; n must be negative for deceleration
            if (abs(v1) < abs(v0)){
                n = -n;
            }
        }
        
        #if(DEBUG_PRINT_ENABLED )
        if (axis_n == 0){
            Serial.print(" P2 n="); Serial.print(n); 
            Serial.print(" cn="); Serial.print(cn); 
            Serial.print(" a="); Serial.print(a); 
            Serial.print(" x="); Serial.print(stepsLeft); 
            Serial.print(" t="); Serial.print(t*1000000.0); 
            Serial.println(" ");
            
        }
        #endif
        
        if (x > 0){
            setDirection(CW);
        } else if (x < 0){
            setDirection(CCW);
        } else {
            setDirection(STOP);
        }

    }
    
    inline uint32_t getStepsLeft(){
        return stepsLeft;
    }
    
    inline uint32_t getPosition(){
        return position;
    }
    
    virtual  void setDirection(Direction dir);
    
    inline long stepAlways(uint32_t now){
        
        // cn is always positive, but n can be negative. n is always stepped +1, 
        // so a negative n causes cn to get larger each step ->  deceleration
        // a positive n causes cn to get smaller each step -> acceleration
        
        cn = fabs( (float)cn - ( (2.0 * (float)cn) / ((4.0 * (float)n) + 1.0))); // Why is it going negative?
        
        stepsLeft -= 1;
        n += 1;

        writeStep();
        
        lastTime = now;
        
        position += direction;

        return stepsLeft;
    }
    
    inline long stepMaybe(uint32_t now){
       
        if ( stepsLeft != 0 && ( (unsigned long)(now - lastTime)   > cn) ) {
            stepAlways(now);
        }
        
        
        return stepsLeft;
    }
    
    virtual inline void writeStep(){
        ;
    }
	
    virtual inline void clearStep(){
        ;
    }
	
protected:

};

class IDXStepper : public IDXStepGenerator
{

protected: 
    uint8_t stepPin;
    uint8_t directionPin;


public: 
    IDXStepper(uint8_t axis_n, uint8_t stepPin, uint8_t directionPin) 
        : IDXStepGenerator(axis_n), stepPin(stepPin), directionPin(directionPin) {
            
        pinMode(stepPin, OUTPUT);
        pinMode(directionPin, OUTPUT);
    }
    
public: 
    inline void writeStep(){
        fastSet(stepPin);
    }
   
    inline void clearStep(){
        fastClear(stepPin);
    }
    
    inline void setDirection(Direction dir){
        
        direction = dir;
        
        if (dir == CW){
            fastSet(directionPin);
        } else {
            fastClear(directionPin);
        }
    }
    
    
};

#endif 
