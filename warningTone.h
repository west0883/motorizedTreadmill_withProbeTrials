// warningTone
// Sarah West
// 9/17/21

// Called from mouse_runner tab, within MouseRunner::RunOnce, case "Running".  

// Compiler instructions
#pragma once
#include "tone.h" 
// Make a class to hold states the function should care about
class WarningTone
{        
    private:

        // Initialize variable for keeping track of when warning time started (for calculating multi-beep warnings) 
        uint32_t toneStartTime;
        
        // Initialize variable for keeping track of when within-beeps we are 
        uint32_t toneCurrentTime;

        //Initialize variable that says what tone (of 3) you're in, initialize as an idle tone #0;
        int toneCase = 0; 

        // Initialize and set duration of each segment of the tone (in ms).
        uint32_t toneDuration = 310;

         // Initialize and set time between tones (in ms).
        uint32_t toneBuffer = 23;

    public:
        // Make a flag that says when warning period has started. Initiate as false.         
        bool playTonesStarted = false; 

         // Set up ToneParameter frequencies for each tone. Use a frequency of 0 if there should be a gap/pause. 
         struct ToneParameters {
            uint32_t frequency1;
            uint32_t frequency2;
            uint32_t frequency3;
        };

        struct ToneParameters toneParameters; 
        void PlayWarningTone(ToneParameters toneParameters); 
};
