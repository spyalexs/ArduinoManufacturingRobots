#include "Command.h"

Command::Command(RobotContainer* MC, Communicator* CC, String name, bool preconfirmed){
  //general command structure to run a drawn out task on a bot
  m_name = name;
  mp_MC = MC;
  mp_CC = CC;

  m_completed = false; 
  m_preconfirmed = preconfirmed;
}

Command::Command(){
  //initialize a placeholder command
  m_name = "Empty";
  mp_MC = nullptr;
  mp_CC = nullptr;

  //mark as completed so the system knows it can replace the command at any time
  m_completed = true; 
}

void Command::setStatus(int status){
  //allows the listener to set the commands status
  this->m_status = status;

  //send the controller the updated status
  updateStatus(status);
}

void Command::updateStatus(int status){
  //update the command's status and write to controller
  if(status == 253){
    this->m_confirmationRequestTime = this->mp_MC->getTime();
  }

  mp_CC->writeMessageToCentral("commandStatus", String(status));
}

void Command::abort(){
  // check if the command needs to be aborted - command to abort will be sent from controller 
  // TODO - exceptions added later for collisions detected by robot
  Serial.println("Abort signal recieved to end command: " + this->getName());

  this->setStatus(255);
}

void Command::confirmCommand(){
  //check if the controller has given the robot permission to run the command...
  //confirmation is value of 253 over the issue characteristic

  //only work if the command is in the 253 status
  if(this->m_status == 253){
    this->setStatus(2);
  }
}

// void Command::startup(){
//   // this should be overriden with tasks when starting command
//   // this may not be blocking and will only occur one time
//   return;
// }

// void Command::cycle(){
//   // this should be overriden to execute the commands cycle 
//   // this may not be blocking and will cycle as many times as needed
//   return;
// }

// bool Command::ifEnd(){
//   //this shoud be ovverriden to return True when the cycling should be stopped
//   return false;
// }

// void Command::cleanup(){
//   //this should be overriden with tasks when ending command
// }

void Command::run(){
  //call run to excute command
  Serial.println("Starting to run: " + m_name);

  //sequence on operations with status updates for controller
  this->setStatus(1);
}

void Command::superCycle(){
  //check which state the command is in and determine farther action based on that

  switch (this->m_status){
    case 0:
      //the command is not yet ready to be run
      break;
    case 254:
      //the command has been completed and is waiting to be replaced
      //this state is deciprated -- should never be called
      break;
    case 1:
      //do startup action
      this->startup();

      //request confirmation if not preconfirmed
      if(this->m_preconfirmed)
      {
        this->setStatus(2);
      }else{
        this->setStatus(253);
      }
    break;

    case 253:
      //do nothing as waiting for confirmation
      break;

    case 2:

      if(!this->ifEnd()){
        this->cycle();
      } else {
        this->setStatus(3);
      }

      break;

    case 3:
      this->cleanup();
      this->m_completed = true;
     break;

    case 255:
      this->cleanup();
      this->m_completed = true;

      //set status of command to complete so it does not clean up -- avoid sending status to central so the abort status still show on GUI
      break;

    default:
      Serial.println("Unknown status! Something must have gone terribly wrong!");
      break;
  }
}

bool Command::isCompleted(){
  return this->m_completed;
}

String Command::getName(){
  return this->m_name;
}

int Command::getStatus(){
  return this->m_status;
}
