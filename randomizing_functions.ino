#include "randomizing_functions.h"
#include <limits>

// Helper functions run at setup. 

//Writes a function that randomizes the time spent in each stage. 
struct time_outputs randomizeTime(void) 
  {

  // Initialize the variable you'll use to keep track of how much of the working time has been used.
  uint32_t cumulativeTime = 0;

  // Calculate the "working" time you can use for possible stages; Subtract the max stage time becuase the last stage added can go over the timeWorking limit. 
  uint32_t timeWorking = TotalTime - StartRestTime - MinEndRestTime - MaxStageTime * 1000;

  //Initialize counter for putting into stageParameters; Start at 1 (instead of 0) because first position will be the initial rest
  int count = 1;

  while (cumulativeTime < timeWorking )
  {
    // Find a random time
    uint32_t stagetime = random(MinStageTime, MaxStageTime + 1) * (1000);

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

   // Calculate the end resting time (will be the minimum rest time plus anything from the division of stages that was left over);
  uint32_t EndRestTime = TotalTime - StartRestTime - cumulativeTime;
 
  // Set up end rest parameters
  stageParameters[0].duration = StartRestTime;

  // Set up end rest duration
  stageParameters[count].duration = EndRestTime;
  
  struct time_outputs result = {
    .cumulativeTime = cumulativeTime,
    .count = count
  };

  return result;
}

// Creates a function that randomizes the speed order
static void randomizeSpeed(time_outputs randomTime)
{  
    // Use the counter from above. Don't include "count", because that is the final rest period.  
    // Don't include index "0" because that is the initial rest period. 
    for (size_t i = 1; i < randomTime.count ; i++)
    {
        // Pick a random index within the speed array
        size_t j = random(0, ARRAY_SIZE(allSpeeds));
    
        // Place in parameter array
        stageParameters[i].speed = allSpeeds[j];
    }
  
    // Set start rest speed.
    stageParameters[0].speed = 0;
    
    // Set end rest speed.
    stageParameters[randomTime.count].speed = 0;

    // Calculate the speed difference (for warning tone and easier use of probe trials).
     for (size_t i = 0; i < randomTime.count ; i++)
    {
        // Place in parameter array
        stageParameters[i].speed_difference = stageParameters[i + 1].speed - stageParameters[i].speed;
    }
}

// Creates a function that randomizes the speed order. Make dependent on speedDiff value to determine what kind of change it is.
static void randomizeAccel(time_outputs randomTime)
{
    // Only do this if user has said they want to randomize the accelerations.
    if (useAccels)
    {
        // Find the minimum speed 
        int min_speed = getMinSpeed(); 
        
        // Don't include index "0" because that is the initial rest period. 
        for (size_t i = 1; i <= randomTime.count ; i++)
        {
           // Use previous stage's speed difference (accel set at start of new stage, when motor transition begins)
    
           // If less than the minimum speed, then assume it's a speed change
           if (abs(stageParameters[i - 1].speed_difference) < min_speed)
           {
            // Pick a random index within the speed array
            size_t j = random(0, ARRAY_SIZE(accelsSpeedChange));
        
            // Place in parameter array
            stageParameters[i].accel = accelsSpeedChange[j];
           }
    
           // Otherwise, is assumed to be a start or stop.
           else
           {
             // Pick a random index within the speed array
            size_t j = random(0, ARRAY_SIZE(accelsStartStop));
        
            // Place in parameter array
            stageParameters[i].accel = accelsStartStop[j];
           }
        }
    }
}

// A function that writes out what goes to the serial monitor.
void Report(float targetSpeed, float accel, int activityTag, String message)
{
      
    CurrentTime=millis()-globalStartTime; 

    if (useTrialNumber)
    {
      Serial.print(trial_number);
      Serial.print(", ");
    }
    
    Serial.print(String(CurrentTime)); 
    Serial.print(", "); 
    Serial.print(String(targetSpeed));
    Serial.print(", "); 
    
    if (useAccels){
      Serial.print(String(accel));
      Serial.print(", "); 
    }
    
    Serial.print(activityTag); 
    Serial.print(", "); 
    Serial.println(message); 
}

void HeaderReport(int count)
{
  // Report trial number
  if (useTrialNumber) 
  {
    Serial.print("Trial ");
    Serial.println(trial_number); 
  }
  
  // Report total number of stages
  Serial.print("Total number of stages: ");
  Serial.println(count);

  // report speeds and times HeaderReport(int randomTime.count); 
  Serial.print("Time, Speed");

  if (useAccels)
  {
     Serial.print(", Acceleration");
  }
  
  if (useProbeTrials){
    Serial.print(", Probe type"); 
  }

  Serial.println();
 
  for (size_t i = 0; i <= count ; i++)
  {  
    Serial.print(stageParameters[i].duration);
    Serial.print(", ");
    
    Serial.print(stageParameters[i].speed);
    

    if (useAccels)
    {
      Serial.print(", ");
      Serial.print(stageParameters[i].accel);
    }

    if (useProbeTrials)
    {

      if (stageParameters[i].probe == Probe::NoWarning)
      {
        Serial.print(", "); 
        Serial.print("Probe: no warning cue");
      }
      
      else if (stageParameters[i].probe == Probe::NoChange)
      {
        Serial.print(", "); 
        Serial.print("Probe: no change in speed");
      }
      
      else
      {
        Serial.print(", "); 
        Serial.print("No probe");
      }
    }
    Serial.println();
  }  
}

int getMinSpeed(void)
{
    // A trick to find the maximum possible value for an int, so everything afterwards is always less than that
    int min_speed = std::numeric_limits<int>::max();
    
    for (std::size_t i = 0; i < ARRAY_SIZE(allSpeeds); i++)
    {
        if ((allSpeeds[i] > 0) && (allSpeeds[i] < min_speed))
        {
            min_speed = allSpeeds[i];
        }
    }
    
    // If our array was broken, make sure we return a reasonable default
    if (min_speed == std::numeric_limits<int>::max())
    {
        min_speed = 2000;
    }
    
    return min_speed;
}
