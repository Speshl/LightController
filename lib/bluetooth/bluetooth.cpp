#include "bluetooth.h"

class Callbacks: public BLECharacteristicCallbacks {
  private:
    BluetoothState* bluetooth;

  public:
    Callbacks(BluetoothState* bt){
      bluetooth = bt;
    }
    void onWrite(BLECharacteristic *pCharacteristic){
      std::string uuid = pCharacteristic->getUUID().toString();
      if(uuid == SWITCH_CHARACTERISTIC_UUID){
        Serial.println("New Switch Value Read");
        bluetooth->newSwitchValue = true;
      }else if(uuid == ANIMATION_CHARACTERISTIC_UUID){
        Serial.println("New Animation Value Read");
        bluetooth->newAnimationValue = true;
      }else{
        for(int i=0; i<MAX_CHANNELS; i++){
          if(bluetooth->channelDetailsCharacteristic[i]->getUUID().toString() == uuid){
            Serial.println("New Channel Detail Value Read");
            bluetooth->newDetailValue[i] = true;
          }else if(bluetooth->channelLocationsCharacteristic[i]->getUUID().toString() == uuid){
            Serial.println("New Channel Location Value Read");
            bluetooth->newLocationValue[i] = true;
          }
        }
      }
    }
};

void initializeBluetooth(BluetoothState* bluetooth){
  Serial.println("Initializing Bluetooth");

  char channelLocationUUID[MAX_CHANNELS][37] = {
    "beb5483e-36e1-4688-b7f5-ea07361b2600",
    "beb5483e-36e1-4688-b7f5-ea07361b2601",
    "beb5483e-36e1-4688-b7f5-ea07361b2602",
    "beb5483e-36e1-4688-b7f5-ea07361b2603",
    "beb5483e-36e1-4688-b7f5-ea07361b2604",
    "beb5483e-36e1-4688-b7f5-ea07361b2605",
    "beb5483e-36e1-4688-b7f5-ea07361b2606",
    "beb5483e-36e1-4688-b7f5-ea07361b2607",
    "beb5483e-36e1-4688-b7f5-ea07361b2608",
    "beb5483e-36e1-4688-b7f5-ea07361b2609",
    "beb5483e-36e1-4688-b7f5-ea07361b260a",
    "beb5483e-36e1-4688-b7f5-ea07361b260b",
    "beb5483e-36e1-4688-b7f5-ea07361b260c",
    "beb5483e-36e1-4688-b7f5-ea07361b260d",
    "beb5483e-36e1-4688-b7f5-ea07361b260e",
    "beb5483e-36e1-4688-b7f5-ea07361b260f",
  };

  char channelDetailUUID[MAX_CHANNELS][37] = {
    "beb5483e-36e1-4688-b7f5-ea07361b2610",
    "beb5483e-36e1-4688-b7f5-ea07361b2611",
    "beb5483e-36e1-4688-b7f5-ea07361b2612",
    "beb5483e-36e1-4688-b7f5-ea07361b2613",
    "beb5483e-36e1-4688-b7f5-ea07361b2614",
    "beb5483e-36e1-4688-b7f5-ea07361b2615",
    "beb5483e-36e1-4688-b7f5-ea07361b2616",
    "beb5483e-36e1-4688-b7f5-ea07361b2617",
    "beb5483e-36e1-4688-b7f5-ea07361b2618",
    "beb5483e-36e1-4688-b7f5-ea07361b2619",
    "beb5483e-36e1-4688-b7f5-ea07361b261a",
    "beb5483e-36e1-4688-b7f5-ea07361b261b",
    "beb5483e-36e1-4688-b7f5-ea07361b261c",
    "beb5483e-36e1-4688-b7f5-ea07361b261d",
    "beb5483e-36e1-4688-b7f5-ea07361b261e",
    "beb5483e-36e1-4688-b7f5-ea07361b261f"
  };
  
  BLEDevice::init("SquadGlow");
  bluetooth->newBLEValue = false;
  bluetooth->pServer = BLEDevice::createServer();
  bluetooth->pService = bluetooth->pServer->createService(BLEUUID(SERVICE_UUID),100,0);

  bluetooth->switchCharacteristic = bluetooth->pService->createCharacteristic(SWITCH_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  bluetooth->switchCharacteristic->setCallbacks(new Callbacks(bluetooth));
  bluetooth->animationCharacteristic = bluetooth->pService->createCharacteristic(ANIMATION_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  bluetooth->animationCharacteristic->setCallbacks(new Callbacks(bluetooth));

  for(int i=0; i<MAX_CHANNELS; i++){
    bluetooth->channelDetailsCharacteristic[i] = bluetooth->pService->createCharacteristic(channelDetailUUID[i], BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    bluetooth->channelDetailsCharacteristic[i]->setCallbacks(new Callbacks(bluetooth));
    bluetooth->channelLocationsCharacteristic[i] = bluetooth->pService->createCharacteristic(channelLocationUUID[i], BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    bluetooth->channelLocationsCharacteristic[i]->setCallbacks(new Callbacks(bluetooth));
  }
  bluetooth->pService->start();
  bluetooth->pAdvertising = bluetooth->pServer->getAdvertising();
  bluetooth->pAdvertising->start();
  Serial.println("Bluetooth Initialized");
}

void pauseBluetooth(BluetoothState* bluetooth){
  if(bluetooth->paused == false){
    bluetooth->pAdvertising->stop();
    bluetooth->pService->stop();
    //btStop();
    //BLEDevice::deinit(true);
    Serial.println("Bluetooth Paused");
    bluetooth->paused = true;
  }
}

void unpauseBluetooth(BluetoothState* bluetooth){
  if(bluetooth->paused == true){
    //btStart();
    bluetooth->pService->start();
    bluetooth->pAdvertising->start();
    Serial.println("Bluetooth Unpaused");
    bluetooth->paused = false;
  }
}

std::string getSwitchStateAsString(BluetoothState* bluetooth){
  return bluetooth->switchCharacteristic->getValue();
}

std::string getAnimationStateAsString(BluetoothState* bluetooth){
  return bluetooth->animationCharacteristic->getValue();
}

std::string getChannelDetailsAsString(BluetoothState* bluetooth, int i){
  return bluetooth->channelDetailsCharacteristic[i]->getValue();
}

std::string getChannelLocationsAsString(BluetoothState* bluetooth, int i){
  return bluetooth->channelLocationsCharacteristic[i]->getValue();
}

void setSwitchStateFromString(BluetoothState* bluetooth, std::string input){
  bluetooth->switchCharacteristic->setValue(input);
}

void setAnimationStateFromString(BluetoothState* bluetooth, std::string input){
  bluetooth->animationCharacteristic->setValue(input);
}

void setChannelDetailsFromString(BluetoothState* bluetooth, int i, std::string input){
  bluetooth->channelDetailsCharacteristic[i]->setValue(input);
}

void setChannelLocationFromString(BluetoothState* bluetooth, int i, std::string input){
  bluetooth->channelLocationsCharacteristic[i]->setValue(input);
}

//Maybe make individual functions to check for new values
bool isNewBLEValue(BluetoothState* bluetooth){
  if(bluetooth->newBLEValue){
    return true;
  }
  
  if(bluetooth->newSwitchValue || bluetooth->newAnimationValue || bluetooth->newChannelValue){
    return true;
  }

  for(int i=0; i<MAX_CHANNELS; i++){
    if(bluetooth->newLocationValue[i] == true || bluetooth->newDetailValue[i] == true){
      return true;
    }
  }

  return false;
}

bool isConnected(BluetoothState* bluetooth){
  return bluetooth->connected;
}

//Maybe make individual functions to acknowledge new values
void acknowledgeNewBLEValue(BluetoothState* bluetooth){
  bluetooth->newSwitchValue = false;
  bluetooth->newAnimationValue = false;
  bluetooth->newChannelValue = false;
  bluetooth->newBLEValue = false;
  
  for(int i=0; i<MAX_CHANNELS; i++){
    bluetooth->newLocationValue[i] = false;
    bluetooth->newDetailValue[i] = false;
  }
}
