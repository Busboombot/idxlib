/*
 *  Class for an IDX Robot Axis
 */

#ifndef  idx_axis_h
#define idx_axis_h


class IDXAxis {
    
    public:
        
        // TODO These should be variables that are configarable to choose which direction 
        // is forward for the axis
        const uint8_t FORWARD = 1;
        const uint8_t REVERSE = 0;
        
        IDXAxis(int stepPin, int directionPin, int acceleration=1);
        
        void begin();
        
        void setVelocity(int targetVelocity);
        
        void updateVelocity();
        
        void startTick(int tick);
        
        void endTick();
        
    private:
        
        
        
        static const uint8_t stepPatterns[32][32];
        
        uint8_t stepPin; //Step pin for Stepper controller
        
        uint8_t directionPin; //Direction pin for Stepper controller
        
        int tick; // Current timestep
        
        int position; // Step position
        
        int velocity; // Current Velocity
        
        int acceleration; 
        
        int targetVelocity; // Current Velocity
        
        int direction; // Current Direction where 1 is clockwise and 0 is counter clockwise
        
        int patternLine; // Pattern Line

};


#endif