#include "motor.h"


/**
 * \brief Creates a motor
 *
 * \param none
 *
 * \return none
 */
Motor::Motor(void):
    stepperMotor(AccelStepper::DRIVER, 3, 2)
{
    pinMode(3, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(Motor::SleepPowerPin, OUTPUT);
    digitalWrite(Motor::SleepPowerPin, HIGH);
    awakeState = false;
    this->stepperMotor.setCurrentPosition(0);
 
    //Set a default acceleration
    this->stepperMotor.setAcceleration(Motor::StepperAccell);
}

/**
 * \brief Starts the motor
 *
 * \param speed
 *      How fast to run the motor
 *
 * \return none
 */
void Motor::Start(float speed, Probe probe)
{
    this->targetSpeed = speed;

    if (this->awakeState == false)
    {
      digitalWrite(Motor::SleepPowerPin, LOW);
      delay(3);
      this->awakeState = true; 
    }
    
    // If we'll be accelerating, just set the new max speed, as the motor driver
    // will manage accelerating
    if (this->targetSpeed >  this->stepperMotor.speed())
    {   
        // Reassign activity reporting tag.
        activityTag = 1;

        // Report
        String message = "Motor: accelerating"; 
        ProbeSubtype2 probe_subtype2 = ProbeSubtype2::Accelerating; 
        checkProbeMotor(activityTag, message, probe, probe_subtype2); 
        Report(this->targetSpeed, this->currentAccel, activityTag, message);
        
        this->stepperMotor.setMaxSpeed(this->targetSpeed);

        this->stepperMotor.move(Motor::nSteps);

        this->state = State::Accelerating;

    }
    // Else, if we'll be decelerating, we'll need to coast the motor down from its
    // current speed manually using stop()
    else if (this->targetSpeed <  this->stepperMotor.speed())
    {
        // Reassign activity reporting tag.
        activityTag = 2;

        // Report
        String message = "Motor: decelerating"; 
         ProbeSubtype2 probe_subtype2 = ProbeSubtype2::Decelerating; 
        checkProbeMotor(activityTag, message, probe, probe_subtype2); 
        Report(this->targetSpeed, this->currentAccel, activityTag, message);

        //this->stepperMotor.stop();
        this->RoundedStop();
        this->state = State::Decelerating;
    }
    
    // Else, if speed is staying the same
    else
    {
        // Reassign activity reporting tag.
        activityTag = 3;
       
        // Report
        String message = "Motor: maintaining current speed "; 
        ProbeSubtype2 probe_subtype2 = ProbeSubtype2::Maintaining; 
        checkProbeMotor(activityTag, message, probe, probe_subtype2); 
        Report(this->targetSpeed, this->currentAccel, activityTag, message);
        
        this->stepperMotor.setMaxSpeed(this->targetSpeed);
        this->stepperMotor.move(Motor::nSteps);
        this->state = State::Running;
    }
}

/**
 * \brief Stops the motor
 *
 * \param none
 *
 * \return none
 */
void Motor::Stop(Probe probe)
{
    this->targetSpeed = 0;
    // Reassign activity reporting tag.
    activityTag = 4;

    // Report
    String message = "Motor: stopping "; 
    ProbeSubtype2 probe_subtype2 = ProbeSubtype2::Stopping; 
    checkProbeMotor(activityTag, message, probe, probe_subtype2); 
    Report(this->targetSpeed, this->currentAccel, activityTag, message);
    
    // Stop our motor
    //this->stepperMotor.stop();
    RoundedStop();
  
    this->state = State::Stopping;
}

/**
 * \brief Handles controlling the motor
 *
 * \param none
 *
 * \return none
 */
void Motor::RunOnce(void)
{
    switch (this->state)
    {
        case State::Idle:
        { 
            // Make sure enable pin is high during waiting for trigger period.
            digitalWrite(Motor::SleepPowerPin, HIGH);
            
            // Nothing else to do
            return;
        }
        
        case State::Stopping:
        {
            // When motor has reached final position
            if (this->stepperMotor.distanceToGo() == 0)
            {     
                // Reassign activity reporting tag.
                activityTag = 5;
  
                // Report
                String message = "Motor: finished stopping "; 
                Report(this->targetSpeed, this->currentAccel, activityTag, message);
                
               // this->stepperMotor.setCurrentPosition(0);
                
                // Turn off the motor's power
                digitalWrite(Motor::SleepPowerPin, HIGH);
                awakeState = false; 
                
                // go to Idle state 
                this->state = State::Idle;
            }
            
            break;
        }

        case State::Accelerating:
        {
            // If the motor has reached the target speed
            if (this->stepperMotor.speed() >= this->targetSpeed)
            {
                // Reassign activity reporting tag.
                activityTag = 6;
             
                // Report
                String message = "Motor: reached faster speed"; 
                Report(this->targetSpeed, this->currentAccel, activityTag, message);
                           
                // the AccelStepper library doesn't need to reset the set speed after accelerating, like decelerating does

                this->state = State::Running;
            }

            break;
        }
        
        case State::Decelerating:
        {
            // If we've coasted enough and we're ready to start moving again, do so
            if (this->stepperMotor.speed() <= this->targetSpeed)
            {    
                // Reassign activity reporting tag.
                activityTag = 7;

                // Report
                String message = "Motor: reached slower speed "; 
                Report(this->targetSpeed, this->currentAccel, activityTag, message);
                
                this->stepperMotor.setMaxSpeed(this->targetSpeed);
                this->stepperMotor.move(Motor::nSteps);
                this->state = State::Running;
            }

            break;
        }

        case State::Running:
        {   
    
            break;
        }
    }
    
    // This holds in its 'memory' where the motor is supposed to go and checks if the
    // stepper is due for another step; runs for every iteration of the loop
    this->stepperMotor.run();
}

// Stops the motor on a whole step (always make divisible by 4) to avoid jerking on next start-up.
void Motor::RoundedStop(void)
{
    if (this->stepperMotor.speed() != 0.0)
    {    
      
      // Get the current position. 
      long currentPos = this->stepperMotor.currentPosition(); 
      long stepsToStop = (long)((this->stepperMotor.speed() *this->stepperMotor.speed()) / (2.0 *this->currentAccel)) + 1; // Equation 16 (+integer rounding)
      
      long roundedSteps = RoundUp(currentPos, stepsToStop, 4);
      
      if (this->stepperMotor.speed() > 0)
      {
          this->stepperMotor.move(roundedSteps);
      }
      else
      {
          this->stepperMotor.move(-roundedSteps);
      }
    }
}

long Motor::RoundUp(long currentPos, long stepsToStop, int multiple) 
{
  if (stepsToStop == 0) return 0;
  else{

    // Create a holder number that incorporates where the motor is now/if it's not on a whole step AND the distance to stop.
    long holder = currentPos + stepsToStop;  

    // Round holder number, subtract out the current position. Should return a whole-step number that takes both into account.  
    return ((holder + multiple - 1) / multiple) * multiple - currentPos;
  }
}

void Motor::setAccel(float input_accel)
{
   this->stepperMotor.setAcceleration(input_accel);
   this->currentAccel = input_accel;
}
