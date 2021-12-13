#pragma once

#include <limits.h>
#include <stddef.h>

#include <AccelStepper.h>
#include "probe.h"

class Motor
{
    private:
        enum class State
        {
            Idle,
            Running,
            Decelerating,
            Stopping,
            Accelerating
        };

        AccelStepper stepperMotor;

        enum State state = State::Idle;

        // Which pin controls our sleeping
        static constexpr const size_t SleepPowerPin = 4;

        // Which pin controls our step size
        //static constexpr const size_t M1pin = 5;

        // The arbitrary number of steps you want the motor to aim for 
        static constexpr const long nSteps = LONG_MAX;

        // Declare the stepper acceleration, set to a default.
        static constexpr const float StepperAccell = 800;

        // The next target speed
        float targetSpeed = 0;
        
        // Keep track of if sleep pin is on or not.
        bool awakeState;
        
    public:
        Motor(void);

         // The current acceleration.
        float currentAccel; 
        
        // Give the function the previous stage's probe status.
        void Start(float speed, Probe probe);
        void Stop(Probe probe);

        // Make a function to change the acceleration (without making AccelStepper object public)
        void setAccel(float currentAccel); 
        
        void RunOnce(void);
        void RoundedStop(void);
        long RoundUp(long currentPos, long stepsToStop,  int multiple);
};
