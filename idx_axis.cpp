#include "Arduino.h"
#include "idx_axis.h"

IDXAxis::IDXAxis(int stp_pin, int dir_pin) {
    
    this->stp_pin = stp_pin;
    
    this->dir_pin = dir_pin;
    
    pinMode(this->stp_pin, OUTPUT);
    
    pinMode(this->dir_pin, OUTPUT);
    
};

void IDXAxis::setVelocity(int tar_vel) {
    
    this->tar_vel = tar_vel;
    
    // Set Velocity
    if (this->cur_vel < this->tar_vel) {
        
        this->cur_vel += ACCEL;
        
    }
    
    if (this->cur_vel > this->tar_vel) {
        
        this->cur_vel -= ACCEL;
        
    }
    
    // Snap to target velocity if within tolerance
    if (this->cur_vel < this->tar_vel + VEL_SNAP_TOL && this->cur_vel > this->tar_vel - VEL_SNAP_TOL) {
        
        this->cur_vel = this->tar_vel;
        
    }
    
    // Set Direction
    if (this->cur_vel > 0) {
        
        this->cur_dir = 1;
        
        this->pat_line = this->cur_vel;
        
    }
    
    if (this->cur_vel < 0) {
        
        this->cur_dir = 0;
        
        this->pat_line = (this->cur_vel * -1);
        
    }
    
};

void IDXAxis::startTick(int tick) {
    
    this->pat_slot = tick;
    
    
    
}