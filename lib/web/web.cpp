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

std::string getChannelPosHTML(int index){
  char buffer[10];
  std::string stringIndex = itoa(index, buffer, 10);

  std::string returnValue = "<H2>Channel "+stringIndex+" Positions</H2>"
                            "<form action=\"/channelsPos\" method=\"post\">"
                            "<input type=\"hidden\" name=\"CHANNEL_INDEX\" value=\""+stringIndex+"\" />"
                            "<input type=\"button\" value=\"Back\" onclick=\"history.back()\"><br>";

  for(int i=0; i<globalState->channels[index].numLEDs; i++){
    std::string stringPos = itoa(i, buffer, 10);
    std::string stringRow;
    std::string stringCol;
    getChannelLocationAtPosition(&globalState->location, index, i, &stringRow, &stringCol);

    returnValue += "<label for=\"LOC_"+stringPos+"_ROW\">"+stringPos+" Position: </label>"
        "<input type=\"number\" id=\"LOC_"+stringPos+"_ROW\" name=\"LOC_"+stringPos+"_ROW\" value="+stringRow+" min=\"0\" max=\"63\">"
        "<input type=\"number\" id=\"LOC_"+stringPos+"_COL\" name=\"LOC_"+stringPos+"_COL\" value="+stringCol+" min=\"0\" max=\"63\"><br>";
  }

  returnValue += "<input type=\"submit\" value=\"Update\">";
  return returnValue;
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
      if(request->hasParam("blending",true)){
        globalState->animation.blending = LINEARBLEND;
      }else{
        globalState->animation.blending = NOBLEND;
      }
      if(request->hasParam("stepSize",true)){
        AsyncWebParameter* p = request->getParam("stepSize", true);
        globalState->animation.stepSize = atoi(p->value().c_str());
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
        Serial.println("Building HTML");
        AsyncWebParameter* p = request->getParam("CHANNEL_INDEX", true);
        int channelIndex = atoi(p->value().c_str());

        std::string headerHtml = "<!DOCTYPE html>" 
                            "<html>"
                            "<head>"
                            "</head>"
                            "<body>"
                            "<div>";

        std::string footerHtml = "</div>"
                                "</body>"
                                "</html>";

        std::string updateHtml = getChannelPosHTML(channelIndex);

        std::string fullHtml = headerHtml + updateHtml + footerHtml;

        request->send(200, "text/html", fullHtml.c_str());
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
          globalState->channels[channelIndex].enabled = true;
        }else{
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
          char newType = atoi(p->value().c_str());
          if(newType != globalState->channels[channelIndex].type){
            globalState->channels[channelIndex].type = 
            globalState->channels[channelIndex].restartRequired = true;
          }
        }

        if(request->hasParam(prefix+"order",true)){
          AsyncWebParameter* p = request->getParam(prefix+"order", true);
          char newOrder = atoi(p->value().c_str());
          if(newOrder != globalState->channels[channelIndex].order){
            globalState->channels[channelIndex].order = atoi(p->value().c_str());
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
  if(var == "REPLACE_ANIMATION_INDEX"){
    return getAnimationIndex(&globalState->animation).c_str();
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
    if( var == prefix+"TYPE"){
      return getChannelType(&globalState->channels[i]).c_str();
    }
    if( var == prefix+"ORDER"){
      return getChannelOrder(&globalState->channels[i]).c_str();
    }
    if( var == prefix+"NUMLEDS"){
      return getChannelNumLEDs(&globalState->channels[i]).c_str();
    }
  }
  return String();
}

