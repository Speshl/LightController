#include "animations.h"

bool delayAndPollForUpdate(SwitchState* switches, BluetoothState* bluetooth, int delay){
  unsigned long time = millis();
  while(millis() < time + delay){
    if(isNewBLEValue(bluetooth) == true || updateSwitchState(switches) == true){
      return true;
    }
  }
  return false;
}

void setInitialState(AnimationState* animation){
  Serial.println("Start setting Initial Animation State");
  animation->animation = 1;
  animation->stepDelay = 60;
  animation->brightness = 255;
  animation->stepSize = 1;
  animation->blending = LINEARBLEND;
  
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
    tempPalette[i] == animation->paletteDescription[i];
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

  /*Save palette*/
  for(int i=0; i<animation->numberOfColors*4; i++){
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

  /*Read palette*/
  //Get number of colors in palete
  animation->numberOfColors = (input.length() - 6) / 4; //Total number of bytes left in string divided by 4 bytes per color
  byte tempPalette[4*animation->numberOfColors];
  for(int i=0; i<4*animation->numberOfColors; i++){
    animation->paletteDescription[i] = input[i+6];
    tempPalette[i] = input[i+6];
  }

  //https://forum.makerforums.info/t/hi-is-it-possible-to-define-a-gradient-palette-at-runtime-the-define-gradient-palette-uses-the/63339/4
  //byte testPalette [8] = {0,255,255,0,255,0,255,255};
  animation->palette.loadDynamicGradientPalette(tempPalette);
}

void describeState(AnimationState* animation){
  Serial.println("Describing Animation State - ");
  Serial.print("Animation: ");
  Serial.println((uint8_t) animation->animation);//So it displays as number
  Serial.print("StepDelay: ");
  Serial.println(animation->stepDelay);//So it displays as number
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

uint16_t getStepDelay(uint16_t stepDelay, uint16_t frameTime){
  if(frameTime > stepDelay){
    return 1;
  }else{
      return stepDelay - frameTime;
  }
}

void updateAnimation(AnimationState* animation, ChannelState channels[MAX_CHANNELS], SwitchState* switches, BluetoothState* bluetooth, LocationState* location){
  unsigned long frameStartTime = millis();

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
  unsigned long currentTime = millis();
  Serial.print("Frame Time: ");
  uint16_t frameTime = currentTime - frameStartTime;
  Serial.println(frameTime);

  uint16_t adjustedStepDelay = getStepDelay(animation->stepDelay, frameTime);
  /*Serial.print("Adjusted Step Delay: ");
  Serial.println(adjustedStepDelay);*/

  if(delayAndPollForUpdate(switches, bluetooth, adjustedStepDelay) == true){
    Serial.println("New BLE value found, breaking out of delay early to update.");
  }

  Serial.print("FrameToFrame Time: ");
  currentTime = millis();
  animation->frameToFrameTime = currentTime - animation->lastFrameTime;
  animation->lastFrameTime = currentTime;
  Serial.println(animation->frameToFrameTime);
}