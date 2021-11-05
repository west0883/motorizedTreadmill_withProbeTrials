
#include "probe.h"

// Assign probe trials. Input total number of trials and the probability of any kind of probe trial happening 
void probeTrials(bool useProbeTrials, int count, double probability){
  
  // Don't use the 0 index or the last entry of stageParameters, because those will be the first and last rests.
  for (int i = 1; i < count; i++){

     // Default the probe in stageParameters to be "None" 
     stageParameters[i].probe = MouseRunner::Probe::None;

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
 
              stageParameters[i].probe = MouseRunner::Probe::NoWarning;

              break;
            
            case 1:

              stageParameters[i].probe = MouseRunner::Probe::NoChange;
              
              break;
           
         }
     } 
    }
  }
}
