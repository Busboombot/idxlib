
#ifndef AccelStepper_h
#define AccelStepper_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#undef round

class AccelStepper
{
public:


    AccelStepper(uint8_t stepPin = 2, uint8_t DirectionPin = 3);

    void    move(long relative);

    void    moveTo(long absolute);

    boolean run();

    boolean runSpeed();

    void    setMaxSpeed(float speed);

    void    setAcceleration(float acceleration);

    void    setSpeed(float speed);

    
    void stop();

    void           computeNewSpeed();

protected:

    /// \brief Direction indicator
    /// Symbolic names for the direction the motor is turning
    typedef enum
    {
	    DIRECTION_CCW = 0,  ///< Clockwise
        DIRECTION_CW  = 1   ///< Counter-Clockwise
    } Direction;
    

private:
    /// Number of pins on the stepper motor. Permits 2 or 4. 2 pins is a
    /// bipolar, and 4 pins is a unipolar.
    uint8_t        _interface;          // 0, 1, 2, 4, 8, See MotorInterfaceType

    /// Arduino pin number assignments for the 2 or 4 pins required to interface to the
    /// stepper motor or driver
    uint8_t        _stepPin;
    uint8_t        _directionPin;

    /// Whether the _pins is inverted or not
    uint8_t        _pinInverted[4];

    /// The current absolution position in steps.
    long           _currentPos;    // Steps

    /// The target position in steps. The AccelStepper library will move the
    /// motor from the _currentPos to the _targetPos, taking into account the
    /// max speed, acceleration and deceleration
    long           _targetPos;     // Steps

    /// The current motos speed in steps per second
    /// Positive is clockwise
    float          _speed;         // Steps per second

    /// The maximum permitted speed in steps per second. Must be > 0.
    float          _maxSpeed;

    /// The acceleration to use to accelerate or decelerate the motor in steps
    /// per second per second. Must be > 0
    float          _acceleration;
    float          _sqrt_twoa; // Precomputed sqrt(2*_acceleration)

    /// The current interval between steps in microseconds.
    /// 0 means the motor is currently stopped with _speed == 0
    unsigned long  _stepInterval;

    /// The last step time in microseconds
    unsigned long  _lastStepTime;

    /// The minimum allowed pulse width in microseconds
    unsigned int   _minPulseWidth;

    /// Is the direction pin inverted?
    ///bool           _dirInverted; /// Moved to _pinInverted[1]

    /// Is the step pin inverted?
    ///bool           _stepInverted; /// Moved to _pinInverted[0]

    /// Is the enable pin inverted?
    bool           _enableInverted;

    /// Enable pin for stepper driver, or 0xFF if unused.
    uint8_t        _enablePin;

    /// The pointer to a forward-step procedure
    void (*_forward)();

    /// The pointer to a backward-step procedure
    void (*_backward)();

    /// The step counter for speed calculations
    long _n;

    /// Initial step size in microseconds
    float _c0;

    /// Last step size in microseconds
    float _cn;

    /// Min step size in microseconds based on maxSpeed
    float _cmin; // at max speed

    /// Current direction motor is spinning in
    boolean _direction; // 1 == CW

};


/// Based on AccelStepper
/// \par Trademarks
///
/// AccelStepper is a trademark of AirSpayce Pty Ltd. The AccelStepper mark was first used on April 26 2010 for
/// international trade, and is used only in relation to motor control hardware and software.
/// It is not to be confused with any other similar marks covering other goods and services.
///
/// \par Copyright
///
/// This software is Copyright (C) 2010 Mike McCauley. Use is subject to license
/// conditions. The main licensing options available are GPL V2 or Commercial:


#endif 
