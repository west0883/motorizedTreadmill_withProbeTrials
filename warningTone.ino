
#include "warningTone.h"

// need to switch the playTonesStarted back to false at some point 

void WarningTone::PlayWarningTone(WarningTone::ToneParameters new_toneParameters) 
{
    //// Find a starting time for saying when multiple-beep warnings should stop and start.
 
    this->toneParameters = new_toneParameters;    
    
    // If tones have never been called,
    if (this->playTonesStarted == false) {
       
        // Switch to playTonesStarted  = true so you don't re-calculate tone start time or tone parameters again
        this->playTonesStarted = true;     

        // Switch toneCase to first tone, start playing tones
        this->toneCase = 1;
    }

    this->toneCurrentTime=millis(); 

    switch (this->toneCase) {

       case 1: {
             // set "tone case" to run next tone
             toneCase = 2; 

            // Get the start time 
            this->toneStartTime = millis(); 
        
            // If frequncy isn't 0,
              if (this->toneParameters.frequency1 != 0){
                  // Play tone 1
                  startTone(tonePin, this->toneParameters.frequency1, this->toneDuration);
              }
              
               // if frequency IS 0, turn off tone
               else {
                  // Turn off tones
                   stopTone(); 
               }
          break;
        }           
      case 2: {
           // If the tone duration for the last tone is up,
           if (this->toneCurrentTime > this->toneStartTime + this->toneDuration + this->toneBuffer){

              // Switch logic to next tone so you never try to play tone 2 again 
              this->toneCase = 3; 

              // Use toneCurrentTime as new toneStartTime
              this->toneStartTime = this->toneCurrentTime;

              // If frequncy isn't 0,
              if (this->toneParameters.frequency2 != 0){
                  // Play tone 2
                  startTone(tonePin, this->toneParameters.frequency2, this->toneDuration);  
              }
              
               // if frequency IS 0, turn off tone
               else {
                   // Turn off tones
                 stopTone(); 
               }
           }
           break;    
        }
       case 3: {
           // If the tone duration for the last tone is up,
           if (this->toneCurrentTime > this->toneStartTime + this->toneDuration  + this->toneBuffer){
          
              // Switch to the tone-off case so you don't try to play tone 3 again.
              this->toneCase = 4;

              // Use toneCurrentTime as new toneStartTime
              this->toneStartTime = this->toneCurrentTime;

              // If frequncy isn't 0,
              if (this->toneParameters.frequency3 != 0){
                  // Play tone 3
                 startTone(tonePin, this->toneParameters.frequency3,this->toneDuration); 
               }
               
               // if frequency IS 0, turn off tone
               else {
               // Turn off tones
                stopTone(); 
               }
          } 
       break;
       }       
       case 4: {
           // If the tone duration for the last tone is up,
           if (this->toneCurrentTime > this->toneStartTime + this->toneDuration  + this->toneBuffer){
          
              // Switch to an idle tone case so you don't try to play tones again.
              this->toneCase = 0;

             // Turn off tones
              stopTone();
          } 
       break;
       }       
    }
}
