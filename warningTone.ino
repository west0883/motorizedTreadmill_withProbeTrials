
#include "warningTone.h"

// need to switch the playTonesStarted back to false at some point 

WarningTone::WarningTone(bool useMaintaining):
    useMaintaining(useMaintaining)
{
    // Initialize output pin.
    pinMode(tonePin, OUTPUT);

    // Initialize variable that will hold warning tone parameters.
    struct WarningTone::ToneParameters toneParameters[1];

}

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

// Write out code for calculating the tone parameters
struct WarningTone::ToneParameters WarningTone::CalculateToneParameters(int currentStage)
{  
  // 
   int nextStage = currentStage +1 ; 

   // If there are probe trials, and this is a probe trial, 
   if (useProbeTrials && stageParameters[currentStage].probe == Probe::NoWarning){
    
      // Call correct probes
 
    
   }

   else if (useProbeTrials && stageParameters[currentStage].probe == Probe::NoChange){
     // Call correct probe reports, continue with normal warning tones
   }

   else {
   
   // Find difference between next speed and current speed. 
   float speedDifference = stageParameters[nextStage].speed - stageParameters[currentStage].speed;

   // First check value of speedDifference, then check if current or next stage speed is 0
  
   // If speedDifference is positve
   if (speedDifference > 0) {

      // If current speed is 0, then give "starting" cue  
      if (stageParameters[currentStage].speed == 0) {
            // Reassign activity reporting tag.
            activityTag = 8;
          
            // Report
            String message = "Warning cue: starting "; 
            Report(stageParameters[currentStage].speed, activityTag, message);
           
           // High pitch, long 
            WarningTone::ToneParameters result = {
                .frequency1 = 10000,
                .frequency2 = 10000,
                .frequency3 = 10000
            };
            return result;    
      } 

       // If curernt speed is not 0, then give "accelerating" cue.
       else  {

           //Announce warning label
           // Reassign activity reporting tag.
           activityTag = 10;

           // Report
           String message = "Warning cue: accelerating "; 
           Report(stageParameters[currentStage].speed, activityTag, message);
           
           // low, blank, high (ascending pitches)
            WarningTone::ToneParameters result = {
                .frequency1 = 4000,
                .frequency2 = 0,
                .frequency3 = 10000
            };
            return result;
        }
   }

   // Else if speedDifference is negative
   else if (speedDifference < 0 ){
      
      // If next speed is 0, then give "stopping" cue  
      if (stageParameters[nextStage].speed == 0) { 

           //Announce warning label
           // Reassign activity reporting tag.
           activityTag = 9;

           // Report
           String message = "Warning cue: stopping"; 
           Report(stageParameters[currentStage].speed, activityTag, message);
           
           // Low pitch, 1 long
           WarningTone::ToneParameters result = {
                .frequency1 = 4000,
                .frequency2 = 4000,
                .frequency3 = 4000
            };
            return result;
      }
      // If next speed is not 0, then give "decelerating" cue.
      else {
           //Announce warning label
           // Reassign activity reporting tag.
           activityTag = 11;
           
           // Report
           String message = "Warning cue: decelerating "; 
           Report(stageParameters[currentStage].speed, activityTag, message);
           
            // high, blank, low (descending pitches)
            WarningTone::ToneParameters result = {
                .frequency1 = 10000,
                .frequency2 = 0,
                .frequency3 = 4000
            };
            return result;
        }
   }
   
   // Else (if speedDifference is 0) 
   else {
       // Motor will continue at current speed


       //Announce warning label
       // Reassign activity reporting tag.
       activityTag = 12;

       if (useMaintaining){
           
          // Report
          String message = "Warning cue: maintaining"; 
          Report(stageParameters[currentStage].speed, activityTag, message);
           
          // Mid, blank, mid
          WarningTone::ToneParameters result = {
                .frequency1 = 7000,
                .frequency2 = 0,
                .frequency3 = 7000
          };
            return result;
       }
       else {

          // Report
          String message = "Warning cue: maintaining, no tone given "; 
          Report(stageParameters[currentStage].speed, activityTag, message);
           
           WarningTone::ToneParameters result = {
                .frequency1 = 0,
                .frequency2 = 0,
                .frequency3 = 0
          };
            return result;
        }
   }
   }
}
