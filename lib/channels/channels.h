#ifndef CHANNELS_H
#define CHANNELS_H

#include <FastLED.h>
#include <switches.h>
#define CHANNEL_SIZE 11
#define MAX_CHANNELS 8
#define MAX_LEDS 100
#define MAX_ROWS 4
#define MAX_COLS 3

//https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
//DONT WORK: 27, 32, 33
//GLITCHY: 0 2
//                                        RIGHT SIDE   |      LEFT SIDE                 
const int channelpins[MAX_CHANNELS] = {23,22,3,21,19,18,5,4/*,0,2,15,25*/}; //TODO: Update to actual pin numbers

//{23,22,3,21,19,18,5,4,0,2,15,25,26,14,12,13}
//When looking at board from usb port
//Channel 0 is 2nd pin frrom back on right side
//Channel 1 is 3rd pin from back on right side
//Channel 2 is 5th pin from back on right side
//Channel 3 is 6th pin from back on right side
//Channel 4 is 8th pin from back on right side
//Channel 5 is 9th pin from back on right side
//Channel 6 is 10th pin from back on right side
//Channel 7 is 13th pin from back on right side //P4

//Channel 8 is 14th pin from back on right side
//Channel 9 is maybe 5th pin from front on right side //GLITCHY???
//Channel 10 is 4th pin from front on right side

//Channel 11 is 11th pin from front on left side
//Channel 12 is 10th pin from front on left side
//Channel 13 is 8th pin from front on left side
//Channel 14 is 7th from front on left side
//Channel 15 is 5th from front on left side
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
