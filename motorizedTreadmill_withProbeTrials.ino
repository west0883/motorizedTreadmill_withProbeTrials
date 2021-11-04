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
#include "randomizing_functions.h"

// Initialize some time variables. 
uint32_t StartTime;
uint32_t CurrentTime;

// write out what speeds you want to include; ***YOU CAN EDIT THIS***
static constexpr int allSpeeds[] ={0, 0, 0, 1600, 2400, 3200};

// The amount of time spend at rest at start of recording (ms). Includes 30 seconds that is cut off from scope ramp-up.
uint32_t StartRestTime = 10000;

// Minimum amound of time the mouse must be at rest at end of recording (ms).
uint32_t MinEndRestTime = 10000;

// The total amount of time in the recording.(ms)
uint32_t TotalTime =330000;

// The amount of time before a stage switch the mouse is given the warning sound (ms).
uint32_t WarnTime = 3000;

// The amount of minimum time you want per stage (includeing the warning time and posible transition times). (IN SECONDS)
int MinStageTime = 15;

// The maximum amount of time you want per stage before a warning sound for next stage is given.(IN SECONDS)
int MaxStageTime = 30;

// Initialize the time stage parameters array with a lot of possible entries
struct MouseRunner::StageParameters stageParameters[30];

// Initialize variable that will hold warning tone parameters.
struct WarningTone::ToneParameters toneParameters[1];

// Make a flag for if probe trials should be used.
bool useProbeTrials = false;

// Probability of those probe trials, if used (a fraction of 1, 1 = 100% of the time); 
double probability = 0.05; 

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


void setup(void)
{
  Serial.begin(115200);
  pinMode(tonePin, OUTPUT);

  struct time_outputs randomTime = randomizeTime();

  // Randomize speed. Edits stageParameters (with pointers) 
  randomizeSpeed(randomTime, TotalTime, StartRestTime);

  // Randomizes probe trials. Edits stageParameters. 
  probeTrials(useProbeTrials, randomTime.count, probability);

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
  
  pinMode(A0, INPUT);  // for the trigger
  Serial.println("Waiting for Trigger"); 
}

void loop(void)
{
  mouseRunner.RunOnce();
  motor.RunOnce();
}
