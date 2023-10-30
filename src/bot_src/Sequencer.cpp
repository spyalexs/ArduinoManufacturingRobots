#include "Sequencer.h"

Sequencer::Sequencer(){
    this->mp_CC = nullptr;
}

void Sequencer::loadInCommand(TurnRight command){
    //load the command into it's respective queue
    this->m_turnRightQueue.push(command);

    //add the name of command to the sequence so the system knows what command to give out next
    this->m_sequence.push(command.getName());
}

void Sequencer::loadInCommand(TurnLeft command){
    //load the command into it's respective queue
    this->m_turnLeftQueue.push(command);

    //add the name of command to the sequence so the system knows what command to give out next
    this->m_sequence.push(command.getName());
}

void Sequencer::loadInCommand(TravelStraight command){
    //load the command into it's respective queue
    this->m_travelStraightQueue.push(command);

    //add the name of command to the sequence so the system knows what command to give out next
    this->m_sequence.push(command.getName());
}

void Sequencer::loadInCommand(TestCommand command){
    //load the command into it's respective queue
    this->m_testCommandQueue.push(command);

    //add the name of command to the sequence so the system knows what command to give out next
    this->m_sequence.push(command.getName());
}

void Sequencer::loadInCommand(FollowLineOnMarker command){
    //load the command into it's respective queue
    this->m_followLineOnMarkerQueue.push(command);

    //add the name of command to the sequence so the system knows what command to give out next
    this->m_sequence.push(command.getName());
}

void Sequencer::loadInCommand(FollowLineUntilAction command){
    //load the command into it's respective queue
    this->m_followLineUntilActionQueue.push(command);

    //add the name of command to the sequence so the system knows what command to give out next
    this->m_sequence.push(command.getName());
}

void Sequencer::loadInCommand(FollowLineUntilMarker command){
    //load the command into it's respective queue
    this->m_followLineUntilMarkerQueue.push(command);

    //add the name of command to the sequence so the system knows what command to give out next
    this->m_sequence.push(command.getName());
}

void Sequencer::loadInCommand(WaitCommand command){
    //load the command into it's respective queue
    this->m_waitCommandQueue.push(command);

    //add the name of command to the sequence so the system knows what command to give out next
    this->m_sequence.push(command.getName());
}

void Sequencer::removeCurrentCommand(){
    //clear the entire queue if need be
    Serial.println(this->m_clearQueue);

    if(this->m_clearQueue == true){
      while(!this->isEmpty()){
          this->m_clearQueue = false;
          this->removeCurrentCommand();
      }

      //send a status to central to let it know that the abort has been handled
      if(mp_CC != nullptr){
        mp_CC->writeMessageToCentral("commandStatus", "0");
      }

      return;
    }

    //find last command and pop it
    String lastCommandName = this->m_sequence.front();

    Serial.println("Removing Last Command: " + lastCommandName);

    if(lastCommandName == "FollowLineUntilMarker"){
        this->m_followLineUntilMarkerQueue.pop();
    }else if(lastCommandName == "FollowLineOnMarker"){
        this->m_followLineOnMarkerQueue.pop();
    }else if(lastCommandName == "FollowLineUntilAction"){
        this->m_followLineUntilActionQueue.pop();
    }else if(lastCommandName == "TravelStraight"){
        this->m_travelStraightQueue.pop();
    }else if(lastCommandName == "TurnLeft"){
        this->m_turnLeftQueue.pop();
    }else if(lastCommandName == "TurnRight"){
        this->m_turnRightQueue.pop();
    }else if(lastCommandName == "WaitCommand"){
        this->m_waitCommandQueue.pop();
    }else if(lastCommandName == "TestCommand"){
        this->m_testCommandQueue.pop();
    }else{
        Serial.println("Unknown command name detected: " + lastCommandName);
    }

    //remove last command from sequencer
    this->m_sequence.pop();

    //if the sequencer is empty after this command, let central know it is finished
    if(this->isEmpty()){
        this->updateStatus(254);
    }
}

Command* Sequencer::getNextCommand(){
    //move onto the next command

    //remove the current command
    this->removeCurrentCommand();

    if(this->isEmpty()){
        //if there is no next command
        return nullptr;
    }

    //return a reference to the command at the front of the sequencer
    return this->getCurrentCommand();
}

Command* Sequencer::getCurrentCommand(){
    //get the command currently at the front of the sequencer
    String currentCommandName = this->m_sequence.front();
    
    if(currentCommandName == "FollowLineUntilMarker"){
        return &(this->m_followLineUntilMarkerQueue.front());
    }else if(currentCommandName == "FollowLineOnMarker"){
        return &(this->m_followLineOnMarkerQueue.front());
    }else if(currentCommandName == "FollowLineUntilAction"){
        return &(this->m_followLineUntilActionQueue.front());
    }else if(currentCommandName == "TravelStraight"){
        return &(this->m_travelStraightQueue.front());
    }else if(currentCommandName == "TurnLeft"){
        return &(this->m_turnLeftQueue.front());
    }else if(currentCommandName == "TurnRight"){
        return &(this->m_turnRightQueue.front());
    }else if(currentCommandName == "WaitCommand"){
        return &(this->m_waitCommandQueue.front());
    }else if(currentCommandName == "TestCommand"){
        return &(this->m_testCommandQueue.front());
    }else{
        Serial.println("Unknown command name detected: " + currentCommandName);
    }

    return nullptr;
}

bool Sequencer::isEmpty(){
    //returns wether the sequencer is empty or not
    return this->m_sequence.empty();
}

void Sequencer::clear(){
  //set the clear flag to true
  //this will clear the command queue at the next safe point (removal of next command)
  this->m_clearQueue = true;
}

void Sequencer::setCommunicatorPointer(Communicator* CC){
    this->mp_CC = CC;
}

void Sequencer::updateStatus(uint8_t status){
    //update the status and write to central
    mp_CC->writeMessageToCentral("commandStatus", String(status));
}
