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
  char numstr[21];
  std::string temp = itoa(index, numstr, 10);
  std::string key = "Detail" + temp;

  byte tempData[value.length()];
  for(int i=0; i<value.length(); i++){
    tempData[i] = value[i];
  }
  preferences->putBytes(key.c_str(), &tempData, value.length());
}

void saveChannelLocationState(Preferences* preferences, int index, uint8_t value[]){
  char numstr[21];
  std::string temp = itoa(index, numstr, 10);
  std::string key = "Loc" + temp;
  preferences->putBytes(key.c_str(), value, 300);
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
  byte tempData[46];
  uint8_t bytesRead = preferences->getBytes("Animation",tempData,46);
  if(bytesRead == 0){
    return "";
  }
  for(int i=0; i<46; i++){
    returnValue += tempData[i];
  }
  return returnValue;
}

std::string loadChannelDetailState(Preferences* preferences, int index){
  char numstr[21];
  std::string temp = itoa(index, numstr, 10);
  std::string key = "Detail" + temp;

  std::string returnValue = "";
  byte tempData[11];
  uint8_t bytesRead = preferences->getBytes(key.c_str(),tempData,11);
  if(bytesRead == 0){
    return "";
  }
  for(int i=0; i<11; i++){
    returnValue += tempData[i];
  }
  return returnValue;
}

void loadChannelLocationState(Preferences* preferences, int index, uint8_t value[]){
  char numstr[21];
  std::string temp = itoa(index, numstr, 10);
  std::string key = "Loc" + temp;
  preferences->getBytes(key.c_str(),value, 3*100);
  return;
}
