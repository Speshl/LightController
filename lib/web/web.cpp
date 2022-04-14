#include "web.h"

void saveState2(State* currentState){
  Serial.println("Saving State");
  saveSwitchState(&currentState->preferences, getStateAsString(&currentState->switches));
  saveAnimationState(&currentState->preferences, getStateAsString(&currentState->animation));
  for(int i=0; i<MAX_CHANNELS; i++){
    saveChannelDetailState(&currentState->preferences, i, getStateAsString(currentState->channels, i));
    std::string data = getChannelLocations(&currentState->location, i, currentState->channels[i].numLEDs);
    saveChannelLocationState(&currentState->preferences, i, data);
  }
  Serial.println("State Saved");
}

std::string getChannelPage(int index){
    if(index >= 0 && index <MAX_CHANNELS){
        char buffer[10];
        String stringNumber = itoa(index,buffer,10);
        std::string returnValue = "/channel";
        returnValue += stringNumber.c_str();
        returnValue += ".htm";
        return returnValue;
    }
    return "./channel0.htm";
}

void initializeAP(State* state){
    globalState = state;
    Serial.println("Starting AP...");
    WiFi.onEvent(OnWiFiEvent);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASS);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("SOFT AP IP: ");
    Serial.println(IP.toString());


    server.on("/", HTTP_GET, [](AsyncWebServerRequest * request){
        request->send(SPIFFS, "/animation.htm", String(), false, animationProcessor);
    });
    server.on("/misc", HTTP_GET, [](AsyncWebServerRequest * request){
        request->send(SPIFFS, "/misc.htm", String(), false, miscProcessor);
    });
    server.on("/animation", HTTP_GET, [](AsyncWebServerRequest * request){
        request->send(SPIFFS, "/animation.htm", String(), false, animationProcessor);
    });
    server.on("/save", HTTP_POST, [](AsyncWebServerRequest * request){
        saveState2(globalState);
        request->send(SPIFFS, "/misc.htm", String(), false, miscProcessor);
    });
    server.on("/defaultLocations", HTTP_POST, [](AsyncWebServerRequest * request){
        
        removeAllEntries(&globalState->location);
        setInitialState(&globalState->location);
        saveState2(globalState); //save so that it gets reloaded next time
        request->send(SPIFFS, "/misc.htm", String(), false, miscProcessor);
    });
    server.on("/switches", HTTP_POST, [](AsyncWebServerRequest * request){
        if(request->hasParam("leftTurn",true)){
          Serial.println("LEFT TURN FOUND, TURNING ON");
          globalState->switches.leftTurn = true;
        }else{
          Serial.println("LEFT TURN NOT FOUND, OFF");
          globalState->switches.leftTurn = false;
        }

        if(request->hasParam("rightTurn",true)){
          globalState->switches.rightTurn = true;
        }else{
          globalState->switches.rightTurn = false;
        }

        if(request->hasParam("Reverse",true)){
          globalState->switches.reverse = true;
        }else{
          globalState->switches.reverse = false;
        }

        if(request->hasParam("Brake",true)){
          globalState->switches.brake = true;
        }else{
          globalState->switches.brake = false;
        }

        if(request->hasParam("Interior",true)){
          globalState->switches.interior = true;
        }else{
          globalState->switches.interior = false;
        }

        if(request->hasParam("UIOverride",true)){
          globalState->switches.uiOverride = true;
        }else{
          globalState->switches.uiOverride = false;
        }
        //describeState(&globalState->switches);
        saveSwitchState(&globalState->preferences, getStateAsString(&globalState->switches));
        
        request->send(SPIFFS, "/misc.htm", String(), false, miscProcessor);
    });

    server.on("/animation", HTTP_POST, [](AsyncWebServerRequest * request){
      if(request->hasParam("animation",true)){
        AsyncWebParameter* p = request->getParam("animation", true);
        globalState->animation.animation = atoi(p->value().c_str());
      }

      if(request->hasParam("fps",true)){
        AsyncWebParameter* p = request->getParam("fps", true);
        globalState->animation.fps = atoi(p->value().c_str());
      }

      if(request->hasParam("brightness",true)){
        AsyncWebParameter* p = request->getParam("brightness", true);
        globalState->animation.brightness = atoi(p->value().c_str());
      }

      if(request->hasParam("blending",true)){
        globalState->animation.blending = LINEARBLEND;
      }else{
        globalState->animation.blending = NOBLEND;
      }

      if(request->hasParam("stepSize",true)){
        AsyncWebParameter* p = request->getParam("stepSize", true);
        globalState->animation.stepSize = atoi(p->value().c_str());
      }

      if(request->hasParam("colorPalette",true)){
        AsyncWebParameter* p = request->getParam("colorPalette", true);
        globalState->animation.palettePreset = atoi(p->value().c_str());
      }


      for(int i=0; i<MAX_COLORS; i++){
        char buffer[10];
        String stringNumber = itoa(i,buffer,10);
        String prefix = "color"+stringNumber;
        if(request->hasParam(prefix+"Pos", true)){
          AsyncWebParameter* p = request->getParam(prefix+"Pos", true);
          setColorPos(&globalState->animation, i, p->value().c_str());
        }
        if(request->hasParam(prefix, true)){
          AsyncWebParameter* p = request->getParam(prefix, true);
          setColorHex(&globalState->animation, i, p->value().c_str());
        }
      }
      animationUpdated(&globalState->animation, globalState->channels);
      saveAnimationState(&globalState->preferences, getStateAsString(&globalState->animation));
      setStateFromString(&globalState->animation, getStateAsString(&globalState->animation));
      request->send(SPIFFS, "/animation.htm", String(), false, animationProcessor);
    });

    server.on("/channelsPos", HTTP_POST, [](AsyncWebServerRequest * request){
      if(request->hasParam("INITIAL",true) == false){ //Read parameters if this isn't the initial load of the page
        if(request->hasParam("CHANNEL_INDEX",true)){
          AsyncWebParameter* p = request->getParam("CHANNEL_INDEX", true);
          int channelIndex = atoi(p->value().c_str());
          removeChannelEntries(&globalState->location, channelIndex);
          for(int i=0;i<globalState->channels[channelIndex].numLEDs; i++){
            char buffer[10];
            std::string posString = itoa(i, buffer, 10);
            std::string locRow = "LOC_";
            std::string locCol = "LOC_";
            locRow += posString.c_str();
            locCol += posString.c_str();
            locRow += "_ROW";
            locCol += "_COL";

            AsyncWebParameter* rowParam = request->getParam(locRow.c_str(), true);
            AsyncWebParameter* colParam = request->getParam(locCol.c_str(), true);

            int row = atoi(rowParam->value().c_str());
            int col = atoi(colParam->value().c_str());
            addEntry(&globalState->location,row, col, channelIndex, i);
          }
          std::string data = getChannelLocations(&globalState->location, channelIndex, globalState->channels[channelIndex].numLEDs);
          saveChannelLocationState(&globalState->preferences, channelIndex, data);
        }else{
          request->send(SPIFFS, "/animation.htm", String(), false, animationProcessor);
        }
      }
      //Build page to display
      if(request->hasParam("CHANNEL_INDEX",true)){
        AsyncWebParameter* p = request->getParam("CHANNEL_INDEX", true);
        int channelIndex = atoi(p->value().c_str());
        request->send(SPIFFS, "/channelLoc.htm", String(), false, channelLocProcessor(channelIndex));
      }else{
        request->send(SPIFFS, "/animation.htm", String(), false, animationProcessor);
      }
    });

    server.on("/channels", HTTP_POST, [](AsyncWebServerRequest * request){
      if(request->hasParam("INITIAL",true)){
        if(request->hasParam("CHANNEL_INDEX",true)){
          AsyncWebParameter* p = request->getParam("CHANNEL_INDEX", true);
          int channelIndex = atoi(p->value().c_str());
          request->send(SPIFFS, getChannelPage(channelIndex).c_str(), String(), false, channelProcessor);
        }else{
          request->send(SPIFFS, "/animation.htm", String(), false, animationProcessor);
        }
      }else if(request->hasParam("CHANNEL_INDEX",true)){
        AsyncWebParameter* p = request->getParam("CHANNEL_INDEX", true);
        int channelIndex = atoi(p->value().c_str());
        String prefix = "CHANNEL_"+p->value();
        prefix += "_";
        if(request->hasParam(prefix+"enabled",true)){
          Serial.println("Enabling Channel");
          globalState->channels[channelIndex].enabled = true;
        }else{
          Serial.println("Disabling Channel");
          globalState->channels[channelIndex].enabled = false;
        }

        if(request->hasParam(prefix+"interior",true)){
          globalState->channels[channelIndex].interior = true;
        }else{
          globalState->channels[channelIndex].interior = false;
        }

        if(request->hasParam(prefix+"directionFlipped",true)){
          globalState->channels[channelIndex].directionFlipped = true;
        }else{
          globalState->channels[channelIndex].directionFlipped = false;
        }

        if(request->hasParam(prefix+"leftTurn",true)){
          globalState->channels[channelIndex].leftTurn = true;
        }else{
          globalState->channels[channelIndex].leftTurn = false;
        }

        if(request->hasParam(prefix+"rightTurn",true)){
          globalState->channels[channelIndex].rightTurn = true;
        }else{
          globalState->channels[channelIndex].rightTurn = false;
        }

        if(request->hasParam(prefix+"brake",true)){
          globalState->channels[channelIndex].brake = true;
        }else{
          globalState->channels[channelIndex].brake = false;
        }

        if(request->hasParam(prefix+"reverse",true)){
          globalState->channels[channelIndex].reverse = true;
        }else{
          globalState->channels[channelIndex].reverse = false;
        }

        if(request->hasParam(prefix+"type",true)){
          AsyncWebParameter* p = request->getParam(prefix+"type", true);
          int newType = atoi(p->value().c_str());
          if(newType != globalState->channels[channelIndex].type){
            globalState->channels[channelIndex].type = newType;
            globalState->channels[channelIndex].restartRequired = true;
          }
        }

        if(request->hasParam(prefix+"order",true)){
          AsyncWebParameter* p = request->getParam(prefix+"order", true);
          int newOrder = atoi(p->value().c_str());
          if(newOrder != globalState->channels[channelIndex].order){
            globalState->channels[channelIndex].order = newOrder;
            //globalState->channels[channelIndex].restartRequired = true;
          }
        }

        if(request->hasParam(prefix+"numLEDs",true)){
          AsyncWebParameter* p = request->getParam(prefix+"numLEDs", true);
          globalState->channels[channelIndex].numLEDs = atoi(p->value().c_str());
        }

        saveChannelDetailState(&globalState->preferences, channelIndex, getStateAsString(globalState->channels, channelIndex));
        request->send(SPIFFS, getChannelPage(channelIndex).c_str(), String(), false, channelProcessor);
      }else{
        Serial.println("Channel Index not found, showing index");
        request->send(SPIFFS, "/animation.htm", String(), false, animationProcessor);
      }
    });
    server.begin();
}

int getClientCount(){
    return clientsConnected;
}

void OnWiFiEvent(WiFiEvent_t event){
    switch(event){
        case SYSTEM_EVENT_STA_CONNECTED:
            Serial.println("ESP32 Connected to WiFi Network");
            break;
        case SYSTEM_EVENT_AP_START:
            Serial.println("ESP32 Soft AP Started");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            Serial.println("Station connected to ESP32 Soft AP");
            clientsConnected++;
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            Serial.println("Station disconnected from ESP32 Soft AP");
            clientsConnected--;
            break;
        default: break;
    }
}

String animationProcessor(const String& var){
  if(var == "SOLID_ANIMATION_SELECTED"){
    if(globalState->animation.animation == 0){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "SWEEPUP_ANIMATION_SELECTED"){
    if(globalState->animation.animation == 1){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "SWEEPDOWN_ANIMATION_SELECTED"){
    if(globalState->animation.animation == 2){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "SWEEPRIGHT_ANIMATION_SELECTED"){
    if(globalState->animation.animation == 3){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "SWEEPLEFT_ANIMATION_SELECTED"){
    if(globalState->animation.animation == 4){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "SPINCLOCK_ANIMATION_SELECTED"){
    if(globalState->animation.animation == 5){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "REPLACE_ANIMATION_FPS"){
    return getFPS(&globalState->animation).c_str();
  }
  if(var == "REPLACE_ANIMATION_BRIGHTNESS"){
    return getBrightness(&globalState->animation).c_str();
  }
  if(var == "REPLACE_ANIMATION_BLEND"){
    return getBlending(&globalState->animation).c_str();
  }
  if(var == "REPLACE_ANIMATION_STEPSIZE"){
    return getStepSize(&globalState->animation).c_str();
  }
  if(var == "CUSTOM_PALETTE_SELECTED"){
    if(globalState->animation.palettePreset == 0){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "RAINBOW_PALETTE_SELECTED"){
    if(globalState->animation.palettePreset == 1){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "RAINBOWSTRIPE_PALETTE_SELECTED"){
    if(globalState->animation.palettePreset == 2){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "PARTY_PALETTE_SELECTED"){
    if(globalState->animation.palettePreset == 3){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "LAVA_PALETTE_SELECTED"){
    if(globalState->animation.palettePreset == 4){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "HEAT_PALETTE_SELECTED"){
    if(globalState->animation.palettePreset == 5){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "CLOUD_PALETTE_SELECTED"){
    if(globalState->animation.palettePreset == 6){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "OCEAN_PALETTE_SELECTED"){
    if(globalState->animation.palettePreset == 7){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "FOREST_PALETTE_SELECTED"){
    if(globalState->animation.palettePreset == 8){
        return "selected";
    }else{
        return "";
    }
  }

  for(int i=0; i<MAX_COLORS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "COLOR_"+stringNumber;
    prefix += "_";
    if(var == prefix+"POS"){
        return getColorPos(&globalState->animation, i).c_str();
    }
    if(var == prefix+"HEX"){
      return getColorHex(&globalState->animation, i).c_str();
    }
  }
  return String();
}

String miscProcessor(const String& var){
  if(var == "LOCATION_MAP"){
    return getLocationGrid(&globalState->location).c_str();
  }
  if(var == "REPLACE_SWITCH_LEFTTURN"){
    return getLeftTurn(&globalState->switches).c_str();
  }
  if(var == "REPLACE_SWITCH_RIGHTTURN"){
    return getRightTurn(&globalState->switches).c_str();
  }
  if(var == "REPLACE_SWITCH_REVERSE"){
    return getReverse(&globalState->switches).c_str();
  }
  if(var == "REPLACE_SWITCH_BRAKE"){
    return getBrake(&globalState->switches).c_str();
  }
  if(var == "REPLACE_SWITCH_INTERIOR"){
    return getInterior(&globalState->switches).c_str();
  }
  if(var == "REPLACE_SWITCH_UIOVERRIDE"){
    return getUIOverride(&globalState->switches).c_str();
  }
  return String();
}

String channelProcessor(const String& var){
  for(int i=0; i<MAX_CHANNELS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "CHANNEL_"+stringNumber;
    prefix += "_";
    if( var == prefix+"ENABLED"){
      return getChannelEnabled(&globalState->channels[i]).c_str();
    }
    if( var == prefix+"INTERIOR"){
      return getChannelInterior(&globalState->channels[i]).c_str();
    }
    if( var == prefix+"DIRECTIONFLIPPED"){
      return getChannelDirectionFlipped(&globalState->channels[i]).c_str();
    }
    if( var == prefix+"LEFTTURN"){
      return getChannelLeftTurn(&globalState->channels[i]).c_str();
    }
    if( var == prefix+"RIGHTTURN"){
      return getChannelRightTurn(&globalState->channels[i]).c_str();
    }
    if( var == prefix+"BRAKE"){
      return getChannelBrake(&globalState->channels[i]).c_str();
    }
    if( var == prefix+"REVERSE"){
      return getChannelReverse(&globalState->channels[i]).c_str();
    }

    if( var == "TYPE0_"+prefix+"SELECTED"){
      if(globalState->channels[i].type == 0){
        return "selected";
      }
      else{
        return "";
      }
    }

    if( var == "TYPE1_"+prefix+"SELECTED"){
      if(globalState->channels[i].type == 1){
        return "selected";
      }
      else{
        return "";
      }
    }

    if( var == "RGB_"+prefix+"SELECTED"){
      if(globalState->channels[i].order == 0){
        return "selected";
      }
      else{
        return "";
      }
    }

    if( var == "RBG_"+prefix+"SELECTED"){
      if(globalState->channels[i].order == 1){
        return "selected";
      }
      else{
        return "";
      }
    }

    if( var == "BRG_"+prefix+"SELECTED"){
      if(globalState->channels[i].order == 2){
        return "selected";
      }
      else{
        return "";
      }
    }

    if( var == "BGR_"+prefix+"SELECTED"){
      if(globalState->channels[i].order == 3){
        return "selected";
      }
      else{
        return "";
      }
    }

    if( var == "GBR_"+prefix+"SELECTED"){
      if(globalState->channels[i].order == 4){
        return "selected";
      }
      else{
        return "";
      }
    }

    if( var == "GRB_"+prefix+"SELECTED"){
      if(globalState->channels[i].order == 5){
        return "selected";
      }
      else{
        return "";
      }
    }

    if( var == prefix+"NUMLEDS"){
      return getChannelNumLEDs(&globalState->channels[i]).c_str();
    }
  }
  return String();
}

String channel0LocProcessor(const String& var){
  int channelIndex = 0;

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "class=\"pos-grid\"";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringCol.c_str();
    }
  }
  return "hidden";
}

String channel1LocProcessor(const String& var){
  int channelIndex = 1;

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringCol.c_str();
    }
  }
  return "hidden";
}

String channel2LocProcessor(const String& var){
  int channelIndex = 2;

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringCol.c_str();
    }
  }
  return "hidden";
}

String channel3LocProcessor(const String& var){
  int channelIndex = 3;

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringCol.c_str();
    }
  }
  return "hidden";
}

String channel4LocProcessor(const String& var){
  int channelIndex = 4;

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringCol.c_str();
    }
  }
  return "hidden";
}

String channel5LocProcessor(const String& var){
  int channelIndex = 5;

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringCol.c_str();
    }
  }
  return "hidden";
}

String channel6LocProcessor(const String& var){
  int channelIndex = 6;

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringCol.c_str();
    }
  }
  return "hidden";
}

String channel7LocProcessor(const String& var){
  int channelIndex = 7;

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringCol.c_str();
    }
  }
  return "hidden";
}

String channel8LocProcessor(const String& var){
  int channelIndex = 8;

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringCol.c_str();
    }
  }
  return "hidden";
}

String channel9LocProcessor(const String& var){
  int channelIndex = 9;

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringCol.c_str();
    }
  }
  return "hidden";
}

String channel10LocProcessor(const String& var){
  int channelIndex = 10;

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringCol.c_str();
    }
  }
  return "hidden";
}

String channel11LocProcessor(const String& var){
  int channelIndex = 11;

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringCol.c_str();
    }
  }
  return "hidden";
}

String channel12LocProcessor(const String& var){
  int channelIndex = 12;

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringCol.c_str();
    }
  }
  return "hidden";
}

String channel13LocProcessor(const String& var){
  int channelIndex = 13;

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringCol.c_str();
    }
  }
  return "hidden";
}

String channel14LocProcessor(const String& var){
  int channelIndex = 14;

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringCol.c_str();
    }
  }
  return "hidden";
}

String channel15LocProcessor(const String& var){
  int channelIndex = 15;

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol);
      return stringCol.c_str();
    }
  }
  return "hidden";
}

AwsTemplateProcessor channelLocProcessor(int channelIndex){
  switch(channelIndex){
    case 0:
      return channel0LocProcessor;
    case 1:
      return channel1LocProcessor;
    case 2:
      return channel2LocProcessor;
    case 3:
      return channel3LocProcessor;
    case 4:
      return channel4LocProcessor;
    case 5:
      return channel5LocProcessor;
    case 6:
      return channel6LocProcessor;
    case 7:
      return channel7LocProcessor;
    case 8:
      return channel8LocProcessor;
    case 9:
      return channel9LocProcessor;
    case 10:
      return channel10LocProcessor;
    case 11:
      return channel11LocProcessor;
    case 12:
      return channel12LocProcessor;
    case 13:
      return channel13LocProcessor;
    case 14:
      return channel14LocProcessor;
    case 15:
      return channel15LocProcessor;
    default:
      return channel0LocProcessor;
  }
}
