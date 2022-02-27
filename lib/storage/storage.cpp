#include "storage.h"

std::string to_string( int x ) {
  int length = snprintf( NULL, 0, "%d", x );
  assert( length >= 0 );
  char* buf = new char[length + 1];
  snprintf( buf, length + 1, "%d", x );
  std::string str( buf );
  delete[] buf;
  return str;
}

void initializeStorage(Preferences* preferences){
  preferences->begin("Storage", false);
}

bool isFirstRun(Preferences* preferences){
  bool firstRun = preferences->getBool("FirstRun", true);
  if(firstRun == true){
    preferences->putBool("FirstRun", false);
  }
  return firstRun;
}

void saveSwitchState(Preferences* preferences, std::string value){
  byte tempData[value.length()];
  for(int i=0; i<value.length(); i++){
    tempData[i] = value[i];
  }
  preferences->putBytes("Switch", tempData, value.length());
}

void saveAnimationState(Preferences* preferences, std::string value){
  byte tempData[value.length()];
  for(int i=0; i<value.length(); i++){
    tempData[i] = value[i];
  }
  preferences->putBytes("Animation", tempData, value.length());
}

void saveChannelDetailState(Preferences* preferences, int index, std::string value){
  byte tempData[value.length()];
  for(int i=0; i<value.length(); i++){
    tempData[i] = value[i];
  }
  preferences->putBytes("Details"+index, &tempData, value.length());
}

void saveChannelLocationState(Preferences* preferences, int index, std::string value){
  /*Serial.print("Location Index: ");
  Serial.println(index);
  Serial.print("Value Length: ");
  Serial.println(value.length());*/
  byte tempData[value.length()];
  for(int i=0; i<value.length(); i++){
    tempData[i] = value[i];
  }
  preferences->putBytes("Location"+index, tempData, value.length());
}

std::string loadSwitchState(Preferences* preferences){
  std::string returnValue;
  byte tempData[6];
  uint8_t bytesRead = preferences->getBytes("Switch",tempData,6);
  if(bytesRead == 0){
    return "";
  }
  for(int i=0; i<6; i++){ //Switch State Length 6
    returnValue += tempData[i];
  }
  return returnValue;
}

std::string loadAnimationState(Preferences* preferences){
  std::string returnValue = "";
  byte tempData[47];
  uint8_t bytesRead = preferences->getBytes("Animation",tempData,47);
  if(bytesRead == 0){
    return "";
  }
  for(int i=0; i<47; i++){
    returnValue += tempData[i];
  }
  return returnValue;
}

std::string loadChannelDetailState(Preferences* preferences, int index){
  std::string returnValue = "";
  byte tempData[11];
  uint8_t bytesRead = preferences->getBytes("Details"+index,tempData,11);
  if(bytesRead == 0){
    return "";
  }
  for(int i=0; i<11; i++){
    returnValue += tempData[i];
  }
  return returnValue;
}

std::string loadChannelLocationState(Preferences* preferences, int index, int numLeds){
  //Serial.println("Loading Locations");
  std::string returnValue = "";
  byte tempData[numLeds*2];
 /* Serial.print("Getting Channel ");
  Serial.print(index);
  Serial.print(" locations with LEDS: ");
  Serial.println(numLeds);*/
  uint8_t bytesRead = preferences->getBytes("Location"+index,tempData,numLeds*2);
  if(bytesRead == 0){
    return "";
  }
  for(int i=0; i<numLeds*2; i++){
    returnValue += tempData[i];
  }
  return returnValue;
}
