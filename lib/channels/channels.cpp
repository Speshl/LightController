#include "channels.h"

CRGB getColor(ChannelState* channel, CRGB color){
  //return color;
  return getColor(channel, color.red, color.green, color.blue);
}

CRGB getColor(ChannelState* channel,int red, int green, int blue){
  CRGB color;
  switch(channel->order){
    case 0: //RGB
      color.red = red;
      color.green = green;
      color.blue = blue;
      break;
    case 1: //RBG
      color.red = red;
      color.green = blue;
      color.blue = green;
      break;
    case 2: //BRG
      color.red = blue;
      color.green = red;
      color.blue = green;
      break;
    case 3: //BGR
      color.red = blue;
      color.green = green;
      color.blue = red;
      break;
    case 4: //GBR
      color.red = green;
      color.green = blue;
      color.blue = red;
      break;
    case 5: //GRB
      color.red = green;
      color.green = red;
      color.blue = blue;
      break;
  }
  return color;
}

void showAll(ChannelState channels[MAX_CHANNELS]){
  //Serial.println("Showing All");
  for(int i=0; i<MAX_CHANNELS; i++){
    if(channels[i].updated == true && channels[i].controller != NULL){
      //Serial.print("Showing Channel ");
      //Serial.println(i);
      channels[i].controller->showLeds();
      channels[i].updated = false;
    }
  }
}

void applyBufferAndShow(ChannelState channels[MAX_CHANNELS], SwitchState* switches){
  for(int i=0; i<MAX_CHANNELS; i++){
    if(isActiveWithSpecialCases(&channels[i], switches) == true){
      //Serial.printf("Applying Buffer for channel: ");
      //Serial.println(i);
      channels[i].updated = true;
      for(int j=0; j<channels[i].numLEDs; j++){
        int actualPosition = j;
        if(channels[i].directionFlipped == true){
          actualPosition = (channels[i].numLEDs-1) - j;
        }
        channels[i].leds[actualPosition] = getColor(&channels[i], channels[i].ledsBuffer[j]);
        //channels[i].leds[j] = channels[i].ledsBuffer[j];
      }
    }
  }
  showAll(channels);
}

bool restartRequired(ChannelState channels[MAX_CHANNELS]){
  bool restart = false;
  for(int i=0; i<MAX_CHANNELS; i++){
    if(channels[i].restartRequired == true){
      restart = true;
    }
  }
  return restart;
}

bool isActive(ChannelState* channel){
  if(channel->enabled == true && channel->numLEDs > 0){
    return true;
  }
  return false;
}

bool isActiveWithSpecialCases(ChannelState* channel, SwitchState* switches){
  if(channel->leftTurn == true && switches->leftTurn == true){ //Don't modify this channel since the signal is on
    return false;
  }

  if(channel->rightTurn == true && switches->rightTurn == true){
    return false;
  }

  if(channel->brake == true && switches->brake == true){
    return false;
  }

  if(channel->reverse == true && switches->reverse == true){
    return false;
  }

  if(channel->interior == true && switches->interior == true){
    return false;
  }
  return isActive(channel);
}

std::string getStateAsString(ChannelState channels[MAX_CHANNELS], int i){
  //Serial.println("Getting channel state from string");
  std::string returnValue = "";
  std::string builderString = "";
  builderString += channels[i].enabled;
  builderString += channels[i].interior;
  builderString += channels[i].directionFlipped;
  builderString += channels[i].leftTurn;
  builderString += channels[i].rightTurn;
  builderString += channels[i].brake;
  builderString += channels[i].reverse;
  builderString += channels[i].type;
  builderString += channels[i].order;
  
  ChannelsConversion converter;
  converter.wordValue = channels[i].numLEDs;
  builderString += converter.charValue[0];
  builderString += converter.charValue[1];    
  returnValue.append(builderString);
  return returnValue;
}

void setStateFromString(ChannelState channels[MAX_CHANNELS],int i, std::string inputValue){
  //Serial.println("Setting channel state from string");
  if (inputValue.length() < CHANNEL_SIZE){
    Serial.print("Invalid string used to set Channel State (Length: ");
    Serial.print(inputValue.length());
    Serial.print(") : ");
    Serial.println(inputValue.c_str());
    return;
  }
  int currentPos = 0;
  bool newTypeFound = false;

  channels[i].enabled = inputValue[currentPos];
  channels[i].interior = inputValue[currentPos+1];
  channels[i].directionFlipped = inputValue[currentPos+2];
  channels[i].leftTurn = inputValue[currentPos+3];
  channels[i].rightTurn = inputValue[currentPos+4];
  channels[i].brake = inputValue[currentPos+5];
  channels[i].reverse = inputValue[currentPos+6];

  char newType = inputValue[currentPos+7];
  if(newType != channels[i].type){
    channels[i].type = newType;
    newTypeFound = true;
  }

  channels[i].order = inputValue[currentPos+8];

  ChannelsConversion converter;
  converter.charValue[0] = inputValue[currentPos+9];
  converter.charValue[1] = inputValue[currentPos+10];
  channels[i].numLEDs = converter.wordValue;
  
  if(channels[i].enabled == true){
    if(channels[i].controller == NULL){
      getDefinition(&(channels[i]), i); //Get a controller where one didn't already exist
    }else{
      if(newTypeFound){
        channels[i].restartRequired = true; //if new type is found then must restart controller before it will work
      }
    }
  }
  clear(channels);
  //Serial.println("Showing All after state from string update for channels");
  showAll(channels);
}

void describeState(ChannelState channels[MAX_CHANNELS]){
  Serial.println("Describing Channels State - ");
  for(int i=0; i<MAX_CHANNELS; i++){
    Serial.print("Describing Channel :");
    Serial.println(i);
    Serial.print("Enabled: ");
    Serial.println((uint8_t) channels[i].enabled);//So it displays as number
    Serial.print("Interior: ");
    Serial.println((uint8_t) channels[i].interior);//So it displays as number
    Serial.print("DirectionFlipped: ");
    Serial.println((uint8_t) channels[i].directionFlipped);//So it displays as number
    Serial.print("LeftTurn: ");
    Serial.println((uint8_t) channels[i].leftTurn);//So it displays as number
    Serial.print("RightTurn: ");
    Serial.println((uint8_t) channels[i].rightTurn);//So it displays as number
    Serial.print("Brake: ");
    Serial.println((uint8_t) channels[i].brake);//So it displays as number
    Serial.print("Reverse: ");
    Serial.println((uint8_t) channels[i].reverse);//So it displays as number
    Serial.print("Type: ");
    Serial.println((uint8_t) channels[i].type);//So it displays as number
    Serial.print("Order: ");
    Serial.println((uint8_t) channels[i].order);//So it displays as number 
    Serial.print("NumLEDs: ");
    Serial.println(channels[i].numLEDs);
  }
}

void clear(ChannelState channels[MAX_CHANNELS]){
  for(int i=0; i<MAX_CHANNELS; i++){
    fill_solid(channels[i].leds, channels[i].numLEDs, CRGB(0,0,0));
    fill_solid(channels[i].ledsBuffer, channels[i].numLEDs, CRGB(0,0,0));
    channels[i].updated = true;
  }
}

void fill(ChannelState channels[MAX_CHANNELS], CRGB color){
  for(int i=0; i<MAX_CHANNELS; i++){
    if(isActive(&channels[i]) == true){
      fill_solid(channels[i].ledsBuffer, channels[i].numLEDs, getColor(&channels[i],color));
      channels[i].updated = true;
    }
  }
}

void fillBrake(ChannelState channels[MAX_CHANNELS], CRGB color){
  for(int i=0; i<MAX_CHANNELS; i++){
    if(channels[i].brake == true){
      fill_solid(channels[i].leds, channels[i].numLEDs, getColor(&channels[i],color));
      channels[i].updated = true;
    }
  }
}

void fillReverse(ChannelState channels[MAX_CHANNELS], CRGB color){
  for(int i=0; i<MAX_CHANNELS; i++){
    if(channels[i].reverse == true){
      fill_solid(channels[i].leds, channels[i].numLEDs, getColor(&channels[i],color));
      channels[i].updated = true;
    }
  }
}

void fillLeftTurn(ChannelState channels[MAX_CHANNELS], CRGB color){
  for(int i=0; i<MAX_CHANNELS; i++){
    if(channels[i].leftTurn == true){
      fill_solid(channels[i].leds, channels[i].numLEDs, getColor(&channels[i],color));
      channels[i].updated = true;
    }
  }
}

void fillRightTurn(ChannelState channels[MAX_CHANNELS], CRGB color){
  for(int i=0; i<MAX_CHANNELS; i++){
    if(channels[i].rightTurn == true){
      fill_solid(channels[i].leds, channels[i].numLEDs, getColor(&channels[i],color));
      channels[i].updated = true;
    }
  }
}

void setColorAtPos(ChannelState* channel, int pos, CRGB color){
  channel->ledsBuffer[pos] = color;
}

CRGB getColorAtPos(ChannelState* channel, int pos){
  if(channel->numLEDs > pos){
    return channel->ledsBuffer[pos];
  }else{
    return CRGB::Black;
  }
}

void setChannelPins(){
  for(int i=0; i<MAX_CHANNELS; i++){
    pinMode(channelpins[i], OUTPUT);
  }
}

void setInitialState(ChannelState channels[MAX_CHANNELS]){
  Serial.println("Start Setting Initial Channel State");

  for(int i=0; i<MAX_CHANNELS; i++){
    channels[i].enabled = false;
    channels[i].interior = false;
    channels[i].directionFlipped = false;
    channels[i].leftTurn = false;
    channels[i].rightTurn = false;
    channels[i].brake = false;
    channels[i].reverse = false;
    channels[i].type = 1;
    channels[i].order = 5;
    channels[i].numLEDs = 0;
    memset(channels[i].leds,0,sizeof(channels[i].leds));
    memset(channels[i].ledsBuffer,0,sizeof(channels[i].ledsBuffer));
    channels[i].updated = false;
    channels[i].restartRequired = false;
    getDefinition(&(channels[i]), i);
  }
  loadTestData(channels);
  Serial.println("Finished Setting Initial channel State");
}

std::string getChannelEnabled(ChannelState* channel){
  if(channel->enabled){
    return "checked=true";
  }else{
    return "";
  }
}

std::string getChannelInterior(ChannelState* channel){
  if(channel->interior){
    return "checked=true";
  }else{
    return "";
  }
}

std::string getChannelDirectionFlipped(ChannelState* channel){
  if(channel->directionFlipped){
    return "checked=true";
  }else{
    return "";
  }
}

std::string getChannelLeftTurn(ChannelState* channel){
  if(channel->leftTurn){
    return "checked=true";
  }else{
    return "";
  }
}

std::string getChannelRightTurn(ChannelState* channel){
  if(channel->rightTurn){
    return "checked=true";
  }else{
    return "";
  }
}

std::string getChannelBrake(ChannelState* channel){
  if(channel->brake){
    return "checked=true";
  }else{
    return "";
  }
}

std::string getChannelReverse(ChannelState* channel){
  if(channel->reverse){
    return "checked=true";
  }else{
    return "";
  }
}

std::string getChannelType(ChannelState* channel){
  char buffer[10];
  return itoa(channel->type,buffer, 10);
}

std::string getChannelOrder(ChannelState* channel){
  char buffer[10];
  return itoa(channel->order,buffer, 10);
}

std::string getChannelNumLEDs(ChannelState* channel){
 char buffer[10];
  return itoa(channel->numLEDs,buffer, 10);
}
















void getDefinition(ChannelState * channel, int index){
  //Serial.println("Start Getting strip definition");
//FIRST 8: GPIO_NUM_23,GPIO_NUM_22,GPIO_NUM_21,GPIO_NUM_19,GPIO_NUM_18,GPIO_NUM_5,GPIO_NUM_17,GPIO_NUM_16
//LAST 8:  ,GPIO_NUM_4,GPIO_NUM_0,GPIO_NUM_2,GPIO_NUM_15,GPIO_NUM_13,GPIO_NUM_12,GPIO_NUM_14,GPIO_NUM_27
  switch (index){
    case 0:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, 23, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, 23, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 23, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 1:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, 22, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, 22, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 22, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 2:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, 21, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, 21, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 3, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 3:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, 19, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, 19, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 21, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 4:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, 18, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, 18, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 19, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 5:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, 5, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, 5, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 18, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 6:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, 17, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, 17, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 5, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 7:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, 16, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, 16, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 4, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 8:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, 4, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, 4, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 0, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 9:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, 0, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, 0, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 2, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 10:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, 2, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, 2, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 15, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 11:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, 15, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, 15, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 25, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 12:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, 13, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, 13, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 26, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 13:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, 12, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, 12, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 14, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 14:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, 14, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, 14, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 12, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 15:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, 27, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, 27, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 13, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
  }
  //Serial.println("Finnished Getting strip definition");*/
}

void loadTestData(ChannelState channels[MAX_CHANNELS]){
  Serial.println("Start Settting DEFAULT Channel Data");
  channels[0].enabled = true; 
  channels[0].interior = false;
  channels[0].directionFlipped = true;
  channels[0].leftTurn = false;
  channels[0].rightTurn = false;
  channels[0].brake = false;
  channels[0].reverse = false;
  channels[0].type = 1;
  channels[0].order = 2;
  channels[0].numLEDs = 6;
  memset(channels[0].leds,0,sizeof(channels[0].leds));
  memset(channels[0].ledsBuffer,0,sizeof(channels[0].ledsBuffer));
  channels[0].updated = false;
  channels[0].restartRequired = false;
  getDefinition(&(channels[0]), 0);

  channels[1].enabled = true; 
  channels[1].interior = false;
  channels[1].directionFlipped = true;
  channels[1].leftTurn = false;
  channels[1].rightTurn = false;
  channels[1].brake = false;
  channels[1].reverse = false;
  channels[1].type = 1;
  channels[1].order = 2;
  channels[1].numLEDs = 6;
  memset(channels[1].leds,0,sizeof(channels[1].leds));
  memset(channels[1].ledsBuffer,0,sizeof(channels[1].ledsBuffer));
  channels[1].updated = false;
  channels[1].restartRequired = false;
  getDefinition(&(channels[1]), 1);

  channels[2].enabled = true; 
  channels[2].interior = false;
  channels[2].directionFlipped = true;
  channels[2].leftTurn = false;
  channels[2].rightTurn = false;
  channels[2].brake = false;
  channels[2].reverse = false;
  channels[2].type = 1;
  channels[2].order = 2;
  channels[2].numLEDs = 6;
  memset(channels[2].leds,0,sizeof(channels[2].leds));
  memset(channels[2].ledsBuffer,0,sizeof(channels[2].ledsBuffer));
  channels[2].updated = false;
  channels[2].restartRequired = false;
  getDefinition(&(channels[2]), 2);

  channels[3].enabled = true; 
  channels[3].interior = false;
  channels[3].directionFlipped = true;
  channels[3].leftTurn = false;
  channels[3].rightTurn = false;
  channels[3].brake = false;
  channels[3].reverse = false;
  channels[3].type = 1;
  channels[3].order = 2;
  channels[3].numLEDs = 6;
  memset(channels[3].leds,0,sizeof(channels[3].leds));
  memset(channels[3].ledsBuffer,0,sizeof(channels[3].ledsBuffer));
  channels[3].updated = false;
  channels[3].restartRequired = false;
  getDefinition(&(channels[3]), 3);

  channels[4].enabled = true; 
  channels[4].interior = false;
  channels[4].directionFlipped = true;
  channels[4].leftTurn = false;
  channels[4].rightTurn = false;
  channels[4].brake = false;
  channels[4].reverse = false;
  channels[4].type = 1;
  channels[4].order = 2;
  channels[4].numLEDs = 6;
  memset(channels[4].leds,0,sizeof(channels[4].leds));
  memset(channels[4].ledsBuffer,0,sizeof(channels[4].ledsBuffer));
  channels[4].updated = false;
  channels[4].restartRequired = false;
  getDefinition(&(channels[4]), 4);

  channels[5].enabled = true; 
  channels[5].interior = false;
  channels[5].directionFlipped = true;
  channels[5].leftTurn = false;
  channels[5].rightTurn = false;
  channels[5].brake = false;
  channels[5].reverse = false;
  channels[5].type = 1;
  channels[5].order = 2;
  channels[5].numLEDs = 6;
  memset(channels[5].leds,0,sizeof(channels[5].leds));
  memset(channels[5].ledsBuffer,0,sizeof(channels[5].ledsBuffer));
  channels[5].updated = false;
  channels[5].restartRequired = false;
  getDefinition(&(channels[5]), 5);

  channels[6].enabled = true; 
  channels[6].interior = false;
  channels[6].directionFlipped = true;
  channels[6].leftTurn = false;
  channels[6].rightTurn = false;
  channels[6].brake = false;
  channels[6].reverse = false;
  channels[6].type = 1;
  channels[6].order = 2;
  channels[6].numLEDs = 6;
  memset(channels[6].leds,0,sizeof(channels[6].leds));
  memset(channels[6].ledsBuffer,0,sizeof(channels[6].ledsBuffer));
  channels[6].updated = false;
  channels[6].restartRequired = false;
  getDefinition(&(channels[6]), 6);

  channels[7].enabled = true; 
  channels[7].interior = false;
  channels[7].directionFlipped = true;
  channels[7].leftTurn = false;
  channels[7].rightTurn = false;
  channels[7].brake = false;
  channels[7].reverse = false;
  channels[7].type = 1;
  channels[7].order = 2;
  channels[7].numLEDs = 6;
  memset(channels[7].leds,0,sizeof(channels[7].leds));
  memset(channels[7].ledsBuffer,0,sizeof(channels[7].ledsBuffer));
  channels[7].updated = false;
  channels[7].restartRequired = false;
  getDefinition(&(channels[7]), 7);

  channels[8].enabled = true; 
  channels[8].interior = false;
  channels[8].directionFlipped = true;
  channels[8].leftTurn = false;
  channels[8].rightTurn = false;
  channels[8].brake = false;
  channels[8].reverse = false;
  channels[8].type = 1;
  channels[8].order = 2;
  channels[8].numLEDs = 6;
  memset(channels[8].leds,0,sizeof(channels[8].leds));
  memset(channels[8].ledsBuffer,0,sizeof(channels[8].ledsBuffer));
  channels[8].updated = false;
  channels[8].restartRequired = false;
  getDefinition(&(channels[8]), 8);

  channels[9].enabled = true; 
  channels[9].interior = false;
  channels[9].directionFlipped = true;
  channels[9].leftTurn = false;
  channels[9].rightTurn = false;
  channels[9].brake = false;
  channels[9].reverse = false;
  channels[9].type = 1;
  channels[9].order = 2;
  channels[9].numLEDs = 6;
  memset(channels[9].leds,0,sizeof(channels[9].leds));
  memset(channels[9].ledsBuffer,0,sizeof(channels[9].ledsBuffer));
  channels[9].updated = false;
  channels[9].restartRequired = false;
  getDefinition(&(channels[9]), 9);

  channels[10].enabled = true; 
  channels[10].interior = false;
  channels[10].directionFlipped = true;
  channels[10].leftTurn = false;
  channels[10].rightTurn = false;
  channels[10].brake = false;
  channels[10].reverse = false;
  channels[10].type = 1;
  channels[10].order = 2;
  channels[10].numLEDs = 6;
  memset(channels[10].leds,0,sizeof(channels[10].leds));
  memset(channels[10].ledsBuffer,0,sizeof(channels[10].ledsBuffer));
  channels[10].updated = false;
  channels[10].restartRequired = false;
  getDefinition(&(channels[10]), 10);

  channels[11].enabled = true; 
  channels[11].interior = false;
  channels[11].directionFlipped = true;
  channels[11].leftTurn = false;
  channels[11].rightTurn = false;
  channels[11].brake = false;
  channels[11].reverse = false;
  channels[11].type = 1;
  channels[11].order = 2;
  channels[11].numLEDs = 6;
  memset(channels[11].leds,0,sizeof(channels[11].leds));
  memset(channels[11].ledsBuffer,0,sizeof(channels[11].ledsBuffer));
  channels[11].updated = false;
  channels[11].restartRequired = false;
  getDefinition(&(channels[11]), 11);

  channels[12].enabled = true; 
  channels[12].interior = false;
  channels[12].directionFlipped = true;
  channels[12].leftTurn = false;
  channels[12].rightTurn = false;
  channels[12].brake = false;
  channels[12].reverse = false;
  channels[12].type = 1;
  channels[12].order = 2;
  channels[12].numLEDs = 6;
  memset(channels[12].leds,0,sizeof(channels[12].leds));
  memset(channels[12].ledsBuffer,0,sizeof(channels[12].ledsBuffer));
  channels[12].updated = false;
  channels[12].restartRequired = false;
  getDefinition(&(channels[12]), 12);

  channels[13].enabled = true; 
  channels[13].interior = false;
  channels[13].directionFlipped = true;
  channels[13].leftTurn = false;
  channels[13].rightTurn = false;
  channels[13].brake = false;
  channels[13].reverse = false;
  channels[13].type = 1;
  channels[13].order = 2;
  channels[13].numLEDs = 6;
  memset(channels[13].leds,0,sizeof(channels[13].leds));
  memset(channels[13].ledsBuffer,0,sizeof(channels[13].ledsBuffer));
  channels[13].updated = false;
  channels[13].restartRequired = false;
  getDefinition(&(channels[13]), 13);

  channels[14].enabled = true; 
  channels[14].interior = false;
  channels[14].directionFlipped = true;
  channels[14].leftTurn = false;
  channels[14].rightTurn = false;
  channels[14].brake = false;
  channels[14].reverse = false;
  channels[14].type = 1;
  channels[14].order = 2;
  channels[14].numLEDs = 6;
  memset(channels[14].leds,0,sizeof(channels[14].leds));
  memset(channels[14].ledsBuffer,0,sizeof(channels[14].ledsBuffer));
  channels[14].updated = false;
  channels[14].restartRequired = false;
  getDefinition(&(channels[14]), 14);

  channels[15].enabled = true; 
  channels[15].interior = false;
  channels[15].directionFlipped = true;
  channels[15].leftTurn = false;
  channels[15].rightTurn = false;
  channels[15].brake = false;
  channels[15].reverse = false;
  channels[15].type = 1;
  channels[15].order = 2;
  channels[15].numLEDs = 6;
  memset(channels[15].leds,0,sizeof(channels[15].leds));
  memset(channels[15].ledsBuffer,0,sizeof(channels[15].ledsBuffer));
  channels[15].updated = false;
  channels[15].restartRequired = false;
  getDefinition(&(channels[15]), 15);

  Serial.println("Finnished Settting DEFAULT Channel Data");
}

void loadCanAmData(ChannelState channels[MAX_CHANNELS]){
  Serial.println("Start Settting DEFAULT Channel Data");
  channels[0].enabled = true; //FRONT RIGHT ROCK LIGHT
  channels[0].interior = false;
  channels[0].directionFlipped = true;
  channels[0].leftTurn = false;
  channels[0].rightTurn = true;
  channels[0].brake = false;
  channels[0].reverse = false;
  channels[0].type = 1;
  channels[0].order = 5;
  channels[0].numLEDs = 18; //18
  memset(channels[0].leds,0,sizeof(channels[0].leds));
  memset(channels[0].ledsBuffer,0,sizeof(channels[0].ledsBuffer));
  channels[0].updated = false;
  channels[0].restartRequired = false;
  getDefinition(&(channels[0]), 0);

  channels[1].enabled = true; //Right Interior Light
  channels[1].interior = true;
  channels[1].directionFlipped = true;
  channels[1].leftTurn = false;
  channels[1].rightTurn = false;
  channels[1].brake = false;
  channels[1].reverse = false;
  channels[1].type = 1;
  channels[1].order = 5;
  channels[1].numLEDs = 18;
  memset(channels[1].leds,0,sizeof(channels[1].leds));
  memset(channels[1].ledsBuffer,0,sizeof(channels[1].ledsBuffer));
  channels[1].updated = false;
  channels[1].restartRequired = false;
  getDefinition(&(channels[1]), 1);

  channels[2].enabled = true; //Right Whip
  channels[2].interior = false;
  channels[2].directionFlipped = false;
  channels[2].leftTurn = false;
  channels[2].rightTurn = true;
  channels[2].brake = false;
  channels[2].reverse = false;
  channels[2].type = 1;
  channels[2].order = 2;
  channels[2].numLEDs = 25;
  memset(channels[2].leds,0,sizeof(channels[2].leds));
  memset(channels[2].ledsBuffer,0,sizeof(channels[2].ledsBuffer));
  channels[2].updated = false;
  channels[2].restartRequired = false;
  getDefinition(&(channels[2]), 2);

  channels[3].enabled = true; //Right Rear Rock Light
  channels[3].interior = false;
  channels[3].directionFlipped = true;
  channels[3].leftTurn = false;
  channels[3].rightTurn = true;
  channels[3].brake = false;
  channels[3].reverse = false;
  channels[3].type = 1;
  channels[3].order = 5;
  channels[3].numLEDs = 18;
  memset(channels[3].leds,0,sizeof(channels[3].leds));  
  memset(channels[3].ledsBuffer,0,sizeof(channels[3].ledsBuffer));
  channels[3].updated = false;
  channels[3].restartRequired = false;
  getDefinition(&(channels[3]), 3);
 
  channels[4].enabled = true; //Left Rear Rock Light
  channels[4].interior = false;
  channels[4].directionFlipped = true;
  channels[4].leftTurn = false;
  channels[4].rightTurn = true;
  channels[4].brake = false;
  channels[4].reverse = false;
  channels[4].type = 1;
  channels[4].order = 5;
  channels[4].numLEDs = 18;
  memset(channels[4].leds,0,sizeof(channels[4].leds));
  memset(channels[4].ledsBuffer,0,sizeof(channels[4].ledsBuffer));
  channels[4].updated = false;
  channels[4].restartRequired = false;
  getDefinition(&(channels[4]), 4);
  
  channels[5].enabled = true; //Left Whip
  channels[5].interior = false;
  channels[5].directionFlipped = false;
  channels[5].leftTurn = true;
  channels[5].rightTurn = false;
  channels[5].brake = false;
  channels[5].reverse = false;
  channels[5].type = 1;
  channels[5].order = 2;
  channels[5].numLEDs = 25;
  memset(channels[5].leds,0,sizeof(channels[5].leds));
  memset(channels[5].ledsBuffer,0,sizeof(channels[5].ledsBuffer));
  channels[5].updated = false;
  channels[5].restartRequired = false;
  getDefinition(&(channels[5]), 5);

  channels[6].enabled = true; //Left Interior
  channels[6].interior = true;
  channels[6].directionFlipped = true;
  channels[6].leftTurn = false;
  channels[6].rightTurn = false;
  channels[6].brake = false;
  channels[6].reverse = false;
  channels[6].type = 1;
  channels[6].order = 5;
  channels[6].numLEDs = 18;
  memset(channels[6].leds,0,sizeof(channels[6].leds));
  memset(channels[6].ledsBuffer,0,sizeof(channels[6].ledsBuffer));
  channels[6].updated = false;
  channels[6].restartRequired = false;
  getDefinition(&(channels[6]), 6);
  
  channels[7].enabled = true; //Left Front Rock Light
  channels[7].interior = false;
  channels[7].directionFlipped = true;
  channels[7].leftTurn = true;
  channels[7].rightTurn = false;
  channels[7].brake = false;
  channels[7].reverse = false;
  channels[7].type = 1;
  channels[7].order = 5;
  channels[7].numLEDs = 18;
  memset(channels[7].leds,0,sizeof(channels[7].leds));
  memset(channels[7].ledsBuffer,0,sizeof(channels[7].ledsBuffer));
  channels[7].updated = false;
  channels[7].restartRequired = false;
  getDefinition(&(channels[7]), 7);

  Serial.println("Finnished Settting DEFAULT Channel Data");
}

