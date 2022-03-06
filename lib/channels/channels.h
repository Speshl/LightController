#ifndef CHANNELS_H
#define CHANNELS_H

#include <FastLED.h>
#include <switches.h>
#define CHANNEL_SIZE 11
#define MAX_CHANNELS 16
#define MAX_LEDS 100
#define MAX_ROWS 4
#define MAX_COLS 3

//Also Neeed to update static declarations in getDefinition() 
//FIRST 8: GPIO_NUM_23,GPIO_NUM_22,GPIO_NUM_21,GPIO_NUM_19,GPIO_NUM_18,GPIO_NUM_5,GPIO_NUM_17,GPIO_NUM_16
//LAST 8:  ,GPIO_NUM_4,GPIO_NUM_0,GPIO_NUM_2,GPIO_NUM_15,GPIO_NUM_13,GPIO_NUM_12,GPIO_NUM_14,GPIO_NUM_27
const int channelpins[MAX_CHANNELS] = {GPIO_NUM_23,GPIO_NUM_22,GPIO_NUM_21,GPIO_NUM_19,GPIO_NUM_18,GPIO_NUM_5,GPIO_NUM_17,GPIO_NUM_16,GPIO_NUM_4,GPIO_NUM_0,GPIO_NUM_2,GPIO_NUM_15,GPIO_NUM_13,GPIO_NUM_12,GPIO_NUM_14,GPIO_NUM_27};

union ChannelsConversion {
  uint16_t wordValue;
  char charValue[2];
};

#define CHANNEL_STATE_LENGTH 11

struct ChannelState {
  bool enabled;
  bool interior;
  bool directionFlipped;
  bool leftTurn;
  bool rightTurn;
  bool brake;
  bool reverse;
  char type;
  char order;
  uint16_t numLEDs;
  //Below not needed to rebuild
  CRGB leds[MAX_LEDS];
  CRGB ledsBuffer[MAX_LEDS];
  CLEDController* controller;
  bool updated;
  bool restartRequired;
};

//TODO: Remove channel from function names, we know we are working on a channel by the input parameters

//Overall State Functions
void setChannelPins();

void loadTestData(ChannelState channels[MAX_CHANNELS]);

std::string getStateAsString(ChannelState channels[MAX_CHANNELS], int i);

void setInitialState(ChannelState channels[MAX_CHANNELS]);

void setStateFromString(ChannelState channels[MAX_CHANNELS], int i, std::string inputValue);

void describeState(ChannelState channels[MAX_CHANNELS]);

void getDefinition(ChannelState * channel, int index);

//Channel functionality
void showAll(ChannelState channels[MAX_CHANNELS]);

void applyBufferAndShow(ChannelState channels[MAX_CHANNELS], SwitchState* switches);

bool restartRequired(ChannelState channels[MAX_CHANNELS]);

CRGB getColor(ChannelState channels[MAX_CHANNELS],int red, int green, int blue);

bool isActive(ChannelState* channel);

bool isActiveWithSpecialCases(ChannelState* channel, SwitchState* switches);

bool update(ChannelState channel, SwitchState* switches, int pos, CRGB color);

void clear(ChannelState channels[MAX_CHANNELS]);

void fill(ChannelState channels[MAX_CHANNELS], CRGB color);

void fillBrake(ChannelState channels[MAX_CHANNELS], CRGB color);

void fillReverse(ChannelState channels[MAX_CHANNELS], CRGB color);

void fillLeftTurn(ChannelState channels[MAX_CHANNELS], CRGB color);

void fillRightTurn(ChannelState channels[MAX_CHANNELS], CRGB color);

void setColorAtPos(ChannelState* channel, int pos, CRGB color);

CRGB getColorAtPos(ChannelState* channel, int pos);

std::string getChannelEnabled(ChannelState* channel);
std::string getChannelInterior(ChannelState* channel);
std::string getChannelDirectionFlipped(ChannelState* channel);
std::string getChannelLeftTurn(ChannelState* channel);
std::string getChannelRightTurn(ChannelState* channel);
std::string getChannelBrake(ChannelState* channel);
std::string getChannelReverse(ChannelState* channel);
std::string getChannelType(ChannelState* channel);
std::string getChannelOrder(ChannelState* channel);
std::string getChannelNumLEDs(ChannelState* channel);

#endif
