#include "webserver.h"

void indexHandler(AsyncWebServerRequest * request){
    File file = SPIFFS.open("/index.htm");
    if(!file){
        Serial.println("File not found");
    }
    String html;
    while(file.available()){
        html += char(file.read());
    }
    file.close();

    html.replace("%REPLACE_SWITCH_LEFTTURN%", getLeftTurn(&state.switches).c_str());
    html.replace("%REPLACE_SWITCH_RIGHTTURN%", getRightTurn(&state.switches).c_str());
    html.replace("%REPLACE_SWITCH_REVERSE%", getReverse(&state.switches).c_str());
    html.replace("%REPLACE_SWITCH_BRAKE%", getBrake(&state.switches).c_str());
    html.replace("%REPLACE_SWITCH_INTERIOR%", getInterior(&state.switches).c_str());
    html.replace("%REPLACE_SWITCH_UIOVERRIDE%", getUIOverride(&state.switches).c_str());

    html.replace("%REPLACE_ANIMATION_INDEX%", getAnimationIndex(&state.animation).c_str());
    html.replace("%REPLACE_ANIMATION_STEPSIZE%", getStepSize(&state.animation).c_str());
    html.replace("%REPLACE_ANIMATION_COLORNUM%", getNumberOfColors(&state.animation).c_str());
    html.replace("%REPLACE_ANIMATION_COLORDATA%", getColorData(&state.animation).c_str());

    html.replace("%REPLACE_CHANNEL1_ENABLED%", getChannelEnabled(&state.channels[0]).c_str());
    html.replace("%REPLACE_CHANNEL1_INTERIOR%", getChannelInterior(&state.channels[0]).c_str());
    html.replace("%REPLACE_CHANNEL1_DIRECTIONFLIPPED%", getChannelDirectionFlipped(&state.channels[0]).c_str());
    html.replace("%REPLACE_CHANNEL1_LEFTTURN%", getChannelLeftTurn(&state.channels[0]).c_str());
    html.replace("%REPLACE_CHANNEL1_RIGHTTURN%", getChannelRightTurn(&state.channels[0]).c_str());
    html.replace("%REPLACE_CHANNEL1_BRAKE%", getChannelBrake(&state.channels[0]).c_str());
    html.replace("%REPLACE_CHANNEL1_REVERSE%", getChannelReverse(&state.channels[0]).c_str());
    html.replace("%REPLACE_CHANNEL1_TYPE%", getChannelType(&state.channels[0]).c_str());
    html.replace("%REPLACE_CHANNEL1_ORDER%", getChannelOrder(&state.channels[0]).c_str());
    html.replace("%REPLACE_CHANNEL1_NUMLEDS%", getChannelNumLEDs(&state.channels[0]).c_str());

    Serial.println("Sending Response");
    Serial.println(html);
    request->send(200, "text/html", html.c_str());
}