#include "randomizing_functions.h"

// Helper functions run at setup. 

//Writes a function that randomizes the time spent in each stage. 
struct time_outputs randomizeTime(void)
{
  // Create a random seed for random sequence generator.
  randomSeed(analogRead(A5));

  // Initialize the variable you'll use to keep track of how much of the working time has been used.
  uint32_t cumulativeTime = 0;

  //Initialize counter for putting into stageParameters; Start at 1 (instead of 0) because first position will be the initial rest
  int count = 1;

  while (cumulativeTime < timeWorking )
  {
    // Find a random time
    uint32_t stagetime = random(MinStageTime, MaxStageTime) * (1000);

    if (count >= ARRAY_SIZE(stageParameters))
    {
      // We usea pre-allocated static array of stage parameters to avoid dynamic
      // memory allocation. If this is printed, then the list of generated random times
      // was mostly short times, and there weren't enough places in the array
      // to contain all of the stages. If you use short minimum stage times and need more stages,
      // increase the number of elements in stageParameters above.
      Serial.println("Overran pre-allocated size of possible motor stages, stopping early");
      break;
    }

    // Put the stagetime into the time part of the parameter list.
    stageParameters[count].duration = stagetime;

    // Calculate new cumulative time.
    cumulativeTime = cumulativeTime + stagetime;

    // Increase counter.
    count = count + 1;
  }

  struct time_outputs result = {
    .cumulativeTime = cumulativeTime,
    .count = count
  };

  return result;
}

// Creates a function that randomizes the speed order
static void randomizeSpeed(time_outputs randomTime, uint32_t TotalTime, uint32_t StartRestTime)
{
  // Creates a random seed for random sequence generator.
  randomSeed(analogRead(A5));

  // use the counter from above. Don't include "count", because that is the final rest period.  
  // Don't include index "0" because that is the initial rest period. 
  for (size_t i = 1; i < randomTime.count ; i++)
  {
    // Pick a random index within the speed array
    size_t j = random(0, ARRAY_SIZE(allSpeeds));

    // Place in parameter array
    stageParameters[i].speed = allSpeeds[j];
  }
  
  // Calculate the end resting time (will be the minimum rest time plus anything from the division of stages that was left over);
  uint32_t EndRestTime = TotalTime - StartRestTime - randomTime.cumulativeTime;
 
  // Set up start rest parameters
  stageParameters[0].duration = StartRestTime;
  stageParameters[0].speed = 0;
  
  // Set end rest parameters
  stageParameters[randomTime.count].duration = EndRestTime;
  stageParameters[randomTime.count].speed = 0;

}

// Assign probe trials. Input total number of trials and the probability of any kind of probe trial happening 
void probeTrials(int count, double probability){
  // Don't use the 0 index or the last entry of stageParameters, because those will be the first and last rests.
  for (int i = 1; i < count; i++){
     
     
     // Get a random number from 1 to 100
     randomSeed(analogRead(A5));
     double prob = random(1, 101)/100;

     // If that number is below the entered probability, this will be a probe trial. 

     if (prob < probability){

        stageParameters[i].probe = MouseRunner::Probe::NoWarning;
        stageParameters[i].probe = MouseRunner::Probe::NoChange;
     }
     else {
       
     }
     
     
  }
}
