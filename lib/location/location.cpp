#include "location.h"

void setInitialState(LocationState* state){
  Serial.println("Start Setting Initial Location State");
  state->totalLocations = 0;
  loadTestData(state);
  Serial.println("Finished Setting Initial Location State");
}

int compareEntriesByRow(LocationEntry* entry1, LocationEntry* entry2){
  if(entry1->row > entry2->row){
    return 1;
  }else if(entry1->row < entry2->row) {
    return -1;
  }else{ //same row so check columns
    if(entry1->col > entry2->col){
      return 1;
    }else if(entry1->col < entry2->col){
      return -1;
    }
    else{
      return 0; //Identical
    }
  }
}

int compareEntriesByCol(LocationEntry* entry1, LocationEntry* entry2){
  if(entry1->col > entry2->col){
    return 1;
  }else if(entry1->col < entry2->col) {
    return -1;
  }else{ //same row so check columns
    if(entry1->row > entry2->row){
      return 1;
    }else if(entry1->row < entry2->row){
      return -1;
    }
    else{
      return 0; //Identical
    }
  }
}

int compareToEntryByRow(LocationEntry* entry, uint8_t row, uint8_t col){
  if(entry->row > row){
    return -1;
  }else if(entry->row < row) {
    return 1;
  }else{ //same row so check columns
    if(entry->col > col){
      return -1;
    }else if(entry->col < col){
      return 1;
    }
    else{
      return 0; //Identical
    }
  }
}

int compareToEntryByCol(LocationEntry* entry, uint8_t row, uint8_t col){
  if(entry->col > col){
    return -1;
  }else if(entry->col < col) {
    return 1;
  }else{ //same row so check columns
    if(entry->row > row){
      return -1;
    }else if(entry->row < row){
      return 1;
    }
    else{
      return 0; //Identical
    }
  }
}

void updateRowIndexes(LocationState* state, LocationEntry* entry){
  if (state->rowIndexes[entry->row] == NULL){
    state->rowIndexes[entry->row] = entry;
  }else{
    if(compareEntriesByRow(state->rowIndexes[entry->row], entry) == 1){
      state->rowIndexes[entry->row] = entry;
    }
  }
}

void updateColIndexes(LocationState* state, LocationEntry* entry){
  if (state->colIndexes[entry->col] == NULL){
    state->colIndexes[entry->col] = entry;
  }else{
    if(compareEntriesByCol(state->colIndexes[entry->col], entry) == 1){
      state->rowIndexes[entry->col] = entry;
    }
  }
}

void rebuildColIndexes(LocationState* state){
  //Serial.println("Rebuild Col Indexes");
  for(int i=0; i<LOCATION_GRID_SIZE; i++){
    state->colIndexes[i] = NULL;
  }
  
  LocationEntry* currentLocation = state->firstColLocation;
  while(currentLocation != NULL){
    updateColIndexes(state, currentLocation);
    currentLocation = currentLocation->nextCol;
  }
}

void rebuildRowIndexes(LocationState* state){
  //Serial.println("Rebuild Row Indexes");
  for(int i=0; i<LOCATION_GRID_SIZE; i++){
    state->rowIndexes[i] = NULL;
  }
  
  LocationEntry* currentLocation = state->firstRowLocation;
  while(currentLocation != NULL){
    updateRowIndexes(state, currentLocation);
    currentLocation = currentLocation->nextRow;
  }
}

void rebuildIndexes(LocationState* state){
  rebuildColIndexes(state);
  rebuildRowIndexes(state);
}

void addEntryByRow(LocationState* state, LocationEntry* entry){
  LocationEntry * currentLocation = state->firstRowLocation;
  LocationEntry * previousLocation = NULL;
  bool added = false;
  while(currentLocation != NULL){
    if(compareToEntryByRow(currentLocation, entry->row, entry->col) == -1){//check if new entry should go before currentLocation
      if(previousLocation == NULL){ //If NULL then no previous location, goes to front of list
        state->firstRowLocation = entry;//goes to front of list
      }else{
        previousLocation->nextRow = entry;//links to previous node
      }
      entry->nextRow = currentLocation; //new entry points to currentLocation
      added = true;
      break;
    }
    previousLocation = currentLocation;
    currentLocation = currentLocation->nextRow; //Increment currentLocation for next iteration of loop
  }

  if(state->firstRowLocation == NULL){ //First entry so set start point
    state->firstRowLocation = entry;
    added = true;
  }
  if(state->lastRowLocation == NULL){ //Most likely first entry, so set initial lastCol
    state->lastRowLocation = entry;
    added = true;
  }
  if(added == false){ //Was not added internally to linked list, so add to the end
    state->lastRowLocation->nextRow = entry;
    state->lastRowLocation = entry;
  }
}

void addEntryByCol(LocationState* state, LocationEntry* entry){
  LocationEntry * currentLocation = state->firstColLocation;
  LocationEntry * previousLocation = NULL;
  bool added = false;
  while(currentLocation != NULL){
    if(compareToEntryByCol(currentLocation, entry->row, entry->col) == -1){//check if new entry should go before currentLocation
      if(previousLocation == NULL){ //If NULL then no previous location, goes to front of list
        state->firstColLocation = entry;//goes to front of list
      }else{
        previousLocation->nextCol = entry;//links to previous node
      }
      entry->nextCol = currentLocation; //new entry points to currentLocation
      added = true;
      break;
    }
    previousLocation = currentLocation;
    currentLocation = currentLocation->nextCol; //Increment currentLocation for next iteration of loop
  }

  if(state->firstColLocation == NULL){ //First entry so set start point
    state->firstColLocation = entry;
    added = true;
  }
  if(state->lastColLocation == NULL){ //Most likely first entry, so set initial lastCol
    state->lastColLocation = entry;
    added = true;
  }
  if(added == false){ //Was not added internally to linked list, so add to the end
    state->lastColLocation->nextCol = entry;
    state->lastColLocation = entry;
  }
}

void addEntry(LocationState* state, uint8_t row, uint8_t col, uint8_t channel, uint8_t pos){
  LocationEntry * entry = (LocationEntry *) malloc(sizeof(LocationEntry));
  entry->row = row;
  entry->col = col;
  entry->channel = channel;
  entry->pos = pos;
  entry->nextRow = NULL;
  entry->nextCol = NULL;
  if(row < LOCATION_GRID_SIZE && col < LOCATION_GRID_SIZE){
    addEntryByRow(state, entry);
    addEntryByCol(state, entry);
    state->totalLocations++;
  }
}

void removeAllEntries(LocationState* state){
  Serial.println("Removing All Entries");
  LocationEntry * currentLocation = state->firstRowLocation;
  while(currentLocation != NULL){
    LocationEntry * toRemove = currentLocation;
    currentLocation = currentLocation->nextRow;
    free(currentLocation);
  }
  state->totalLocations = 0;
  state->firstRowLocation = NULL;
  state->firstColLocation = NULL;
  state->lastRowLocation = NULL;
  state->lastColLocation = NULL;
  rebuildIndexes(state);
}

void addChannelToEntries(LocationState* state, uint8_t channel, std::string inputData){
  int pos = 0;
  for(int i=0; i<inputData.length(); i+=2){
    uint8_t row = inputData[i];
    uint8_t col = inputData[i+1];
    addEntry(state,row,col,channel,pos);
    pos++;//increment position
  }
}

std::string getChannelEntriesData(LocationState* state, uint8_t channel, uint16_t numPos){
  std::string returnValue (numPos*2, 0);
  LocationEntry* currentLocation = state->firstRowLocation;
  while(currentLocation != NULL){
    if(currentLocation->channel == channel){
      returnValue[(currentLocation->pos) * 2] = currentLocation->row;
      returnValue[((currentLocation->pos) * 2)+1] = currentLocation->col;
    }
    currentLocation = currentLocation->nextRow;
  }
  return returnValue;
}

void describeLocation(LocationEntry* entry){
  Serial.println("Describing Entry");
  Serial.print("Row: ");
  Serial.println(entry->row);
  Serial.print("Col: ");
  Serial.println(entry->col);
  Serial.print("Channel: ");
  Serial.println(entry->channel);
  Serial.print("Pos: ");
  Serial.println(entry->pos);
  Serial.print("NextRow: ");
  Serial.println((entry->nextRow != NULL));
  Serial.print("NextCol: ");
  Serial.println((entry->nextCol != NULL));
}

void describeLocations(LocationState* state){
  LocationEntry* currentLocation = state->firstRowLocation;
  
  while(currentLocation != NULL){
    describeLocation(currentLocation);
    currentLocation = currentLocation->nextRow;
  }
}

void describeState(LocationState* state){
  Serial.println("Describing Locations State");
  Serial.print("Total Used Locations:");
  Serial.println(state->totalLocations);
  //describeLocations(state);

  Serial.println("Printing Location Grid");
  for(int row=0; row<LOCATION_GRID_SIZE; row++){
    Serial.print("Row ");
    Serial.print(row);
    if(row<10){
      Serial.print(" ");
    }
    Serial.print(" | ");
    for(int col=0; col<LOCATION_GRID_SIZE; col++){
      int value = entryAtLocation(state, row, col);
      if(value == -1){
        Serial.print(" .");
      }else if(value >= 0 && value <= 9){
        Serial.print(" ");
        Serial.print(value);
      }else{
        Serial.print(value);
      }
    }
    Serial.println(" |");
  }
}

int entryAtLocation(LocationState* state, uint8_t row, uint8_t col){
  LocationEntry* currentLocation = state->firstRowLocation;
  if(state->rowIndexes[row] != NULL){
    currentLocation = state->rowIndexes[row];
  }
  while(currentLocation != NULL){
    if(currentLocation->row == row && currentLocation->col == col){
      return currentLocation->channel;
    }else{
      currentLocation = currentLocation->nextRow;
    }
  }
  return -1;
}

//Location Management Functions Above
//Location Animation Functions Below

bool setColorAtLocation(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t row, uint8_t col, CRGB color){
  bool found = false;
  LocationEntry * currentLocation = state->rowIndexes[row];
  if(currentLocation == NULL){
    currentLocation = state->firstRowLocation;
  }
  while(currentLocation != NULL){
    if(currentLocation->row == row && currentLocation->col == col){
      setColorAtPos(&channels[currentLocation->channel], currentLocation->pos, color);
      found = true;
    }else if(currentLocation->row > row || (currentLocation->row == row && currentLocation->col > col)){
      break;
    }
  }
  return found;
}

bool setColorAll(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color){
  bool found = false;
  LocationEntry * currentLocation = state->firstRowLocation;
  while(currentLocation != NULL){
    setColorAtPos(&channels[currentLocation->channel], currentLocation->pos, color);
    currentLocation = currentLocation->nextRow;
  }
  return true;
}

bool setColorAtRow(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t row, CRGB color){
  bool found = false;
  LocationEntry * currentLocation = state->rowIndexes[row];
  if(currentLocation == NULL){
    currentLocation = state->firstRowLocation;
  }
  while(currentLocation != NULL){
    if(currentLocation->row == row){
      setColorAtPos(&channels[currentLocation->channel], currentLocation->pos, color);
      found = true;
    }else if(currentLocation->row > row){
      break;
    }
    currentLocation = currentLocation->nextRow;
  }
  return true;
}

bool setColorAtCol(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t col, CRGB color){
  bool found = false;
  LocationEntry * currentLocation = state->colIndexes[col];
    if(currentLocation == NULL){
    currentLocation = state->firstColLocation;
  }
  while(currentLocation != NULL){
    if(currentLocation->col == col){
      setColorAtPos(&channels[currentLocation->channel], currentLocation->pos, color);
      found = true;
    }
    currentLocation = currentLocation->nextCol;
  }
  return true;
}


CRGB getColorAtLocation(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t row, uint8_t col){
  Serial.println("Get color at Location");
  LocationEntry * currentLocation = state->rowIndexes[row];
  if(currentLocation == NULL){
    currentLocation = state->firstRowLocation;
  }
  while(currentLocation != NULL){
    if(currentLocation->row == row && currentLocation->col == col){
      return getColorAtPos(&channels[currentLocation->channel], currentLocation->pos);
    }else if(currentLocation->row > row || (currentLocation->row == row && currentLocation->col > col)){
      break;
    }
    currentLocation = currentLocation->nextRow;
  }
  return CRGB::Black;
}


CRGB getColorAtRow(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t row){
  //Serial.print("Get color at row: ");
  //Serial.println(row);
  LocationEntry * currentLocation = state->rowIndexes[row];
  if(currentLocation == NULL){
    //Serial.println("No index found starting at first Row");
    currentLocation = state->firstRowLocation;
  }
  while(currentLocation != NULL){
    if(currentLocation->row == row){
      //Serial.println("Found color at row");
      return getColorAtPos(&channels[currentLocation->channel], currentLocation->pos);
    }else if(currentLocation->row > row){
      break;
    }
    currentLocation = currentLocation->nextRow;
  }
  //Serial.println("Color not found at Row");
  return CRGB::Black;
}


CRGB getColorAtCol(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t col){
  LocationEntry * currentLocation = state->colIndexes[col];
  if(currentLocation == NULL){
    currentLocation = state->firstColLocation;
  }
  while(currentLocation != NULL){
    if(currentLocation->col == col){
      return getColorAtPos(&channels[currentLocation->channel], currentLocation->pos);
    }else if(currentLocation->col > col){
      break;
    }
    currentLocation = currentLocation->nextCol;
  }
  return CRGB::Black;
}

void shiftColorUpRow(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color){
  //Serial.println("Shifting color up row");
  CRGB savedColor;
  CRGB newColor = color;
  for(int i=0; i<LOCATION_GRID_SIZE; i++){
    savedColor = getColorAtRow(state, channels, i);
    bool updated = setColorAtRow(state,channels, i, newColor);
    if(updated){
      //Serial.println("Row updated, saving color to be used on next row");
      newColor = savedColor;
    }
  }
}

void shiftColorDownRow(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color){
  CRGB savedColor;
  CRGB newColor = color;
  for(int i=LOCATION_GRID_SIZE-1; i>=0; i--){
    savedColor = getColorAtRow(state, channels, i);
    setColorAtRow(state,channels, i, newColor);
    newColor = savedColor;
  }
}

void shiftColorUpCol(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color){
  CRGB savedColor;
  CRGB newColor = color;
  for(int i=0; i<LOCATION_GRID_SIZE; i++){
    savedColor = getColorAtCol(state, channels, i);
    setColorAtCol(state,channels, i, newColor);
    newColor = savedColor;
  }
}

void shiftColorDownCol(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color){
  CRGB savedColor;
  CRGB newColor = color;
  for(int i=LOCATION_GRID_SIZE-1; i>=0; i--){
    savedColor = getColorAtCol(state, channels, i);
    setColorAtCol(state,channels, i, newColor);
    newColor = savedColor;
  }
}

void loadTestData(LocationState* state){
  Serial.println("Start Loading Location Test Data");

  //Rock Light Right Front
  addEntry(state, 0, 63, 0, 0);
  addEntry(state, 1, 63, 0, 1);
  addEntry(state, 2, 63, 0, 2);
  addEntry(state, 3, 63, 0, 3);
  addEntry(state, 4, 63, 0, 4);
  addEntry(state, 5, 63, 0, 5);
  addEntry(state, 6, 63, 0, 6);
  addEntry(state, 7, 63, 0, 7);
  addEntry(state, 8, 63, 0, 8);
  addEntry(state, 9, 63, 0, 9);
  addEntry(state, 10, 63, 0, 10);
  addEntry(state, 11, 63, 0, 11);
  addEntry(state, 12, 63, 0, 12);
  addEntry(state, 13, 63, 0, 13);
  addEntry(state, 14, 63, 0, 14);
  addEntry(state, 15, 63, 0, 15);
  addEntry(state, 16, 63, 0, 16);
  addEntry(state, 17, 63, 0, 17);

  //Right Interior Light
  addEntry(state, 20, 40, 1, 0);
  addEntry(state, 21, 40, 1, 1);
  addEntry(state, 22, 40, 1, 2);
  addEntry(state, 23, 40, 1, 3);
  addEntry(state, 24, 40, 1, 4);
  addEntry(state, 25, 40, 1, 5);
  addEntry(state, 26, 40, 1, 6);
  addEntry(state, 27, 40, 1, 7);
  addEntry(state, 28, 40, 1, 8);
  addEntry(state, 29, 40, 1, 9);
  addEntry(state, 30, 40, 1, 10);
  addEntry(state, 31, 40, 1, 11);
  addEntry(state, 32, 40, 1, 12);
  addEntry(state, 33, 40, 1, 13);
  addEntry(state, 34, 40, 1, 14);
  addEntry(state, 35, 40, 1, 15);
  addEntry(state, 36, 40, 1, 16);
  addEntry(state, 37, 40, 1, 17);

  //Whip Right
  addEntry(state, 39, 1, 2, 0);
  addEntry(state, 40, 1, 2, 1);
  addEntry(state, 41, 1, 2, 2);
  addEntry(state, 42, 1, 2, 3);
  addEntry(state, 43, 1, 2, 4);
  addEntry(state, 44, 1, 2, 5);
  addEntry(state, 45, 1, 2, 6);
  addEntry(state, 46, 1, 2, 7);
  addEntry(state, 47, 1, 2, 8);
  addEntry(state, 48, 1, 2, 9);
  addEntry(state, 49, 1, 2, 10);
  addEntry(state, 50, 1, 2, 11);
  addEntry(state, 51, 1, 2, 12);
  addEntry(state, 52, 1, 2, 13);
  addEntry(state, 53, 1, 2, 14);
  addEntry(state, 54, 1, 2, 15);
  addEntry(state, 55, 1, 2, 16);
  addEntry(state, 56, 1, 2, 17);
  addEntry(state, 57, 1, 2, 18);
  addEntry(state, 58, 1, 2, 19);
  addEntry(state, 59, 1, 2, 20);
  addEntry(state, 60, 1, 2, 21);
  addEntry(state, 61, 1, 2, 22);
  addEntry(state, 62, 1, 2, 23);
  addEntry(state, 63, 1, 2, 24);

    //Rock Light Right Rear
  addEntry(state, 46, 0, 3, 0);
  addEntry(state, 47, 0, 3, 1);
  addEntry(state, 48, 0, 3, 2);
  addEntry(state, 49, 0, 3, 3);
  addEntry(state, 50, 0, 3, 4);
  addEntry(state, 51, 0, 3, 5);
  addEntry(state, 52, 0, 3, 6);
  addEntry(state, 53, 0, 3, 7);
  addEntry(state, 54, 0, 3, 8);
  addEntry(state, 55, 0, 3, 9);
  addEntry(state, 56, 0, 3, 10);
  addEntry(state, 57, 0, 3, 11);
  addEntry(state, 58, 0, 3, 12);
  addEntry(state, 59, 0, 3, 13);
  addEntry(state, 60, 0, 3, 14);
  addEntry(state, 61, 0, 3, 15);
  addEntry(state, 62, 0, 3, 16);
  addEntry(state, 63, 0, 3, 17);

  //Rock Light Left Rear
  addEntry(state, 46, 63, 4, 0);
  addEntry(state, 47, 63, 4, 1);
  addEntry(state, 48, 63, 4, 2);
  addEntry(state, 49, 63, 4, 3);
  addEntry(state, 50, 63, 4, 4);
  addEntry(state, 51, 63, 4, 5);
  addEntry(state, 52, 63, 4, 6);
  addEntry(state, 53, 63, 4, 7);
  addEntry(state, 54, 63, 4, 8);
  addEntry(state, 55, 63, 4, 9);
  addEntry(state, 56, 63, 4, 10);
  addEntry(state, 57, 63, 4, 11);
  addEntry(state, 58, 63, 4, 12);
  addEntry(state, 59, 63, 4, 13);
  addEntry(state, 60, 63, 4, 14);
  addEntry(state, 61, 63, 4, 15);
  addEntry(state, 62, 63, 4, 16);
  addEntry(state, 63, 63, 4, 17);

  //Whip Left
  addEntry(state, 39, 62, 5, 0);
  addEntry(state, 40, 62, 5, 1);
  addEntry(state, 41, 62, 5, 2);
  addEntry(state, 42, 62, 5, 3);
  addEntry(state, 43, 62, 5, 4);
  addEntry(state, 44, 62, 5, 5);
  addEntry(state, 45, 62, 5, 6);
  addEntry(state, 46, 62, 5, 7);
  addEntry(state, 47, 62, 5, 8);
  addEntry(state, 48, 62, 5, 9);
  addEntry(state, 49, 62, 5, 10);
  addEntry(state, 50, 62, 5, 11);
  addEntry(state, 51, 62, 5, 12);
  addEntry(state, 52, 62, 5, 13);
  addEntry(state, 53, 62, 5, 14);
  addEntry(state, 54, 62, 5, 15);
  addEntry(state, 55, 62, 5, 16);
  addEntry(state, 56, 62, 5, 17);
  addEntry(state, 57, 62, 5, 18);
  addEntry(state, 58, 62, 5, 19);
  addEntry(state, 59, 62, 5, 20);
  addEntry(state, 60, 62, 5, 21);
  addEntry(state, 61, 62, 5, 22);
  addEntry(state, 62, 62, 5, 23);
  addEntry(state, 63, 62, 5, 24);

  //Left Interior Light
  addEntry(state, 20, 23, 6, 0);
  addEntry(state, 21, 23, 6, 1);
  addEntry(state, 22, 23, 6, 2);
  addEntry(state, 23, 23, 6, 3);
  addEntry(state, 24, 23, 6, 4);
  addEntry(state, 25, 23, 6, 5);
  addEntry(state, 26, 23, 6, 6);
  addEntry(state, 27, 23, 6, 7);
  addEntry(state, 28, 23, 6, 8);
  addEntry(state, 29, 23, 6, 9);
  addEntry(state, 30, 23, 6, 10);
  addEntry(state, 31, 23, 6, 11);
  addEntry(state, 32, 23, 6, 12);
  addEntry(state, 33, 23, 6, 13);
  addEntry(state, 34, 23, 6, 14);
  addEntry(state, 35, 23, 6, 15);
  addEntry(state, 36, 23, 6, 16);
  addEntry(state, 37, 23, 6, 17);

  //Rock Light Left Front
  addEntry(state, 0, 0, 7, 0);
  addEntry(state, 1, 0, 7, 1);
  addEntry(state, 2, 0, 7, 2);
  addEntry(state, 3, 0, 7, 3);
  addEntry(state, 4, 0, 7, 4);
  addEntry(state, 5, 0, 7, 5);
  addEntry(state, 6, 0, 7, 6);
  addEntry(state, 7, 0, 7, 7);
  addEntry(state, 8, 0, 7, 8);
  addEntry(state, 9, 0, 7, 9);
  addEntry(state, 10, 0, 7, 10);
  addEntry(state, 11, 0, 7, 11);
  addEntry(state, 12, 0, 7, 12);
  addEntry(state, 13, 0, 7, 13);
  addEntry(state, 14, 0, 7, 14);
  addEntry(state, 15, 0, 7, 15);
  addEntry(state, 16, 0, 7, 16);
  addEntry(state, 17, 0, 7, 17);
  Serial.println("Finnished Loading Location Test Data");
}
