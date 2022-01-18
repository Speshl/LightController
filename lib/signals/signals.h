#include <Arduino.h>
#include <channels.h>
#include <bluetooth.h>
#include <animations.h>
#include <location.h>
#include <switches.h>
#include <storage.h>

#define SIGNAL_CORE 0

struct State {
  BluetoothState bluetooth;
  AnimationState animation;
  SwitchState switches;
  ChannelState channels[MAX_CHANNELS];
  LocationState location;
  Preferences preferences;
};

void createSignals(State* state, TaskHandle_t* handle);