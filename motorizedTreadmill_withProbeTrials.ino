// motoriezedTreadmill_randomTone.ino
// Sarah West
// 3/9/21
// Be sure you have the AccelStepper library installed & have included the Arduino Due as a possible board.

// Should write a "report" function, so "activity tag" doesn't have to be global and you don't have to re-write the reporting lines over and over. Will be used in motor and warning tone.
#include <stdint.h>
#include <AccelStepper.h>

#include "randomizing_functions.h"
#include "probe.h"
#include "motor.h"
#include "tone.h" 
#include "warningTone.h"
#include "mouse_runner.h"


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

// Make a flag for if maintaining tones should be used; 
static const bool useMaintaining = false; 

// Make a flag for if probe trials should be used.
static const bool useProbeTrials = false;

// Probability of those probe trials, if used (a fraction of 1, 1 = 100% of the time); 
static const double probability = 0.05; 

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
 *  13 = tone probe: no warning.
 *  14 = motor probe: no warning.
 *  15 = tone probe: no change.
 *  16 = motor probe: no change.
 */ 
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

// Predefine your times (do it here, and with "static" so they're accessible throughout program).
static uint32_t globalStartTime; 
static uint32_t CurrentTime;

// Declare your objects.
static Motor motor;
static WarningTone warningTone(useMaintaining);
static MouseRunner mouseRunner(stageParameters, ARRAY_SIZE(stageParameters), motor, warningTone);

void setup(void)
{
  // Randomize time. Edits stageParameters.
  struct time_outputs randomTime = randomizeTime();

  // Randomize speed. Edits stageParameters.
  randomizeSpeed(randomTime, TotalTime, StartRestTime);

  // Randomizes probe trials. Edits stageParameters. 
  probeTrials(useProbeTrials, randomTime.count, probability);

  Serial.begin(115200);
  
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
}

void loop(void)
{
  mouseRunner.RunOnce();
  motor.RunOnce();
}
