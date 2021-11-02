// motoriezedTreadmill_randomTone.ino
// Sarah West
// 3/9/21
// Be sure you have the AccelStepper library installed & have included the Arduino Due as a possible board.

#include <stdint.h>
#include <AccelStepper.h>

#include "motor.h"
#include "mouse_runner.h"
#include "tone.h" 
#include "warningTone.h"

//#define buzzerPin 31 //selected pin

// Initialize and set tonePin 
static uint32_t tonePin = 31;

// Initialize some time variables. 
uint32_t StartTime;
uint32_t CurrentTime;

// write out what speeds you want to include; ***YOU CAN EDIT THIS***
static constexpr int allSpeeds[] ={0, 0, 800, 1600};

// The amount of time spend at rest at start of recording (ms). Includes 30 seconds that is cut off from scope ramp-up.
uint32_t StartRestTime = 10000;

// Minimum amound of time the mouse must be at rest at end of recording (ms).
uint32_t MinEndRestTime = 10000;

// The total amount of time in the recording.(ms)
uint32_t TotalTime =930000;

// The amount of time before a stage switch the mouse is given the warning sound (ms).
uint32_t WarnTime = 5000;

// The amount of minimum time you want per stage (includeing the warning time and posible transition times). (IN SECONDS)
int MinStageTime = 15;

// The maximum amount of time you want per stage before a warning sound for next stage is given.(IN SECONDS)
int MaxStageTime = 25;

// Initialize the time stage parameters array with a lot of possible entries
struct MouseRunner::StageParameters stageParameters[70];

// Initialize variable that will hold warning tone parameters.
struct  WarningTone::ToneParameters toneParameters[1];

// Initialize the variable that will hold the end time.
uint32_t EndRestTime;

// Calculate the "working" time you can use for possible stages; Subtract the max stage time becuase the last stage added can go over the timeWorking limit. 
uint32_t timeWorking = TotalTime - StartRestTime - MinEndRestTime - MaxStageTime * 1000;

// Make a tag that motor and warningTone will use to make understanding what's happening in the serial monitor easier to understand later.
int activityTag = 0;
/** 
 *  1 = motor accelerating
 *  2 = motor decelerating
 *  3 = motor maintaining
 *  4 = motor stopping
 *  5 = motor finished stopping // Keeping this because it quickly finds the start of a rest period.
 *  6 = motor reached faster speed
 *  7 = motor reached slower speed
 *  8 = tone: starting
 *  9 = tone: stopping
 *  10 = tone: accelerating
 *  11 = tone: decellerating
 *  12 = tone: maintaining
 */ 

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

// Declare your objects.
static Motor motor;
static WarningTone warningTone;
static MouseRunner mouseRunner(stageParameters, ARRAY_SIZE(stageParameters), motor, warningTone);

// Initialize a structure that will hold the output of randomizeTim
struct time_outputs {
  uint32_t cumulativeTime;
  int count;
};


void setup(void)
{
  Serial.begin(115200);
  Serial.println(timeWorking);

  pinMode(tonePin, OUTPUT);

  struct time_outputs randomTime = randomizeTime();

  // Calculate the end resting time (will be the minimum rest time plus anything from the division of stages that was left over);
  EndRestTime = TotalTime - StartRestTime - randomTime.cumulativeTime;
 
  // Set up start rest parameters
  stageParameters[0].duration = StartRestTime;
  stageParameters[0].speed = 0;
  
  // Set end rest parameters
  stageParameters[randomTime.count].duration = EndRestTime;
  stageParameters[randomTime.count].speed = 0;

  // Randomize speed. 
  randomizeSpeed(randomTime.count);

  // Report total number of stages
  Serial.print("Total number of stages: ");
  Serial.println(randomTime.count);

  // report speeds and times
  Serial.println("Speed, Time"); 
  
  for (size_t i = 0; i <= randomTime.count ; i++)
  {
    Serial.print(stageParameters[i].speed);
    Serial.print(", ");     
    Serial.println(stageParameters[i].duration);
  }

  // Set pin modes for tone pins.
  
  pinMode(A0, INPUT);  // for the trigger
  Serial.println("Waiting for Trigger"); 
}

void loop(void)
{
  mouseRunner.RunOnce();
  motor.RunOnce();
}


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
static void randomizeSpeed(int count)
{
  // Creates a random seed for random sequence generator.
  randomSeed(analogRead(A5));

  // use the counter from above. Don't include "count", because that is the final rest period.  
  // Don't include index "0" because that is the initial rest period. 
  for (size_t i = 1; i < count ; i++)
  {
    // Pick a random index within the speed array
    size_t j = random(0, ARRAY_SIZE(allSpeeds));

    // Place in parameter array
    stageParameters[i].speed = allSpeeds[j];
  }
}
