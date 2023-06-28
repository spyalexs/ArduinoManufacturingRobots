#include "Sequencer.h"

Sequencer::Sequencer(){

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

void Sequencer::removeCurrentCommand(){
    //find last command and pop it
    String lastCommandName = this->m_sequence.front();

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
    }else if(lastCommandName == "TestCommand"){
        this->m_testCommandQueue.pop();
    }else{
        Serial.println("Unknown command name detected!");
    }

    //remove last command from sequencer
    this->m_sequence.pop();
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
    }else if(currentCommandName == "TestCommand"){
        return &(this->m_testCommandQueue.front());
    }else{
        Serial.println("Unknown command name detected!");
    }

    return nullptr;
}

bool Sequencer::isEmpty(){
    //returns wether the sequencer is empty or not
    return this->m_sequence.empty();
}

void Sequencer::clear(){
    while(!this->isEmpty()){
        this->removeCurrentCommand();
    }
}
