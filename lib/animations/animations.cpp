#include "animations.h"
void setInitialState(AnimationState* animation){
  Serial.println("Start setting Initial Animation State");
  animation->animation = 1;
  animation->stepDelay = 60;
  animation->brightness = 255;
  animation->stepSize = 1;
  animation->blending = LINEARBLEND;

  for(int i=0; i<MAX_COLORS*4;i++){
    animation->paletteDescription[i] = 0;
  }
  
  animation->paletteDescription[0] = 0;
  animation->paletteDescription[1] = 255;
  animation->paletteDescription[2] = 0;
  animation->paletteDescription[3] = 255;

  animation->paletteDescription[4] = 255;
  animation->paletteDescription[5] = 0;
  animation->paletteDescription[6] = 255;
  animation->paletteDescription[7] = 255;
  animation->numberOfColors = 2;

  byte tempPalette[animation->numberOfColors * 2];
  for(int i=0; i<animation->numberOfColors*4; i++){
    tempPalette[i] = animation->paletteDescription[i];
  }
  
  animation->palette.loadDynamicGradientPalette(tempPalette);
  Serial.println("Finished setting Initial Animation State");
}



std::string getStateAsString(AnimationState* animation){
  std::string returnValue = "";
  returnValue += animation->animation;
  returnValue += animation->brightness;
  returnValue += animation->stepSize;

  switch(animation->blending){
    case NOBLEND:
      returnValue += '0';
      break;
    case LINEARBLEND:
      returnValue += '1';
      break;
    default:
      returnValue += '1';
  }
  

  AnimationConversion converter;
  converter.wordValue = animation->stepDelay;
  returnValue += converter.charValue[0];
  returnValue += converter.charValue[1];

  returnValue += animation->numberOfColors;

  /*Save palette*/
  for(int i=0; i<MAX_COLORS*4; i++){
    returnValue += animation->paletteDescription[i];
  }
  
  return returnValue;
}

void setStateFromString(AnimationState* animation, std::string input){
  animation->animation = input[0];
  animation->brightness = input[1];
  animation->stepSize = input[2];

  switch(input[3]){
    case 0:
      animation->blending = NOBLEND;
      break;
    case 1:
      animation->blending = LINEARBLEND;
      break;
    default:
      animation->blending = LINEARBLEND;
  }
  
  AnimationConversion converter;
  converter.charValue[0] = input[4];
  converter.charValue[1] = input[5];
  animation->stepDelay = converter.wordValue;

  animation->numberOfColors = input[6];

  /*Read palette*/
  byte tempPalette[4*MAX_COLORS];
  for(int i=0; i<4*MAX_COLORS; i++){
    animation->paletteDescription[i] = input[i+7];
    tempPalette[i] = input[i+7];
  }

  //https://forum.makerforums.info/t/hi-is-it-possible-to-define-a-gradient-palette-at-runtime-the-define-gradient-palette-uses-the/63339/4
  //byte testPalette [8] = {0,255,255,0,255,0,255,255};
  animation->palette.loadDynamicGradientPalette(tempPalette);
}

void describeState(AnimationState* animation){
  Serial.println("Describing Animation State - ");
  Serial.print("Animation: ");
  Serial.println((uint8_t) animation->animation);//So it displays as number
  Serial.print("StepSize: ");
  Serial.println(animation->stepSize);//So it displays as number

  Serial.print("NumberOfColors: ");
  Serial.println(animation->numberOfColors);
}

void solidFromPalette(AnimationState* animation, ChannelState channels[MAX_CHANNELS], SwitchState* switches, BluetoothState* bluetooth, LocationState* location){
  CRGB color = ColorFromPalette( animation->palette, animation->stepIndex, animation->brightness, animation->blending);
  setColorAll(location, channels, color);
  applyBufferAndShow(channels,switches);
  animation->stepIndex += animation->stepSize;
}

void sweepUpPalette(AnimationState* animation, ChannelState channels[MAX_CHANNELS], SwitchState* switches, BluetoothState* bluetooth, LocationState* location){
  //Serial.println("Starting Sweep Up From Palette command");
  CRGB color = ColorFromPalette( animation->palette, animation->stepIndex, animation->brightness, animation->blending);
  shiftColorUpRow(location, channels, color);
  applyBufferAndShow(channels,switches);
  animation->stepIndex += animation->stepSize;
  //Serial.println("Finished Sweep Up From Palette command");
}

void sweepDownPalette(AnimationState* animation, ChannelState channels[MAX_CHANNELS], SwitchState* switches, BluetoothState* bluetooth, LocationState* location){
  //Serial.println("Starting Sweep Up From Palette command");
  CRGB color = ColorFromPalette( animation->palette, animation->stepIndex, animation->brightness, animation->blending);
  shiftColorDownRow(location, channels, color);
  applyBufferAndShow(channels,switches);
  animation->stepIndex += animation->stepSize;
  //Serial.println("Finished Sweep Up From Palette command");
}

void sweepRightPalette(AnimationState* animation, ChannelState channels[MAX_CHANNELS], SwitchState* switches, BluetoothState* bluetooth, LocationState* location){
  //Serial.println("Starting Sweep Up From Palette command");
  CRGB color = ColorFromPalette( animation->palette, animation->stepIndex, animation->brightness, animation->blending);
  shiftColorUpCol(location, channels, color);
  applyBufferAndShow(channels,switches);
  animation->stepIndex += animation->stepSize;
  //Serial.println("Finished Sweep Up From Palette command");
}

void sweepLeftPalette(AnimationState* animation, ChannelState channels[MAX_CHANNELS], SwitchState* switches, BluetoothState* bluetooth, LocationState* location){
  //Serial.println("Starting Sweep Up From Palette command");
  CRGB color = ColorFromPalette( animation->palette, animation->stepIndex, animation->brightness, animation->blending);
  shiftColorDownCol(location, channels, color);
  applyBufferAndShow(channels,switches);
  animation->stepIndex += animation->stepSize;
  //Serial.println("Finished Sweep Up From Palette command");
}

void brakeAnimation(ChannelState channels[MAX_CHANNELS], SwitchState* switches, BluetoothState* bluetooth){
  Serial.println("Start Brake Animation");
  fillBrake(channels, CRGB::Red);
  Serial.println("Finished Brake Animation");
}

void reverseAnimation(ChannelState channels[MAX_CHANNELS], SwitchState* switches, BluetoothState* bluetooth){
  Serial.println("Start Reverse Animation");
  fillReverse(channels, CRGB::White);
  Serial.println("Finished Reverse Animation");
}

void leftTurnAnimationWithColor(ChannelState channels[MAX_CHANNELS], SwitchState* switches, BluetoothState* bluetooth, CRGB color){
  Serial.println("Starting Left Turn Animation");
  fillLeftTurn(channels, color);
  Serial.println("Finished Left Turn Animation");
}

void rightTurnAnimationWithColor(ChannelState channels[MAX_CHANNELS], SwitchState* switches, BluetoothState* bluetooth, CRGB color){
  Serial.println("Start Right Turn Animation");
  fillRightTurn(channels, color);
  Serial.println("Finished Right Turn Animation");
}

uint16_t getStepDelay(AnimationState* animation, uint16_t frameTime){
  if(frameTime > animation->stepDelay){
    return 1;
  }else{
      return animation->stepDelay - frameTime;
  }
}

void updateAnimation(AnimationState* animation, ChannelState channels[MAX_CHANNELS], SwitchState* switches, BluetoothState* bluetooth, LocationState* location){
  switch(animation->animation){
      case 0:
        solidFromPalette(animation, channels, switches, bluetooth, location);
        break;
      case 1:
        sweepUpPalette(animation, channels, switches, bluetooth, location);
        break;
      case 2:
        sweepDownPalette(animation, channels, switches, bluetooth, location);
        break;
      case 3:
        sweepRightPalette(animation, channels, switches, bluetooth, location);
        break;
      case 4:
        sweepLeftPalette(animation, channels, switches, bluetooth, location);
        break;
      default:
        solidFromPalette(animation, channels, switches, bluetooth, location);
  }
}

std::string getAnimationIndex(AnimationState* animation){
  char buffer[5];
  return itoa(animation->animation,buffer, 10);
}

std::string getStepSize(AnimationState* animation){
  char buffer[10];
  return itoa(animation->stepSize,buffer, 10);
}

std::string getNumberOfColors(AnimationState* animation){
  char buffer[10];
  return itoa((int)animation->numberOfColors,buffer, 10);
}

std::string getColorData(AnimationState* animation){
  std::string fullData;
  for(int i=0; i<40; i++){
    if(i%4 == 0){
      fullData += "\r\n";
    }
    char buffer[5];
    int temp = animation->paletteDescription[i];
    fullData += itoa(temp, buffer, 10);
    if(i != 39){
      fullData += ",";
    }
  }
  return fullData;
}

void setColorData(AnimationState* animation, std::string colorData){
  int pos = 0;
  std::string buffer[MAX_COLORS*4];

  for(int i=0; i<colorData.length(); i++){
    if(pos/4 >= animation->numberOfColors){
      break;
    }
    if(colorData[i] == ','){
      pos++;
    }else if(colorData[i] != ','){
      buffer[pos] += colorData[i];
    }
  }

  for(int i=0; i<MAX_COLORS*4; i++){
    int bufferInt = atoi(buffer[i].c_str());
    animation->paletteDescription[i] = bufferInt;
    Serial.print("Setting Palette ");
    Serial.print(i);
    Serial.print(" to value ");
    Serial.println(bufferInt);
  }
}

void setColorData2(AnimationState* animation, std::string colorData){
  int pos = 0;
  for(int i=0; i<animation->numberOfColors; i++){
    for(int k=0; k<4; k++){
    std::string dataPoint;
      for(int j=pos; j < colorData.length(); j++){
        if(colorData[j] == ','){
          pos = j+1;
          break;
        }else if(colorData[j] == '\r' || colorData[j] == '\n' ){
          pos = j+1;
          break;
        }else{
          dataPoint += colorData[j];
        }
      }
      int numberFromData = atoi(dataPoint.c_str());
      animation->paletteDescription[4*i+k] = numberFromData;
      Serial.print("Setting Palette ");
      Serial.print(4*i+k);
      Serial.print(" to value ");
      Serial.println(numberFromData);
    }
  }
}

void animationUpdated(AnimationState* animation, ChannelState channels[MAX_CHANNELS]){
  animation->stepIndex = 0;
  clear(channels);
}