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
        
        enum State state = State::Idle;

        AccelStepper stepperMotor;

        // Which pin controls our sleeping
        static constexpr const size_t SleepPowerPin = 49;

        // Which pin controls our step size
        //static constexpr const size_t M1pin = 5;

        // Acceleration and deceleration of stepper motor
        static constexpr const float StepperAccell = 800;

        // The arbitrary number of steps you want the motor to aim for 
        static constexpr const long nSteps = LONG_MAX;

        // The next target speed
        float targetSpeed = 0;

        
    public:
        Motor(void);

        // Give the function the previous stage's probe status.
        void Start(float speed, Probe probe);
        void Stop(Probe probe);
        
        void RunOnce(void);
        void RoundedStop(void);
        long RoundUp(long numToRound, int multiple);
};
