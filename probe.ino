
#include "probe.h"

// Assign probe trials. Input total number of trials and the probability of any kind of probe trial happening 
void probeTrials(bool useProbeTrials, int count, double probability){
  
  // Don't use the 0 index or the last entry of stageParameters, because those will be the first and last rests.
  for (int i = 1; i < count; i++){

     // Default the probe in stageParameters to be "None" 
     stageParameters[i].probe = Probe::None;

     // If the user wants to use probe trials, go about changing them to probe
     if (useProbeTrials) {
     
     // Get a random number from 1 to 100
     randomSeed(analogRead(A5));
     double prob1 = random(1, 101)/100;

     // If that number is below the entered probability, this will be a probe trial. 

     if (prob1 < probability){

         // Now, get a random number either 0 or 1
         randomSeed(analogRead(A5));
         int prob2 = random(0, 2);

         switch (prob2){
            
            case 0: 
 
              stageParameters[i].probe = Probe::NoWarning;

              break;
            
            case 1:

              stageParameters[i].probe = Probe::NoChange;

              // Edit next stage parameter so the motor doesn't change AND the future warning tone calculations stay correct. 
              stageParameters[i + 1].speed = stageParameters[i].speed;
              
              break;
           
         }
     } 
    }
  }
}

// Both of these will be reported at the motor's time.
struct Probe_Messages getProbeMessages(Probe probeName, ProbeSubtype probe_subtype) {

  struct Probe_Messages probe_messages;
  
  switch (probeName){
    
    case Probe::NoWarning: 
    {    
        switch (probe_subtype){
          
          case ProbeSubtype::Warning:
          {
              probe_messages.activity_tag = 13;
              probe_messages.probe_string = "Warning cue: probe, no warning tone."; 
              probe_messages.toneParameters = {
                  .frequency1 = 0,
                  .frequency2 = 0,
                  .frequency3 = 0
              };
              
              return probe_messages; 
            break; 
          }
          
          case ProbeSubtype::Motor:
          {
               probe_messages.activity_tag = 14;
               probe_messages.probe_string = "Motor: probe, no warning tone."; 
               return probe_messages; 
            break;
          } 
        }       
    }

    case Probe::NoChange: 
    {
       switch (probe_subtype){
          
          case ProbeSubtype::Warning:
          {
               probe_messages.activity_tag = 16;
               probe_messages.probe_string = "Warning cue: probe, no change in motor."; 
               return probe_messages; 
            break; 
          }

         case ProbeSubtype::Motor:
         {
             probe_messages.activity_tag = 16;
             probe_messages.probe_string = "Motor: probe, no change in motor."; 
             return probe_messages; 
           break; 
         }
      }
    }
  }
}

  
