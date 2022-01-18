#include "switches.h"

bool getSwitchState(int pin){
  if(digitalRead(pin) == HIGH){//Switch is considered on when + signal is applied
    return false;//TODO: SET BACK TO FALSE
  }else{
    return false;
  }
}

bool updateSwitchState(SwitchState* switches){
  //Serial.println("Start Updating Switch State");
  if(switches->uiOverride == true){ //UI Override is on so we don't want to use state changes from the physical switches
    return false;
  }

  bool leftTurn = getSwitchState(LEFT_TURN_PIN);
  bool rightTurn = getSwitchState(RIGHT_TURN_PIN);
  bool brake = getSwitchState(BRAKE_PIN);
  bool reverse = getSwitchState(REVERSE_PIN);
  bool interior = getSwitchState(INTERIOR_LIGHT_PIN);
  bool updated = false;

  if(switches->leftTurn != leftTurn){
    switches->leftTurn = leftTurn;
    updated = true;
  }
  
  if(switches->rightTurn != rightTurn){
    switches->rightTurn = rightTurn;
    updated = true;
  }
  
  if(switches->brake != brake){
    switches->brake = brake;
    updated = true;
  }
  
  if(switches->reverse != reverse){
    switches->reverse = reverse;
    updated = true;
  }

  if(switches->interior  != interior){
    switches->interior = interior;
    updated = true;
  }
  //Serial.println("Finished Updating Switch State");
  return updated;
}

std::string getStateAsString(SwitchState* switches){
  std::string returnString = "";
  returnString += switches->leftTurn;
  returnString += switches->rightTurn;
  returnString += switches->brake;
  returnString += switches->reverse;
  returnString += switches->interior;
  returnString += switches->uiOverride;
  return returnString;
}

void setStateFromString(SwitchState* switches, std::string inputValue){
  if(inputValue.length() != SWITCH_STATE_LENGTH){
    Serial.print("Expected switch state length of ");
    Serial.print(SWITCH_STATE_LENGTH);
    Serial.print(" - Found length of ");
    Serial.println(inputValue.length());
  }
  switches->leftTurn = inputValue[0];
  switches->rightTurn = inputValue[1];
  switches->brake = inputValue[2];
  switches->reverse = inputValue[3];
  switches->interior = inputValue[4];
  switches->uiOverride = inputValue[5];
}

void describeState(SwitchState* switches){
  Serial.println("Describing Switch State - ");
  Serial.print("LeftTurn: ");
  Serial.println(switches->leftTurn);//So it displays as number
  Serial.print("RightTurn: ");
  Serial.println(switches->rightTurn);//So it displays as number
  Serial.print("Brake: ");
  Serial.println(switches->brake);//So it displays as number
  Serial.print("Reverse: ");
  Serial.println(switches->reverse);//So it displays as numberer
  Serial.print("Interior: ");
  Serial.println(switches->interior);//So it displays as number
  Serial.print("UI Override: ");
  Serial.println(switches->uiOverride);//So it displays as number
}
