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
MouseRunner::MouseRunner(const struct StageParameters *stageParameters, int stageTotal, Motor &motor, WarningTone &warningTone):
    stageParameters(stageParameters),
    stageTotal(stageTotal),
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
    this->motor.Stop();
    
    // Note we're done
    this->state = State::Done;
}

/**
 * \brief Starts running the mouse
 *
 * \param none
 *
 * \return none
 */
void MouseRunner::StartNextStage(void)
{
    // We're in a new stage, so switch the tone warning played flag back to false.
    this->warningTone.playTonesStarted = false;
                                 
    // If the target speed is 0 (motor is supposed to go to rest)
    if (this->stageParameters[this->currentStage].speed == 0) {
    
      // Stop motor
      this->motor.Stop();            
           
    }

    // If the target speed is anything besides 0 (not rest)
    else {
        // Start the motor stage  
        this->motor.Start(this->stageParameters[this->currentStage].speed);
    }

    // Note when we started running
    this->timeStageStarted = millis();

    // Note we're running
    this->state = State::Running;
}


// Write out code for calculating the tone parameters
WarningTone::ToneParameters MouseRunner::CalculateToneParameters(void)
{  
  // 
   int nextStage = currentStage +1 ; 
   
   // Find difference between next speed and current speed. 
   float speedDifference = this->stageParameters[nextStage].speed - this->stageParameters[this->currentStage].speed;

   // First check value of speedDifference, then check if current or next stage speed is 0
  
   // If speedDifference is positve
   if (speedDifference > 0) {

      // If current speed is 0, then give "starting" cue  
      if (stageParameters[this->currentStage].speed == 0) {
            // Reassign activity reporting tag.
            activityTag = 8;
           //Announce warning label
           CurrentTime=millis()-StartTime; 
           Serial.print(String(CurrentTime)); 
           Serial.print(", "); 
           Serial.print(stageParameters[this->currentStage].speed);
           Serial.print(", "); 
           Serial.print(activityTag); 
           Serial.print(", "); 
           Serial.println("Warning cue: starting"); 
           
           // High pitch, long 
            WarningTone::ToneParameters result = {
                .frequency1 = 10000,
                .frequency2 = 10000,
                .frequency3 = 10000
            };
            return result;    
      } 

       // If curernt speed is not 0, then give "accelerating" cue.
       else  {

           //Announce warning label
           // Reassign activity reporting tag.
           activityTag = 10;
           CurrentTime=millis()-StartTime; 
           Serial.print(String(CurrentTime)); 
           Serial.print(", "); 
           Serial.print(stageParameters[this->currentStage].speed);
           Serial.print(", "); 
           Serial.print(activityTag); 
           Serial.print(", "); 
           Serial.println("Warning cue: accelerating"); 
           
           // low, blank, high (ascending pitches)
            WarningTone::ToneParameters result = {
                .frequency1 = 4000,
                .frequency2 = 0,
                .frequency3 = 10000
            };
            return result;
        }
   }

   // Else if speedDifference is negative
   else if (speedDifference < 0 ){
      
      // If next speed is 0, then give "stopping" cue  
      if (stageParameters[nextStage].speed == 0) { 

           //Announce warning label
           // Reassign activity reporting tag.
           activityTag = 9;
           CurrentTime=millis()-StartTime; 
           Serial.print(String(CurrentTime)); 
           Serial.print(", "); 
           Serial.print(stageParameters[this->currentStage].speed);
           Serial.print(", "); 
           Serial.print(activityTag); 
           Serial.print(", "); 
           Serial.println("Warning cue: stopping"); 
            
            // Low pitch, 1 long
            WarningTone::ToneParameters result = {
                .frequency1 = 4000,
                .frequency2 = 4000,
                .frequency3 = 4000
            };
            return result;
      }
      // If next speed is not 0, then give "decelerating" cue.
      else {
           //Announce warning label
           // Reassign activity reporting tag.
           activityTag = 11;
           CurrentTime=millis()-StartTime; 
           Serial.print(String(CurrentTime)); 
           Serial.print(", "); 
           Serial.print(stageParameters[this->currentStage].speed);
           Serial.print(", "); 
           Serial.print(activityTag); 
           Serial.print(", "); 
           Serial.println("Warning cue: decelerating"); 
           
            // high, blank, low (descending pitches)
            WarningTone::ToneParameters result = {
                .frequency1 = 10000,
                .frequency2 = 0,
                .frequency3 = 4000
            };
            return result;
        }
   }
   
   // Else (if speedDifference is 0) 
   else {
       // Motor will continue at current speed

       //Announce warning label
       // Reassign activity reporting tag.
       activityTag = 12;
       CurrentTime=millis()-StartTime; 
       Serial.print(String(CurrentTime)); 
       Serial.print(", "); 
       Serial.print(stageParameters[this->currentStage].speed);
       Serial.print(", "); 
       Serial.print(activityTag); 
       Serial.print(", "); 
       Serial.println("Warning cue: maintaining"); 

       // For training, will be blank
        WarningTone::ToneParameters result = {
              .frequency1 = 0,
              .frequency2 = 0,
              .frequency3 = 0
        };
       
       // Mid, blank, mid
//        WarningTone::ToneParameters result = {
//              .frequency1 = 7000,
//              .frequency2 = 0,
//              .frequency3 = 7000
//        };
        return result;
   }
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
         //if (digitalRead(A0) == HIGH)
          //{
                Serial.println("Starting Mouse Runner");

                this->Start();
  //       }
            
            break;
        }

        case State::Running:
        {
            // Continue in this state until stage duration is reached.
            if (MouseRunner::TimeElapsed(this->timeStageStarted) < this->stageParameters[this->currentStage].duration)
            {
    
              // See if a tone should be played --> time has entered into the "warning" time window & the flag "toneStarted" has not been switched to "true"; 
              // And the next stage's duration is not 0 (which happens if we hit the end of the stages)
              if (MouseRunner::TimeElapsed(this->timeStageStarted) > stageParameters[this->currentStage].duration - WarnTime && stageParameters[this->currentStage + 1].duration != 0)
              {
                 // If this is the first time the tones are being called (if playTonesStared is false)
                 if (this->warningTone.playTonesStarted == false){
                                 
                  // Figure out warning paramters; input is void because stageParameters was constructed in MouseRunner?
                  WarningTone::ToneParameters toneParameters = CalculateToneParameters();

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
            if (this->currentStage >= this->stageTotal || this->stageParameters[this->currentStage].duration == 0)
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
              
              // Tell user that stack is done. 
              Serial.print(String(CurrentTime)); 
              Serial.print(", "); 
              Serial.println("Done.");
            }
            // Change finished flag to true
            this->everFinished = true; 
            
            // Nothing to do anymore
            break;
        }
    }
}
