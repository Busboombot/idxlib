// AccelStepper.cpp
//
// Copyright (C) 2009-2013 Mike McCauley
// $Id: AccelStepper.cpp,v 1.21 2015/08/25 04:57:29 mikem Exp mikem $

#include "idx_stepper.h"

#if 0
// Some debugging assistance
void dump(uint8_t* p, int l)
{
    int i;

    for (i = 0; i < l; i++)
    {
	Serial.print(p[i], HEX);
	Serial.print(" ");
    }
    Serial.println("");
}
#endif

// NOTE! These only work on the SAM3X, or possibly other ARM Chips, but certainly the Arduino DUE. 
#define fastSet(pin) (digitalPinToPort(pin)->PIO_SODR |= digitalPinToBitMask(pin) ) 
#define fastClear(pin) (digitalPinToPort(pin)->PIO_CODR |= digitalPinToBitMask(pin) )

AccelStepper::AccelStepper(uint8_t stepPin, uint8_t directionPin) {
    
  _currentPos = 0;
  _targetPos = 0;
  _speed = 0.0;
  _maxSpeed = 1.0;
  _acceleration = 0.0;
  _sqrt_twoa = 1.0;
  _stepInterval = 0;
  _minPulseWidth = 4;
  
  _stepPin = stepPin;
  _directionPin = directionPin;
  
  _lastStepTime = 0;
  
  // NEW
  _n = 0;
  _c0 = 0.0;
  _cn = 0.0;
  _cmin = 1.0;
  _direction = DIRECTION_CCW;
  
  pinMode(_stepPin, OUTPUT);
  pinMode(_directionPin, OUTPUT);

  setAcceleration(1);
}

void AccelStepper::moveTo(long absolute) {
  if (_targetPos != absolute) {
    _targetPos = absolute;
    computeNewSpeed();
    // compute new n?
  }
}


// Implements steps according to the current step interval
// You must call this at least once per step
// returns true if a step occurred
boolean AccelStepper::runSpeed() {
    
  // Dont do anything unless we actually have a step interval
  if (!_stepInterval)
    return false;

  unsigned long time = micros();
  
  if( (unsigned long)(time - _lastStepTime)   > _stepInterval ){
           
    if (_direction == DIRECTION_CW) {
      // Clockwise
      _currentPos += 1;
    } else {
      // Anticlockwise
      _currentPos -= 1;
    }

    if (_direction){
        fastSet(_directionPin);
    } else {
        fastClear(_directionPin);
    }
    
    
    fastSet(_stepPin);
    delayMicroseconds(_minPulseWidth);
    fastClear(_stepPin);

    _lastStepTime = time;
    return true;
  } else {
    return false;
  }
}


boolean AccelStepper::run() {
  if (runSpeed())
    computeNewSpeed();
  
  return _speed != 0.0 || _targetPos != _currentPos;
}

void AccelStepper::computeNewSpeed() {
    
  long distanceTo = _targetPos - _currentPos; // +ve is clockwise from curent location

  long stepsToStop =
      (long)((_speed * _speed) / (2.0 * _acceleration)); // Equation 16

  if (distanceTo == 0 && stepsToStop <= 1) {
    // We are at the target and its time to stop
    _stepInterval = 0;
    _speed = 0.0;
    _n = 0;
    return;
  }

  if (distanceTo > 0) {
    // We are anticlockwise from the target
    // Need to go clockwise from here, maybe decelerate now
    if (_n > 0) {
      // Currently accelerating, need to decel now? Or maybe going the wrong
      // way?
      if ((stepsToStop >= distanceTo) || _direction == DIRECTION_CCW)
        _n = -stepsToStop; // Start deceleration
    } else if (_n < 0) {
      // Currently decelerating, need to accel again?
      if ((stepsToStop < distanceTo) && _direction == DIRECTION_CW)
        _n = -_n; // Start accceleration
    }
  } else if (distanceTo < 0) {
    // We are clockwise from the target
    // Need to go anticlockwise from here, maybe decelerate
    if (_n > 0) {
      // Currently accelerating, need to decel now? Or maybe going the wrong
      // way?
      if ((stepsToStop >= -distanceTo) || _direction == DIRECTION_CW)
        _n = -stepsToStop; // Start deceleration
    } else if (_n < 0) {
      // Currently decelerating, need to accel again?
      if ((stepsToStop < -distanceTo) && _direction == DIRECTION_CCW)
        _n = -_n; // Start accceleration
    }
  }

  // Need to accelerate or decelerate
  if (_n == 0) {
    // First step from stopped
    _cn = _c0;
    _direction = (distanceTo > 0) ? DIRECTION_CW : DIRECTION_CCW;
  } else {
    // Subsequent step. Works for accel (n is +_ve) and decel (n is -ve).
    _cn = _cn - ((2.0 * _cn) / ((4.0 * _n) + 1)); // Equation 13
    _cn = max(_cn, _cmin);
  }
  _n++;
  _stepInterval = _cn;
  _speed = 1000000.0 / _cn;
  
  if (_direction == DIRECTION_CCW)
    _speed = -_speed;

}


void AccelStepper::setMaxSpeed(float speed) {
  if (_maxSpeed != speed) {
    _maxSpeed = speed;
    _cmin = 1000000.0 / speed;
    // Recompute _n from current speed and adjust speed if accelerating or
    // cruising
    if (_n > 0) {
      _n = (long)((_speed * _speed) / (2.0 * _acceleration)); // Equation 16
      computeNewSpeed();
    }
  }
}

void AccelStepper::setAcceleration(float acceleration) {
  if (acceleration == 0.0)
    return;
  if (_acceleration != acceleration) {
    // Recompute _n per Equation 17
    _n = _n * (_acceleration / acceleration);
    // New c0 per Equation 7, with correction per Equation 15
    _c0 = 0.676 * sqrt(2.0 / acceleration) * 1000000.0; // Equation 15
    _acceleration = acceleration;
    computeNewSpeed();
  }
}

void AccelStepper::setSpeed(float speed) {
  if (speed == _speed)
    return;
  speed = constrain(speed, -_maxSpeed, _maxSpeed);
  if (speed == 0.0)
    _stepInterval = 0;
  else {
    _stepInterval = fabs(1000000.0 / speed);
    _direction = (speed > 0.0) ? DIRECTION_CW : DIRECTION_CCW;
  }
  _speed = speed;
}




void AccelStepper::stop() {
  if (_speed != 0.0) {
    long stepsToStop = (long)((_speed * _speed) / (2.0 * _acceleration)) + 1; // Equation 16 (+integer rounding)
    if (_speed > 0)
      move(stepsToStop);
    else
      move(-stepsToStop);
  }
}


