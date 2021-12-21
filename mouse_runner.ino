#include "mouse_runner.h"

/**
 * \brief Creates a mouse runner
 *
 * \param *stageParameters
 *      The array of stages we'll run
 * \param stageTotal
 *      The number of stages in the array
 * \param &motor
 *      The motor to use
 *
 * \return none
 */
MouseRunner::MouseRunner(const struct StageParameters *stageParameters, Motor &motor, WarningTone &warningTone):
    stageParameters(stageParameters),
    motor(motor),
    warningTone(warningTone)
{
    
}

/**
 * \brief Starts the mouse runner
 *
 * \param none
 *
 * \return none
 */
void MouseRunner::Start(void)
{
    // If we aren't waiting to start, ignore this
    if (this->state != State::Waiting)
    {
        return;
    }
    
    // Begin movement
    this->StartNextStage();
}

/**
 * \brief Stops the mouse runner
 *
 * \param none
 *
 * \return none
 */
void MouseRunner::Stop(void)
{
    // Stop our motor immediately
    this->motor.Stop(this->stageParameters[this->currentStage - 1].probe);
    
    // Note we're done
    this->state = State::Done;
}

/**
 * \brief s running the mouse
 *
 * \param none
 *
 * \return none
 */
void MouseRunner::StartNextStage(void)
{
    // If using varying accelerations, set acceleration for this stage. 
    
    if (useAccels)
    {
      this->motor.setAccel(this->stageParameters[this->currentStage].accel);
    }
    
    // We're in a new stage, so switch the tone warning played flag back to false.
    this->warningTone.playTonesStarted = false;
                                 
    // If the target speed is 0 (motor is supposed to go to rest)
    if (this->stageParameters[this->currentStage].speed == 0) {
    
      // Stop motor
      this->motor.Stop(this->stageParameters[this->currentStage - 1].probe);            
           
    }

    // If the target speed is anything besides 0 (not rest)
    else {
        // Start the motor stage  
        // Give the motor the PREVIOUS stage's probe.
        this->motor.Start(this->stageParameters[this->currentStage].speed, this->stageParameters[this->currentStage - 1].probe);
    }                           

    // Note when we started running
    this->timeStageStarted = millis();

    // Note we're running
    this->state = State::Running;
}

// Starts a new trial. Runs all the start-up needs for a trial. Is called from "Done" state in MouseRunner::RunOnce, only if useTrialNumber == true.
void MouseRunner::StartNewTrial(void)
{   
    // Increase trial number.
    trial_number += 1;

    // Reset currentStage to 0; 
    this->currentStage = 0;
    
    // Randomize time. Edits stageParameters.
    struct time_outputs randomTime = randomizeTime();

    this->stageTotal = randomTime.count; 
    
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

    // Switch to state waiting, if not already at waiting, which will start the next trial.
    this->state = State::Waiting;
}
/**
 * \brief Handles the mouse runner
 *
 * \param none
 *
 * \return none
 */
 
void MouseRunner::RunOnce(void)
{
    switch (this->state)
    {
        case State::Waiting:
        {  
           // Re-set the Done state "ever finished" piece.
           this->everFinished = false; 
           
           // If useing a trigger (useTrigger == true), check for a trigger input
           if (useTrigger) 
           {
              if (digitalRead(A0) == HIGH)
              {
                Serial.println("Starting Mouse Runner");
                globalStartTime = millis();
                this->Start();
              }
            }

            // If not using a trigger, start right away
            else
            { 
              Serial.println("Starting Mouse Runner");
              
              globalStartTime = millis();
              this->Start();
            }

            break;
        }

        case State::Running:
        {
            // Continue in this state until stage duration is reached.
            if (MouseRunner::TimeElapsed(this->timeStageStarted) < this->stageParameters[this->currentStage].duration)
            {
    
              // See if a tone should be played --> time has entered into the "warning" time window & the flag "toneStarted" has not been switched to "true"; 
              // And the next stage is not greater than the total stages (which happens if we hit the end of the stages)
              if (MouseRunner::TimeElapsed(this->timeStageStarted) > stageParameters[this->currentStage].duration - WarnTime && this->currentStage + 1 <= this->stageTotal)
              {
                 // If this is the first time the tones are being called (if playTonesStared is false)
                 if (this->warningTone.playTonesStarted == false){
                                 
                  // Figure out warning paramters; input is void because stageParameters was constructed in MouseRunner?
                  WarningTone::ToneParameters toneParameters = warningTone.CalculateToneParameters(this->currentStage);

                  this->warningTone.toneParameters=toneParameters;
                 }
                  
                  // Using the determined tone parameters, play the warning tones. 
                  this->warningTone.PlayWarningTone(this->warningTone.toneParameters);
              }
             
              // Continue without changing anything else.
              break;
            }

            // Note we're starting another stage
            this->currentStage++;

            // If we're out of stages, or if the duration of the stage is 0, that means we've reached the end of the list of stages and need to stop  
            if (this->currentStage > this->stageTotal || this->stageParameters[this->currentStage].duration == 0)
            {
                this->Stop();
                break;
            }
            
            // Start our next stage
            this->StartNextStage();
            
            break;
        }
        
        case State::Done:
        {   

           // If This is the first time the code has entered the "Done" stage,     
            if (this->everFinished == false){
              
              // Tell user that trial is done. 
              Serial.print(String(CurrentTime)); 
              Serial.print(", "); 
              Serial.println("Done.");
              Serial.println("");

              // If we're using trial number updates, go to a new function that re-starts everything without going through the setup loop again.
              if (useTrialNumber) 
              {
                this->StartNewTrial(); 
              }
              
            }
            // Change finished flag to true
            this->everFinished = true; 

            // Nothing to do anymore
            break;
        }
      
    }
}
