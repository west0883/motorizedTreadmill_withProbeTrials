#pragma once

#include <stddef.h>
#include <stdint.h>

#include "warningTone.h"
#include "motor.h"

class MouseRunner
{
    public:

        struct StageParameters
        {
            
            // How long we'll run the mouse for
            uint32_t duration;

            // How fast the motor will run this interval
            float speed;
        };

    private:
        enum class State
        {
            // We're waiting for the initial starting signal
            Waiting,

            // We're running the mouse
            Running,

            // We're done running the mouse
            Done,
        };
        
        enum State state = State::Waiting;

        // Our stages
        const struct StageParameters *stageParameters;
        int stageTotal;

        // When we started our last stage
        uint32_t timeStageStarted;

        // The stage we're currently handling
        int currentStage = 0;

        // Our motor
        Motor &motor;

        // Our warning tone object;
        WarningTone &warningTone; 

       // Create flag for when the code first reaches the "Done" stage. Initialize as false.
       bool everFinished = false; 

    private:
        /**
         * \brief Gets the time since a start time
         *
         * \param startTime
         *      The start time
         *
         * \return uint32_t
         *      The time since
         */
        static inline uint32_t TimeElapsed(uint32_t startTime)
        {
            return (uint32_t)millis() - startTime;
        }

        void StartNextStage(void);
        
    public:
        MouseRunner(const struct StageParameters *stageParameters, int stageTotal, Motor &motor, WarningTone &warningTone);

        WarningTone::ToneParameters CalculateToneParameters(void);
        void Start(void);
        void Stop(void);
        
        void RunOnce(void);
};
