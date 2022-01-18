#include "signals.h"

bool delayAndPollForUpdate(State* state, int delay){
  unsigned long time = millis();
  while(millis() < time + delay){
    if(isNewBLEValue(&state->bluetooth) == true || updateSwitchState(&state->switches) == true){
      return true;
    }
    vTaskDelay(10);
  }
  return false;
}

void signalTask(void *pvParameters){
  Serial.println("Signal Task Started");
  State* state = (State*)pvParameters;
  for(;;){//following comments assume all switches are on
    if(state->switches.brake || state->switches.reverse || state->switches.leftTurn || state->switches.rightTurn){
      Serial.println("Start Signals");
      //lowest priority goes first since any overlaps will be over written with higher priorities
      if(state->switches.reverse){
        reverseAnimation(state->channels, &state->switches, &state->bluetooth);
      }

      //brake light is next highest importance
      if(state->switches.brake){
        brakeAnimation(state->channels, &state->switches, &state->bluetooth);
      }
      
      //turn signals are highest priority because they are intermittent and will default back to other signal if applicable
      if(state->switches.leftTurn){
        leftTurnAnimationWithColor(state->channels, &state->switches, &state->bluetooth, CRGB::Orange); 
      }

      if(state->switches.rightTurn){
        rightTurnAnimationWithColor(state->channels, &state->switches, &state->bluetooth, CRGB::Orange);
      }

      //display lights with priority maintained
      showAll(state->channels);
      if(delayAndPollForUpdate(state, 500) == true){
        continue; //break out early if we get an update
      }

      //Set turn signals back to off
      if(state->switches.leftTurn){
        leftTurnAnimationWithColor(state->channels, &state->switches, &state->bluetooth, CRGB::Black); 
      }

      if(state->switches.rightTurn){
        rightTurnAnimationWithColor(state->channels, &state->switches, &state->bluetooth, CRGB::Black);
      }
      //turn any turn signal back to to reverse color if they overlapped
      if(state->switches.reverse){
        reverseAnimation(state->channels, &state->switches, &state->bluetooth);
      }
      //finally turn back on any overlapping breaklights
      if(state->switches.brake){
        brakeAnimation(state->channels, &state->switches, &state->bluetooth);
      }
      //Show the final prioritized state of the lights
      showAll(state->channels);
      if(delayAndPollForUpdate(state, 500) == true){
        continue; //break out early if we get an update
      }
      Serial.println("End Signals");
    }else{
      delayAndPollForUpdate(state, 500);
    }
  }
}

void createSignals(State* state, TaskHandle_t* handle){
    xTaskCreatePinnedToCore(signalTask, "signalTask", 2048, (void*)state, 4, handle, SIGNAL_CORE);
}