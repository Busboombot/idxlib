
#include "idx_stepper.h"
#include "debug.h"

#include <FixedPoints.h>
#include <FixedPointsCommon.h>

void IDXStepperState::setParams(uint32_t segment_time, Direction direction, long n, unsigned long ca, int32_t x ){
    
    t = ((float)segment_time)/TIMEBASE;
    this->ca = ca;
    this->n = n;
    stepsLeft = abs(x);
    this->direction = direction;
    
}



void IDXStepperState::setParams(uint32_t segment_time, int32_t v0, int32_t v1, long x){
    // One of the parameters -- v, t or x, can be derived from the other 
    // two. All three are specified because there are some contraints; all 
    // three axes should complete the segment at
    // the same time ( so segment_time is specified once for all axes ) and 
    // each axis must travel an exact number of steps. The stepping always 
    // executes the given number of steps, but may take longer than 
    // specified and end up with a diferent final velocity. 
    
    float cn;
    float a;
            
    stepsLeft = abs(x);
    

    t = ((float)segment_time)/((float)TIMEBASE);
    
    if (v0==0 && v1==0){
        // Both velocitied are 0, so no movement
        a = 0;
        n = 0;
        cn = 0;
      
    } else if (v0 == v1){
        // Both velocities are the same, so constant speed motion
        a = 0;
        n = N_BIG;
        
        v0  = MAX( ((float)v0), MIN_VELOCITY);
        
        cn = ((float)TIMEBASE) / fabs(v0);
      
    } else  {
        
        /* This is the normal case for v0 == 0, 
           but we are limiting the minimum to MIN_VELOCITY, 
           so the case never  gets used */
        /*
        if (v0 == 0){
            
            // Starting from a stop, so always accelerating in one dir or the 
            // other
            v1  = FORCE(v1);

            a = fabs(v1) / t;
            n = 0; // n will always be positive, so accelerating
            cn = 0.676 * sqrt(2.0 / fabs(a)) * ((float)TIMEBASE); // c0 in Equation 
        }   
        */
 
        v0  = MAX( ((float)v0), MIN_VELOCITY);
        v1  = MAX( ((float)v1), MIN_VELOCITY);
        
        // Normal acceleration case
        a = fabs(v1-v0) / t;
        n = abs((long) ( (v0 * v0) / (2.0 * a))); // Eq 16
        cn = ((float)TIMEBASE) / fabs(v0);
       
       
        // Need to put the sign back on n; n must be negative for deceleration
        if (fabs(v1) <fabs(v0)){
            n = -n;
        }  
    }
    

    if (x > 0){
        direction = CW;
    } else if (x < 0){
        direction = CCW;
    } else {
        direction = STOP;
    }
    
    // Keeps cn within the bit range. 
    //if (cn > (1<<FP_BITS)){
    //    cn = (1<<FP_BITS);
    //}
    

    // ca is cn times the alpha factor, to make it fixed point
    
    ca =  (long) cn*(1<<FP_BITS);

    delay = ca>>FP_BITS;

    lastTime = micros();
}

long IDXStepperState::stepMaybe(uint32_t now,  IDXStepInterface& stepper,int &activeAxes ){
   
    // Shifting by FP_BITS+1 to take 1/2 of the time delay; the other half 
    // is for clearing the bit. 

    if ( stepsLeft !=0 && ( (unsigned long)(now - lastTime)   >  delay ) ) {
        
        long ca1;
    
        if (n != N_BIG){ // No need to update ca if velocity is constant
            n++;
            ca1 = ( (ca*2) / ((n*4) + 1));
            ca = ca - ca1;
            delay = ca>>FP_BITS;
        }
        
        if (pinState){
            stepper.writeStep();
            pinState = false;
            stepsLeft--;
        } else {
            stepper.clearStep();
            pinState = true;
        }
        
        if (lastTime == 0) {
            lastTime = now;
        } else {
            lastTime = now; // = now; 
        }
    } 
    
    if (stepsLeft)
        activeAxes += 1;
    
    return stepsLeft;
}