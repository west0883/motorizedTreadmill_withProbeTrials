
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
    // Find a starting time for saying when multiple-beep warnings should stop and start.
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

   WarningTone::ToneParameters result; 
   String message; 
   
   // Declare probe subtypes. Give probe_subtype2 a default value of Blank
   ProbeSubtype1 probe_subtype1 = ProbeSubtype1::Warning;
   ProbeSubtype2 probe_subtype2 = ProbeSubtype2::Blank;
   
   // If there are probe trials, and this is a probe trial, 
   if (useProbeTrials && stageParameters[currentStage].probe == Probe::NoWarning)
   {

      // The probe subtype2 should be blank.
      probe_subtype2 = ProbeSubtype2::Blank;
    
      // Call correct probes
      Probe_Messages probe_messages = getProbeMessages(stageParameters[currentStage].probe, probe_subtype1, probe_subtype2);

      // Report
      Report(stageParameters[currentStage].speed, stageParameters[currentStage].accel, probe_messages.activity_tag, probe_messages.probe_string);

      // Output the probe toneParameters
      return probe_messages.toneParameters;    
   }

   // If not that particular probe, calculate tone parameters. 
   else 
   {

       // First check value of stageParameters[currentStage].speed_difference, then check if current or next stage speed is 0
      
       // If stageParameters[currentStage].speed_difference is positve
       if (stageParameters[currentStage].speed_difference > 0) {
    
          // If current speed is 0, then give "starting" cue  
          if (stageParameters[currentStage].speed == 0) {
                
                // Reassign activity reporting tag.
                activityTag = 8;
              
                // Report
                message = "Warning cue: starting "; 
                probe_subtype2 = ProbeSubtype2::Starting;
               // High pitch, long 
                result = {
                    .frequency1 = 10000,
                    .frequency2 = 10000,
                    .frequency3 = 10000
                };
          } 
    
           // If curernt speed is not 0, then give "accelerating" cue.
           else  {
    
               //Announce warning label
               // Reassign activity reporting tag.
               activityTag = 10;
    
               // Report
               message = "Warning cue: accelerating ";
               probe_subtype2 = ProbeSubtype2::Accelerating;
               
               // low, blank, high (ascending pitches)
               result = {
                    .frequency1 = 4000,
                    .frequency2 = 0,
                    .frequency3 = 10000
                };
            }
       }
    
       // Else if stageParameters[currentStage].speed_difference is negative
       else if (stageParameters[currentStage].speed_difference < 0 ){
          
          // If next speed is 0, then give "stopping" cue  
          if (stageParameters[currentStage + 1].speed == 0) { 
    
               // Reassign activity reporting tag.
               activityTag = 9;
    
               // Report
               message = "Warning cue: stopping"; 
               probe_subtype2 = ProbeSubtype2::Stopping;
               
               // Low pitch, 1 long
               result = {
                    .frequency1 = 4000,
                    .frequency2 = 4000,
                    .frequency3 = 4000
                };
          }
          // If next speed is not 0, then give "decelerating" cue.
          else {

               // Reassign activity reporting tag.
               activityTag = 11;
               
               // Report
               message = "Warning cue: decelerating "; 
               probe_subtype2 = ProbeSubtype2::Decelerating;
               
               // high, blank, low (descending pitches)
               result = {
                    .frequency1 = 10000,
                    .frequency2 = 0,
                    .frequency3 = 4000
                };
            }
       }
       
       // Else (if stageParameters[currentStage].speed_difference is 0) 
       else {
           // Motor will continue at current speed
    
           // Reassign activity reporting tag.
           activityTag = 12;
    
           if (useMaintaining){
               
              // Report
              message = "Warning cue: maintaining"; 
              probe_subtype2 = ProbeSubtype2::Maintaining;
               
              // Mid, blank, mid
              result = {
                    .frequency1 = 7000,
                    .frequency2 = 0,
                    .frequency3 = 7000
              };
           }
           else {
    
              // Report
              message = "Warning cue: maintaining, no tone given "; 
              probe_subtype2 = ProbeSubtype2::Maintaining;
              
              result = {
                    .frequency1 = 0,
                    .frequency2 = 0,
                    .frequency3 = 0
              };
            }
         }

        // If this was the "no change" probe, overwrite the reporting messages  
        if (useProbeTrials && stageParameters[currentStage].probe == Probe::NoChange){
      
            // Call correct probes
            Probe_Messages probe_messages = getProbeMessages(stageParameters[currentStage].probe, probe_subtype1, probe_subtype2);

            // Reassign 
            activityTag = probe_messages.activity_tag;

            // Reassign 
            message = probe_messages.probe_string;
        }

        Report(stageParameters[currentStage].speed, stageParameters[currentStage].accel, activityTag, message);
        return result; 
   }
}
