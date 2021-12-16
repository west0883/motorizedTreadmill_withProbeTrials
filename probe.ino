
#include "probe.h"

// Assign probe trials. Input total number of trials and the probability of any kind of probe trial happening 
void probeTrials(bool useProbeTrials, int count, double probability){
  
  // Don't use the 0 index or the last entry of stageParameters, because those will be the first and last rests. 
  // Also don't use the second to last, because a no change in motor requires changing the next stage.
  for (int i = 1; i < count - 1; i++){

     // Default the probe in stageParameters to be "None" 
     stageParameters[i].probe = Probe::None;

     // If the user wants to use probe trials, go about changing them to probe
     if (useProbeTrials) {
     
     // Get a random number from 1 to 100
     double prob1 = random(1, 101); // /100;
     prob1 = prob1/100;
     
     // If that number is below the entered probability, this will be a probe trial. 

     if (prob1 < probability){

         // Now, get a random number either 0 or 1
         int prob2 = random(0, 2);

         switch (prob2){
            
            case 0: 
 
              stageParameters[i].probe = Probe::NoWarning;

              break;
            
            case 1:

              stageParameters[i].probe = Probe::NoChange;
       
              // Edit next stage parameter so the motor doesn't change AND the future warning tone calculations stay correct. 
              // THIS DOESN"T WORK RIGHT NOW BECAUSE YOU'LL CHANGE THE WARNING TONE FOR THIS STAGE TO ALWAYS BE MAINTAINING
              stageParameters[i + 1].speed = stageParameters[i].speed;

              // Set the new speed difference for the next stage so the next warning tone stays correct.
              stageParameters[i + 1].speed_difference = stageParameters[i + 2].speed - stageParameters[i + 1].speed; 
              
              break;
         }
      } 
    }
  }
}

// Both of these will be reported at the motor's time.
struct Probe_Messages getProbeMessages(Probe probeName, ProbeSubtype1 probe_subtype1, ProbeSubtype2 probe_subtype2) {

  struct Probe_Messages probe_messages;
  
  switch (probeName){
    
    case Probe::NoWarning: 
    {    
        switch (probe_subtype1){
          
          case ProbeSubtype1::Warning:
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
          
          case ProbeSubtype1::Motor:
          {
              switch (probe_subtype2){

                // There's no subtype for Starting with the motor. 
              
                case ProbeSubtype2::Accelerating:
                {
                    probe_messages.activity_tag = 14;
                    probe_messages.probe_string = "Motor: probe, no warning tone, accelerating."; 
                    return probe_messages; 
                  break;
                }

                case ProbeSubtype2::Decelerating:
                {
                    probe_messages.activity_tag = 15;
                    probe_messages.probe_string = "Motor: probe, no warning tone, decelerating."; 
                    return probe_messages; 
                  break;
                }

                case ProbeSubtype2::Stopping:
                {
                    probe_messages.activity_tag = 16;
                    probe_messages.probe_string = "Motor: probe, no warning tone, stopping."; 
                    return probe_messages; 
                  break;
                }

                 case ProbeSubtype2::Maintaining:
                {
                    probe_messages.activity_tag = 17;
                    probe_messages.probe_string = "Motor: probe, no warning tone, maintaining."; 
                    return probe_messages; 
                  break;
                }
             }
           } 
        }       
    }

    case Probe::NoChange: 
    {
       switch (probe_subtype1){
          
        case ProbeSubtype1::Warning:
        {
          switch (probe_subtype2){

            case ProbeSubtype2::Starting:
            {
                 probe_messages.activity_tag = 18;
                 probe_messages.probe_string = "Warning cue: probe, starting cue, no change in motor."; 
                 return probe_messages; 
              break; 
            }

             case ProbeSubtype2::Stopping:
             {
                 probe_messages.activity_tag = 19;
                 probe_messages.probe_string = "Warning cue: probe, stopping cue, no change in motor."; 
                 return probe_messages; 
              break; 
             }

             case ProbeSubtype2::Accelerating:
             {
                 probe_messages.activity_tag = 20;
                 probe_messages.probe_string = "Warning cue: probe, accerlerating cue, no change in motor."; 
                 return probe_messages; 
              break; 
             }

             case ProbeSubtype2::Decelerating:
             {
                 probe_messages.activity_tag = 21;
                 probe_messages.probe_string = "Warning cue: probe, decelerating cue, no change in motor."; 
                 return probe_messages; 
              break; 
             }

             case ProbeSubtype2::Maintaining:
             {
                 probe_messages.activity_tag = 22;
                 probe_messages.probe_string = "Warning cue: probe, maintaining cue, no change in motor."; 
                 return probe_messages; 
              break; 
             }
          }
        }
          
         case ProbeSubtype1::Motor:
         {    
             // Is the same for all warning cue types. 
             probe_messages.activity_tag = 23;
             probe_messages.probe_string = "Motor: probe, no change in motor."; 
             return probe_messages; 
           break; 
         }
      }
    }
  }
}

void checkProbeMotor (int &activityTag, String &message, Probe probe, ProbeSubtype2 probe_subtype2)
{
  if (probe == Probe::None)
  {
    // If no probe, do nothing
  }
  
  else 
  {
    Probe_Messages probe_messages = getProbeMessages(probe, ProbeSubtype1::Motor, probe_subtype2);   
    activityTag = probe_messages.activity_tag;
    message = probe_messages.probe_string;    
  }
}
