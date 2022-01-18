#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#define MAX_CHANNELS 8

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b" //TODO: Change these
#define SWITCH_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a0"
#define ANIMATION_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a1"

struct BluetoothState {
  BLEServer *pServer;
  BLEService *pService;
  BLECharacteristic *switchCharacteristic;
  BLECharacteristic *animationCharacteristic;
  BLECharacteristic *channelDetailsCharacteristic[MAX_CHANNELS];
  BLECharacteristic *channelLocationsCharacteristic[MAX_CHANNELS];
  BLEAdvertising *pAdvertising;
  bool newBLEValue; //Will be depreciated used as accumulation of below
  bool newAnimationValue;
  bool newSwitchValue;
  bool newChannelValue;
  bool newLocationValue[MAX_CHANNELS];
  bool newDetailValue[MAX_CHANNELS];
  bool connected;
};

void initializeBluetooth(BluetoothState* bluetooth);

std::string getSwitchStateAsString(BluetoothState* bluetooth);
std::string getAnimationStateAsString(BluetoothState* bluetooth);
std::string getChannelDetailsAsString(BluetoothState* bluetooth, int i);
std::string getChannelLocationsAsString(BluetoothState* bluetooth, int i);

void setSwitchStateFromString(BluetoothState* bluetooth, std::string input);
void setAnimationStateFromString(BluetoothState* bluetooth, std::string input);
void setChannelDetailsFromString(BluetoothState* bluetooth, int i, std::string input);
void setChannelLocationFromString(BluetoothState* bluetooth, int i, std::string input);

bool isNewBLEValue(BluetoothState* bluetooth);

bool isConnected(BluetoothState* bluetooth);

void acknowledgeNewBLEValue(BluetoothState* bluetooth);

#endif
