#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <FastLED.h>
#include <Arduino.h>
#include <channels.h>
#include <bluetooth.h>
#include <location.h>
#include "string"


#define ANIMATION_STATE_LENGTH 47
#define MAX_COLORS 10

struct AnimationState {
  uint8_t animation;
  uint8_t brightness;
  uint8_t stepSize;
  TBlendType blending;
  uint16_t stepDelay;
  byte paletteDescription[MAX_COLORS*4];
  //Below not used to rebuild state
  uint8_t numberOfColors;
  CRGBPalette16 palette;
  uint8_t stepIndex;
  unsigned long frameToFrameTime;
  unsigned long lastFrameTime;
  unsigned long numFrames;
};

union AnimationConversion {
  uint16_t wordValue;
  char charValue[2];
};

bool delayAndPollForUpdate(BluetoothState* bluetooth, int delay);

void setInitialState(AnimationState* animation);

std::string getStateAsString(AnimationState* animation);

void setStateFromString(AnimationState* animation, std::string input);

void describeState(AnimationState* animation);

uint16_t getStepDelay(AnimationState* animation, uint16_t frameTime);

void updateAnimation(AnimationState* animation, ChannelState channels[MAX_CHANNELS], SwitchState* switches, BluetoothState* bluetooth, LocationState* location);

void brakeAnimation(ChannelState channels[MAX_CHANNELS], SwitchState* switches, BluetoothState* bluetooth);

void reverseAnimation(ChannelState channels[MAX_CHANNELS], SwitchState* switches, BluetoothState* bluetooth);

void leftTurnAnimationWithColor(ChannelState channels[MAX_CHANNELS], SwitchState* switches, BluetoothState* bluetooth, CRGB color);

void rightTurnAnimationWithColor(ChannelState channels[MAX_CHANNELS], SwitchState* switches, BluetoothState* bluetooth, CRGB color);

std::string getAnimationIndex(AnimationState* animation);

std::string getStepSize(AnimationState* animation);

std::string getNumberOfColors(AnimationState* animation);

std::string getColorData(AnimationState* animation);

void setColorData(AnimationState* animation, std::string colorData);

void animationUpdated(AnimationState* animation, ChannelState channels[MAX_CHANNELS]);

#endif
