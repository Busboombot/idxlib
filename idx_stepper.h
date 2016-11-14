
#ifndef AccelStepper_h
#define AccelStepper_h

#include <Arduino.h>

#define fastSet(pin) (digitalPinToPort(pin)->PIO_SODR |= digitalPinToBitMask(pin) ) 
#define fastClear(pin) (digitalPinToPort(pin)->PIO_CODR |= digitalPinToBitMask(pin) )


class IDXStepper
{
public:

    typedef enum
    {
	    CCW = -1,  ///< Clockwise
        CW  = 1   ///< Counter-Clockwise
    } Direction;
    


    IDXStepper(uint8_t stepPin, uint8_t directionPin) {
    
        direction = CCW;

        stepPin = stepPin;
        directionPin = directionPin;

        lastTime = 0;

        n = 0;
        cn = 0.0;
        
        pinMode(stepPin, OUTPUT);
        pinMode(directionPin, OUTPUT);

    }
    
    
    inline void setDir(Direction dir) {
        if (dir){
            fastSet(directionPin);
        } else {
            fastClear(directionPin);
        }
    }
    
    inline void setParams(long n, float cn, long stepsLeft){
        this->stepsLeft = stepsLeft;
        this->direction = (stepsLeft > 0) ? 1 : ((stepsLeft < 0) ? -1 : 0);
        this->n = n;
        this->cn = cn;
    }
    

    inline long step(uint32_t now){
        
        if ( stepsLeft != 0 && ( (unsigned long)(now - lastTime)   > cn) ) {
            
            cn = cn - ( (2.0 * cn) / ((4.0 * n) + 1));  
            
            stepsLeft -= 1;
            n += 1;

            fastSet(stepPin);
            lastTime = now;
            

        }
        
        return stepsLeft;
    }
    
    //Set the step line to low
    inline void clearStep(){
        
        fastClear(stepPin);
        
    }
    

protected:



private:

    uint8_t  stepPin;
    uint8_t directionPin;

    byte direction; 
    long stepsLeft;
    uint32_t lastTime;
   
    long n; // Step counter for interval calculations
    float cn; // Interval, in microseconds




};

#endif 
