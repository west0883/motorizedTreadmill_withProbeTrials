// motoriezedTreadmill_withProbeTrials.ino
// Sarah West
// 3/9/21
// Be sure you have the AccelStepper library installed & have included the Arduino Due as a possible board.

#include <stdint.h>
#include <AccelStepper.h>

#include "randomizing_functions.h"
#include "probe.h"
#include "motor.h"
#include "tone.h" 
#include "warningTone.h"
#include "mouse_runner.h"


// Paramters that can be edited. 

// Flag for if the trial number count (used with Putty) should be used. 
bool useTrialNumber = true;

// Initial trial number to be displayed with Putty. Is updated automatically at the end of each complete trial.
int trial_number = 1;

// write out what speeds you want to include (steps/s); 
static constexpr int allSpeeds[] = {0, 2000, 2400, 2800};

// write out what accelerations you want to include for starts and stops(steps/s/s):
static constexpr int accelsStartStop[] = {400, 600, 800}; 

// What accelerations you want to include for speed changes (steps/s/s):
static constexpr int accelsSpeedChange[] = {100, 200, 400, 800}; 

// The amount of time spend at rest at start of recording (ms). Includes 30 seconds that is cut off from scope ramp-up.
uint32_t StartRestTime = 15000;

// Minimum amound of time the mouse must be at rest at end of recording (ms).
uint32_t MinEndRestTime = 15000;

// The total amount of time in the recording.(ms)
uint32_t TotalTime = 330000;

// The amount of time before a stage switch the mouse is given the warning sound (ms).
uint32_t WarnTime = 5000;

// The amount of minimum time you want per stage (includeing the warning time and posible transition times). (IN SECONDS)
int MinStageTime = 15;

// The maximum amount of time you want per stage before a warning sound for next stage is given.(IN SECONDS)
int MaxStageTime = 35;

// Make a flag for if an input trigger (from Spike2) should be used.
static const bool useTrigger = false; 

// Make a flag for if maintaining tones should be used; 
static const bool useMaintaining = true; 

// Make a flag for if probe trials should be used.
static const bool useProbeTrials = false;

// Make a flag for if random accelerations should be used.
static const bool useAccels = true; 

// Probability of those probe trials, if used (a fraction of 1, 1 = 100% of the time); 
static const double probability = 0.10; 

// Initialize the time stage parameters array with a lot of possible entries
struct MouseRunner::StageParameters stageParameters[25];

// DON'T EDIT:

// Make a tag that motor and warningTone will use to make understanding what's happening in the serial monitor easier to understand later.
// Explained further in comments at bottom.
int activityTag;

// Function that finds the size of an array.
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
  pinMode(A0, INPUT);  // for the trigger
  Serial.begin(115200);
  
  // Randomize time. Edits stageParameters.
  struct time_outputs randomTime = randomizeTime();

  // Randomize speed. Edits stageParameters.
  randomizeSpeed(randomTime);

  // Randomize accelerations. Edits stageParameters. 
  randomizeAccel(randomTime); 
  
  // Randomizes probe trials. Edits stageParameters. 
  probeTrials(useProbeTrials, randomTime.count, probability);

  // Report stages to be run
  HeaderReport(randomTime.count);

  // If using a trigger, print out that it's waiting.
  if (useTrigger)
  {
    Serial.println("Waiting for Trigger");
  }
}

void loop(void)
{
  mouseRunner.RunOnce();
  motor.RunOnce();
}

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
 *  13 = Warning cue: probe, no warning.
 *  14 = Motor: probe, no warning tone, accelerating.
 *  15 = Motor: probe, no warning tone, decelerating.
 *  16 = Motor: probe, no warning tone, stopping.
 *  17 = Motor: probe, no warning tone, maintaining.
 *  18 = Warning cue: probe, starting cue, no change in motor.
 *  19 = Warning cue: probe, stopping cue, no change in motor.
 *  20 = Warning cue: probe, accerlerating cue, no change in motor.
 *  21 = Warning cue: probe, decelerating cue, no change in motor.
 *  22 = Warning cue: probe, maintaining cue, no change in motor.
 *  23 = motor probe: no change.
 */ 
