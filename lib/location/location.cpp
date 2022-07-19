#include "location.h"

void setInitialState(LocationState* state){
  Serial.println("Start Setting Initial Location State");
  state->totalLocations = 0;

  for(int i=0; i<LOCATION_GRID_SIZE; i++){
    state->rowIndexes[i] = NULL;
    state->colIndexes[i] = NULL;
    state->heightIndexes[i] = NULL;
  }
  removeAllEntries(state);
  loadTestData(state);
  Serial.println("Finished Setting Initial Location State");
}

int compareEntriesByRow(LocationEntry* entry1, LocationEntry* entry2){
  if(entry1 == entry2){
    return 0;
  }
  if(entry1 == NULL){
    return 1;
  }
  if(entry2 == NULL){
    return -1;
  }
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
      if(entry1->height > entry2->height){
        return 1;
      }else if(entry1->height < entry2->height){
        return -1;
      }
      else{
        return 0; //Identical
      }
    }
  }
}

int compareEntriesByCol(LocationEntry* entry1, LocationEntry* entry2){
  if(entry1 == entry2){
    return 0;
  }
  if(entry1 == NULL){
    return 1;
  }
  if(entry2 == NULL){
    return -1;
  }
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
      if(entry1->height > entry2->height){
        return 1;
      }else if(entry1->height < entry2->height){
        return -1;
      }
      else{
        return 0; //Identical
      }
    }
  }
}

int compareEntriesByHeight(LocationEntry* entry1, LocationEntry* entry2){
  if(entry1 == entry2){
    return 0;
  }
  if(entry1 == NULL){
    return 1;
  }
  if(entry2 == NULL){
    return -1;
  }
  if(entry1->height > entry2->height){
    return 1;
  }else if(entry1->height < entry2->height){
    return -1;
  }
  else{
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
}

void describeRowIndexes(LocationState* state){
  Serial.println("Describing Row Indexes");
  for(int i=0; i<LOCATION_GRID_SIZE; i++){
    if(state->rowIndexes[i] == NULL){
      Serial.print("No Index for Row ");
      Serial.println(i);
    }else{
      Serial.print("Row ");
      Serial.print(i);
      Serial.print(" Index Value - Channel: ");
      Serial.print(state->rowIndexes[i]->channel);
      Serial.print(" Row: ");
      Serial.print(state->rowIndexes[i]->row);
      Serial.print(" Col: ");
      Serial.println(state->rowIndexes[i]->col);
      Serial.print(" Height: ");
      Serial.println(state->rowIndexes[i]->height);
    }
  }
}

void removeFromRowIndexes(LocationState* state, LocationEntry* entry){
  for(int i=0; i<LOCATION_GRID_SIZE; i++){
    if(state->rowIndexes[i] == entry){
      if(entry->nextRow != NULL && entry->nextRow->row == entry->row){
        state->rowIndexes[i] = entry->nextRow;
      }else{
        state->rowIndexes[i] = NULL;
      }
    }
  }
}

void removeFromColIndexes(LocationState* state, LocationEntry* entry){
  for(int i=0; i<LOCATION_GRID_SIZE; i++){
    if(state->colIndexes[i] == entry){
      if(entry->nextCol != NULL && entry->nextCol->col == entry->col){
        state->colIndexes[i] = entry->nextCol;
      }else{
        state->colIndexes[i] = NULL;
      }
    }
  }
}

void removeFromHeightIndexes(LocationState* state, LocationEntry* entry){
  for(int i=0; i<LOCATION_GRID_SIZE; i++){
    if(state->heightIndexes[i] == entry){
      if(entry->nextHeight != NULL && entry->nextHeight->height == entry->height){
        state->heightIndexes[i] = entry->nextHeight;
      }else{
        state->heightIndexes[i] = NULL;
      }
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
      state->colIndexes[entry->col] = entry;
    }
  }
}

void updateHeightIndexes(LocationState* state, LocationEntry* entry){
  if (state->heightIndexes[entry->height] == NULL){
    state->heightIndexes[entry->height] = entry;
  }else{
    if(compareEntriesByHeight(state->heightIndexes[entry->height], entry) == 1){
      state->heightIndexes[entry->height] = entry;
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

void rebuildHeightIndexes(LocationState* state){
  //Serial.println("Rebuild Row Indexes");
  for(int i=0; i<LOCATION_GRID_SIZE; i++){
    state->heightIndexes[i] = NULL;
  }
  
  LocationEntry* currentLocation = state->firstHeightLocation;
  while(currentLocation != NULL){
    updateHeightIndexes(state, currentLocation);
    currentLocation = currentLocation->nextHeight;
  }
}

void rebuildIndexes(LocationState* state){
  rebuildRowIndexes(state);
  rebuildColIndexes(state);
  rebuildHeightIndexes(state);
}

void updateIndexes(LocationState* state, LocationEntry* entry){
  updateRowIndexes(state, entry);
  updateColIndexes(state, entry);
  updateHeightIndexes(state, entry);
}

void removeFromIndexes(LocationState* state, LocationEntry* entry){
  removeFromRowIndexes(state, entry);
  removeFromColIndexes(state, entry);
  removeFromHeightIndexes(state, entry);
}

LocationEntry * getRowStartPointForEntry(LocationState* state, LocationEntry* entry){
  if(state->rowIndexes[entry->row] != NULL){
    return state->rowIndexes[entry->row];
  }else{
    return state->firstRowLocation;
  }
}

LocationEntry * getColStartPointForEntry(LocationState* state, LocationEntry* entry){
  if(state->colIndexes[entry->col] != NULL){
    return state->colIndexes[entry->col];
  }else{
    return state->firstColLocation;
  }
}

LocationEntry * getHeightStartPointForEntry(LocationState* state, LocationEntry* entry){
  if(state->heightIndexes[entry->height] != NULL){
    return state->heightIndexes[entry->height];
  }else{
    return state->firstHeightLocation;
  }
}

void addEntryByRow(LocationState* state, LocationEntry* entry){
  LocationEntry * currentLocation = getRowStartPointForEntry(state,entry);
  if(currentLocation == NULL){ //First entry
    state->firstRowLocation = entry;
    state->lastRowLocation = entry;
    return;
  }
  LocationEntry * previousLocation = currentLocation->previousRow;
  bool added = false;
  while(currentLocation != NULL){
    if(compareEntriesByRow(entry, currentLocation) == -1){//check if new entry should go before currentLocation
      if(currentLocation == state->firstRowLocation){
        state->firstRowLocation = entry;//goes to front of list
      }
      if(previousLocation != NULL){
        previousLocation->nextRow = entry;//links to previous node
        entry->previousRow = previousLocation;
      }
      entry->nextRow = currentLocation; //new entry points to currentLocation
      currentLocation->previousRow = entry;
      added = true;
      break;
    }
    previousLocation = currentLocation;
    currentLocation = currentLocation->nextRow; //Increment currentLocation for next iteration of loop
  }
  if(added == false){
    entry->previousRow = state->lastRowLocation;
    if(state->lastRowLocation != NULL){
      state->lastRowLocation->nextRow = entry;
    }
    state->lastRowLocation = entry;
  }
}

void addEntryByCol(LocationState* state, LocationEntry* entry){
  LocationEntry * currentLocation = getColStartPointForEntry(state,entry);
  if(currentLocation == NULL){ //First entry
    state->firstColLocation = entry;
    state->lastColLocation = entry;
    return;
  }
  LocationEntry * previousLocation = currentLocation->previousCol;
  bool added = false;
  while(currentLocation != NULL){
    if(compareEntriesByCol(entry, currentLocation) == -1){//check if new entry should go before currentLocation
      if(currentLocation == state->firstColLocation){ //If NULL then no previous location, goes to front of list
        state->firstColLocation->previousCol = entry;
        entry->nextCol = state->firstColLocation;
        state->firstColLocation = entry;//goes to front of list
      }else{
        previousLocation->nextCol = entry;//links to previous node
        entry->previousCol = previousLocation;
      }
      entry->nextCol = currentLocation; //new entry points to currentLocation
      currentLocation->previousCol = entry;
      added = true;
      break;
    }
    previousLocation = currentLocation;
    currentLocation = currentLocation->nextCol; //Increment currentLocation for next iteration of loop
  }
  if(added == false){
    entry->previousCol = state->lastColLocation;
    if(state->lastColLocation != NULL){
      state->lastColLocation->nextCol = entry;
    }
    state->lastColLocation = entry;
  }
}

void addEntryByHeight(LocationState* state, LocationEntry* entry){
  LocationEntry * currentLocation = getHeightStartPointForEntry(state,entry);
  if(currentLocation == NULL){ //First entry
    state->firstHeightLocation = entry;
    state->lastHeightLocation = entry;
    return;
  }
  LocationEntry * previousLocation = currentLocation->previousHeight;
  bool added = false;
  while(currentLocation != NULL){
    if(compareEntriesByHeight(entry, currentLocation) == -1){//check if new entry should go before currentLocation
      if(currentLocation == state->firstHeightLocation){
        state->firstHeightLocation = entry;//goes to front of list
      }
      if(previousLocation != NULL){
        previousLocation->nextHeight = entry;//links to previous node
        entry->previousHeight = previousLocation;
      }
      entry->nextHeight = currentLocation; //new entry points to currentLocation
      currentLocation->previousHeight = entry;
      added = true;
      break;
    }
    previousLocation = currentLocation;
    currentLocation = currentLocation->nextHeight; //Increment currentLocation for next iteration of loop
  }
  if(added == false){
    entry->previousHeight = state->lastHeightLocation;
    if(state->lastHeightLocation != NULL){
      state->lastHeightLocation->nextHeight = entry;
    }
    state->lastHeightLocation = entry;
  }
}

void addEntry(LocationState* state, uint8_t row, uint8_t col, uint8_t height, uint8_t channel, uint8_t pos){
  if(row < LOCATION_GRID_SIZE && col < LOCATION_GRID_SIZE){
    LocationEntry * entry = (LocationEntry *) malloc(sizeof(LocationEntry));
    entry->row = row;
    entry->col = col;
    entry->height = height;
    entry->channel = channel;
    entry->pos = pos;
    entry->nextRow = NULL;
    entry->previousRow = NULL;
    entry->nextCol = NULL;
    entry->previousCol = NULL;
    entry->nextHeight = NULL;
    entry->previousHeight = NULL;
    //Serial.println("Adding Entry");
    //describeLocation(entry);
    //validate(state);
    addEntryByRow(state, entry);
    addEntryByCol(state, entry);
    addEntryByHeight(state, entry);
    updateIndexes(state, entry);
    state->totalLocations++;
    //validate(state);
  }
}

void runGarbageCollector(LocationState* state){
  while(state->garbage != NULL){
    LocationEntry * toRemove = state->garbage;
    state->garbage = toRemove->nextRow;
    Serial.println("Deleting Garbage");
    free(toRemove);
  }
}

void addToGarbage(LocationState* state, LocationEntry * entry){
  entry->nextCol = NULL;
  entry->nextRow = NULL;
  entry->nextHeight = NULL;
  entry->previousCol = NULL;
  entry->previousRow = NULL;
  entry->previousHeight = NULL;
  if(state->garbage == NULL){
    state->garbage = entry;
  }else{
    state->garbage->previousRow = entry;
    entry->nextRow = state->garbage;
    state->garbage = entry;
  }
}

void removeEntry(LocationState* state, LocationEntry * location){
    /*Serial.print("Removing Location :");
    describeLocation(location);*/
    location->channel = 255;//Easy way to tell if a locationEntry has been marked for deletion

    LocationEntry *previousRow = location->previousRow;
    if(previousRow != NULL){
      previousRow->nextRow = location->nextRow;
    }
    LocationEntry *nextRow = location->nextRow;
    if(nextRow != NULL){
      nextRow->previousRow = location->previousRow;
    }

    LocationEntry *previousCol = location->previousCol;
    if(previousCol != NULL){
      previousCol->nextCol = location->nextCol;
    }
    LocationEntry *nextCol = location->nextCol;
    if(nextCol != NULL){
      nextCol->previousCol = location->previousCol;
    }

    LocationEntry *previousHeight = location->previousHeight;
    if(previousHeight != NULL){
      previousHeight->nextHeight = location->nextHeight;
    }
    LocationEntry *nextHeight = location->nextHeight;
    if(nextHeight != NULL){
      nextHeight->previousHeight = location->previousHeight;
    }


    if(state->firstRowLocation == location){
      state->firstRowLocation = location->nextRow;
      if(state->firstRowLocation != NULL){
        state->firstRowLocation->previousRow = NULL;
      }
    }
    if(state->lastRowLocation == location){
      state->lastRowLocation = location->previousRow;
      if(state->lastRowLocation != NULL){
        state->lastRowLocation->nextRow = NULL;
      }
    }

    if(state->firstColLocation == location){
      state->firstColLocation = location->nextCol;
      if(state->firstColLocation != NULL){
        state->firstColLocation->previousCol = NULL;
      }
    }
    if(state->lastColLocation == location){
      state->lastColLocation = location->previousCol;
      if(state->lastColLocation != NULL){
        state->lastColLocation->nextCol = NULL;
      }
    }

    if(state->firstHeightLocation == location){
      state->firstHeightLocation = location->nextHeight;
      if(state->firstHeightLocation != NULL){
        state->firstHeightLocation->previousHeight = NULL;
      }
    }
    if(state->lastHeightLocation == location){
      state->lastHeightLocation = location->previousHeight;
      if(state->lastHeightLocation != NULL){
        state->lastHeightLocation->nextHeight = NULL;
      }
    }

    removeFromIndexes(state, location);
    state->totalLocations--;

    //Add to garbage list to be freeed later
    addToGarbage(state,location);
}

void removeChannelEntries(LocationState* state, uint8_t channel){
  //validate(state);
  Serial.print("Removing Entries For Channel ");
  Serial.println(channel);
  LocationEntry * currentLocation = state->firstRowLocation;
  while(currentLocation != NULL){
    if(currentLocation->channel == channel){
      LocationEntry * toRemove = currentLocation;
      currentLocation = currentLocation->nextRow;
      removeEntry(state, toRemove);
    }else{
      currentLocation = currentLocation->nextRow;
    }
  }
}

void removeAllEntries(LocationState* state){
  Serial.println("Removing All Entries");
  LocationEntry * currentLocation = state->firstRowLocation;
  while(currentLocation != NULL){
    LocationEntry * toRemove = currentLocation;
    currentLocation = currentLocation->nextRow;
    free(toRemove);
  }
  state->totalLocations = 0;
  state->firstRowLocation = NULL;
  state->firstColLocation = NULL;
  state->firstHeightLocation = NULL;
  state->lastRowLocation = NULL;
  state->lastColLocation = NULL;
  state->lastHeightLocation = NULL;
  rebuildIndexes(state);
}

void describeLocation(LocationEntry* entry){
  if(entry == NULL){
    Serial.println("Location is empty");
    return;
  }
  Serial.println("***Describing Entry***");
  Serial.print("Row: ");
  Serial.println(entry->row);
  Serial.print("Col: ");
  Serial.println(entry->col);
  Serial.print("Height: ");
  Serial.println(entry->height);
  Serial.print("Channel: ");
  Serial.println(entry->channel);
  Serial.print("Pos: ");
  Serial.println(entry->pos);
  Serial.println("**********************");
}

void describeLocations(LocationState* state){
  LocationEntry* currentLocation = state->firstRowLocation;
  Serial.println("Descriibing all locations ordered by ROW");
  int foundLocations = 0;
  while(currentLocation != NULL){
    describeLocation(currentLocation);
    currentLocation = currentLocation->nextRow;
    foundLocations++;
  }
  Serial.print("Found ");
  Serial.print(foundLocations);
  Serial.println(" Locations");
}

int entryAtLocation(LocationState* state, uint8_t row, uint8_t col, uint8_t height){
  LocationEntry* currentLocation = state->firstRowLocation;
  if(state->rowIndexes[row] != NULL){
    currentLocation = state->rowIndexes[row];
  }
  while(currentLocation != NULL){
    if(currentLocation->row == row && currentLocation->col == col && currentLocation->height == height){
      return currentLocation->channel;
    }else{
      currentLocation = currentLocation->nextRow;
    }
  }
  return -1;
}

//Location Management Functions Above
//Location Animation Functions Below

bool setColorAtLocationIgnoreHeight(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t row, uint8_t col, CRGB color){
  bool found = false;
  LocationEntry * currentLocation = state->rowIndexes[row];
  if(currentLocation == NULL){
    currentLocation = state->firstRowLocation;
  }
  while(currentLocation != NULL){
    if(currentLocation->row == row && currentLocation->col == col){
      setColorAtPos(&channels[currentLocation->channel], currentLocation->pos, color);
      found = true;
      //break;
    }else if(currentLocation->row > row || (currentLocation->row == row && currentLocation->col > col)){
      break;
    }else{
      currentLocation = currentLocation->nextRow;
    }
  }
  return found;
}

bool setColorAtLocation(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t row, uint8_t col, uint8_t height, CRGB color){
  bool found = false;
  LocationEntry * currentLocation = state->rowIndexes[row];
  if(currentLocation == NULL){
    currentLocation = state->firstRowLocation;
  }
  while(currentLocation != NULL){
    if(currentLocation->row == row && currentLocation->col == col && currentLocation->height == height){
      setColorAtPos(&channels[currentLocation->channel], currentLocation->pos, color);
      found = true;
      //break;
    }else if(currentLocation->row > row || (currentLocation->row == row && currentLocation->col > col)){
      break;
    }else{
      currentLocation = currentLocation->nextRow;
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
    found = true;
  }
  return found;
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
  return found;
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
  return found;
}

bool setColorAtHeight(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t height, CRGB color){
  bool found = false;
  LocationEntry * currentLocation = state->heightIndexes[height];
    if(currentLocation == NULL){
    currentLocation = state->firstHeightLocation;
  }
  while(currentLocation != NULL){
    if(currentLocation->height == height){
      setColorAtPos(&channels[currentLocation->channel], currentLocation->pos, color);
      found = true;
    }
    currentLocation = currentLocation->nextHeight;
  }
  return found;
}


CRGB getColorAtLocation(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t row, uint8_t col, uint8_t height){
  //Serial.println("Get color at Location");
  LocationEntry * currentLocation = state->rowIndexes[row];
  if(currentLocation == NULL){
    currentLocation = state->firstRowLocation;
  }
  while(currentLocation != NULL){
    if(currentLocation->row == row && currentLocation->col == col && currentLocation->height == height){
      return getColorAtPos(&channels[currentLocation->channel], currentLocation->pos);
    }else if(currentLocation->row > row || (currentLocation->row == row && currentLocation->col > col)){
      break;
    }
    currentLocation = currentLocation->nextRow;
  }
  return CRGB::Black;
}


CRGB getColorAtRow(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t row){
  LocationEntry * currentLocation = state->rowIndexes[row];
  if(currentLocation == NULL){
    currentLocation = state->firstRowLocation;
  }
  while(currentLocation != NULL){
    if(currentLocation->row == row){
      return getColorAtPos(&channels[currentLocation->channel], currentLocation->pos);
    }else if(currentLocation->row > row){
      break;
    }
    currentLocation = currentLocation->nextRow;
  }
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

CRGB getColorAtHeight(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t height){
  LocationEntry * currentLocation = state->heightIndexes[height];
  if(currentLocation == NULL){
    currentLocation = state->firstHeightLocation;
  }
  while(currentLocation != NULL){
    if(currentLocation->height == height){
      return getColorAtPos(&channels[currentLocation->channel], currentLocation->pos);
    }else if(currentLocation->height > height){
      break;
    }
    currentLocation = currentLocation->nextHeight;
  }
  return CRGB::Black;
}

void shiftColorUpRow(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color){
  CRGB savedColor;
  CRGB newColor = color;
  for(int i=0; i<LOCATION_GRID_SIZE; i++){
    savedColor = getColorAtRow(state, channels, i);
    bool updated = setColorAtRow(state,channels, i, newColor);
    if(updated){
      newColor = savedColor;
    }
  }
}

void shiftColorDownRow(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color){
  CRGB savedColor;
  CRGB newColor = color;
  for(int i=LOCATION_GRID_SIZE-1; i>=0; i--){
    savedColor = getColorAtRow(state, channels, i);
    bool updated = setColorAtRow(state,channels, i, newColor);
    if(updated){
      newColor = savedColor;
    }
  }
}

void shiftColorUpCol(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color){
  CRGB savedColor;
  CRGB newColor = color;
  for(int i=0; i<LOCATION_GRID_SIZE; i++){
    savedColor = getColorAtCol(state, channels, i);
    bool updated = setColorAtCol(state,channels, i, newColor);
    if(updated){
      newColor = savedColor;
    }
  }
}

void shiftColorDownCol(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color){
  CRGB savedColor;
  CRGB newColor = color;
  for(int i=LOCATION_GRID_SIZE-1; i>=0; i--){
    savedColor = getColorAtCol(state, channels, i);
    bool updated = setColorAtCol(state,channels, i, newColor);
    if(updated){
      newColor = savedColor;
    }
  }
}

void shiftColorUpHeight(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color){
  CRGB savedColor;
  CRGB newColor = color;
  for(int i=0; i<LOCATION_GRID_SIZE; i++){
    savedColor = getColorAtHeight(state, channels, i);
    bool updated = setColorAtHeight(state,channels, i, newColor);
    if(updated){
      newColor = savedColor;
    }
  }
}

void shiftColorDownHeight(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color){
  CRGB savedColor;
  CRGB newColor = color;
  for(int i=LOCATION_GRID_SIZE-1; i>=0; i--){
    savedColor = getColorAtHeight(state, channels, i);
    bool updated = setColorAtHeight(state,channels, i, newColor);
    if(updated){
      newColor = savedColor;
    }
  }
}


void setChannelLocations(LocationState* state, uint8_t channel, uint8_t maxPos, uint8_t locations[]){
  removeChannelEntries(state, channel);
  for(int i=0; i<maxPos; i++){
    int offset = i*3;
    uint8_t x = locations[offset+0];
    uint8_t y = locations[offset+1];
    uint8_t z = locations[offset+2];
    addEntry(state,x,y,z,channel,i);
  }
}

void getChannelLocations(LocationState* state, uint8_t channel, uint8_t maxPos, uint8_t locations[]){
  for(int i=0; i<maxPos; i++){
    int offset = i * 3;
    uint8_t x = 0;
    uint8_t y = 0;
    uint8_t z = 0;

    getChannelLocationAtPosition(state, channel, i, &x, &y, &z);

    locations[offset] = x;
    locations[offset+1] = y;
    locations[offset+2] = z;
  }
}

void getChannelLocationAtPosition(LocationState* state, uint8_t channel, uint8_t pos, uint8_t *x, uint8_t *y, uint8_t *z){
  LocationEntry *currentLocation = state->firstRowLocation;
  while(currentLocation != NULL){
    if(currentLocation->channel == channel && currentLocation->pos == pos){
      *x = currentLocation->row;
      *y = currentLocation->col;
      *z = currentLocation->height;
      return;
    }else{
      currentLocation = currentLocation->nextRow;
    }
  }
  return;
}

void getChannelLocationAtPosition(LocationState* state, uint8_t channel, uint8_t pos, std::string* row, std::string* col, std::string* height){
  LocationEntry *currentLocation = state->firstRowLocation;
  while(currentLocation != NULL){
    if(currentLocation->channel == channel && currentLocation->pos == pos){
      char buffer[10];
      *row = itoa(currentLocation->row, buffer, 10);
      *col = itoa(currentLocation->col, buffer, 10);
      *height = itoa(currentLocation->height, buffer, 10);
      return;
    }else{
      currentLocation = currentLocation->nextRow;
    }
  }
  return;
}

//Need to figure out a better way to display this
/*void describeState(LocationState* state){
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
}*/

//Need to figure out something different for this, Now a cube instead of a grid
/*std::string getLocationGrid(LocationState* state){
  std::string grid;
  for(int row=0; row<LOCATION_GRID_SIZE; row++){
    char buffer[10];
    std::string stringNumber = itoa(row,buffer,10);
    grid += "Row "+stringNumber;
    if(row<10){
      grid += " ";
    }
    grid +=" | ";
    for(int col=0; col<LOCATION_GRID_SIZE; col++){
      int value = entryAtLocation(state, row, col);
      if(value == -1){
        grid +="  .";
      }else if(value >= 0 && value <= 9){
        grid +="  ";
        char buffer2[10];
        std::string stringValue = itoa(value,buffer2,10);
        grid +=stringValue;
      }else{
        grid +=" ";
        char buffer2[10];
        std::string stringValue = itoa(value,buffer2,10);
        grid +=stringValue;
      }
    }
    grid +=" |\r\n";
  }

  //Serial.println(grid.c_str());
  return grid;
}*/

//Not updating for Height unless required
/*bool validate(LocationState* state){
  //Serial.println("Validating Locations");
  bool valid = true;
  LocationEntry* currentLocation = state->firstRowLocation;
  if(currentLocation == NULL && state->totalLocations !=0){
    Serial.println("ERROR: No First Row but have locations");
    valid = false;
  }
  while(currentLocation != NULL){

    if(currentLocation == state->firstRowLocation){
      if(state->firstRowLocation->previousRow != NULL){
        Serial.println("ERROR: First row has a previous row");
        valid = false;
      }
    }else if(currentLocation == state->firstColLocation){
      if(state->firstColLocation->previousCol != NULL){
        Serial.println("ERROR: First col has a previous col");
        valid = false;
      }
    }else if(currentLocation == state->lastRowLocation){
      if(state->lastRowLocation->nextRow != NULL){
        Serial.println("ERROR: Last row has a next row");
        valid = false;
      }
    }else if(currentLocation == state->lastColLocation){
      if(state->lastColLocation->nextCol != NULL){
        Serial.println("ERROR: Last col has a next col");
        valid = false;
      }
    }else{
      if(currentLocation->nextRow == NULL){
        Serial.println("ERROR: Next row is null but not last row");
        valid = false;
      }
      if(currentLocation->nextCol == NULL){
        Serial.println("ERROR: Next col is null but not last col");
        valid = false;
      }
      if(currentLocation->previousRow == NULL){
        Serial.println("ERROR: Previous row is null but not first row");
        valid = false;
      }
      if(currentLocation->previousCol == NULL){
        Serial.println("ERROR: Previous col is NULL but not first COL");
        valid = false;
      }
      LocationEntry* loopBackRow = currentLocation->previousRow->nextRow;
      LocationEntry* loopForwardRow = currentLocation->nextRow->previousRow;

      if(loopBackRow != currentLocation){
        Serial.println("ERROR: Loop Back ROW failed");
        valid = false;
      }

      if(loopForwardRow != currentLocation){
        Serial.println("ERROR: Loop Forward ROW failed");
        valid = false;
      }
    }
    currentLocation = currentLocation->nextRow;
  }
  return valid;
}*/

void loadTestData(LocationState* state){
  Serial.println("Start Loading Location Test Data");

  //TOP LEFT
  addEntry(state, 0, 0, 0, 0, 0);
  addEntry(state, 1, 0, 0, 0, 1);
  addEntry(state, 2, 0, 0, 0, 2);
  addEntry(state, 3, 0, 0, 0, 3);
  addEntry(state, 4, 0, 0, 0, 4);
  addEntry(state, 5, 0, 0, 0, 5);

  addEntry(state, 0, 1, 0, 1, 0);
  addEntry(state, 1, 1, 0, 1, 1);
  addEntry(state, 2, 1, 0, 1, 2);
  addEntry(state, 3, 1, 0, 1, 3);
  addEntry(state, 4, 1, 0, 1, 4);
  addEntry(state, 5, 1, 0, 1, 5);

  addEntry(state, 0, 2, 0, 2, 0);
  addEntry(state, 1, 2, 0, 2, 1);
  addEntry(state, 2, 2, 0, 2, 2);
  addEntry(state, 3, 2, 0, 2, 3);
  addEntry(state, 4, 2, 0, 2, 4);
  addEntry(state, 5, 2, 0, 2, 5);

  addEntry(state, 0, 3, 0, 3, 0);
  addEntry(state, 1, 3, 0, 3, 1);
  addEntry(state, 2, 3, 0, 3, 2);
  addEntry(state, 3, 3, 0, 3, 3);
  addEntry(state, 4, 3, 0, 3, 4);
  addEntry(state, 5, 3, 0, 3, 5);
  
  addEntry(state, 0, 4, 0, 4, 0);
  addEntry(state, 1, 4, 0, 4, 1);
  addEntry(state, 2, 4, 0, 4, 2);
  addEntry(state, 3, 4, 0, 4, 3);
  addEntry(state, 4, 4, 0, 4, 4);
  addEntry(state, 5, 4, 0, 4, 5);

  addEntry(state, 0, 5, 0, 5, 0);
  addEntry(state, 1, 5, 0, 5, 1);
  addEntry(state, 2, 5, 0, 5, 2);
  addEntry(state, 3, 5, 0, 5, 3);
  addEntry(state, 4, 5, 0, 5, 4);
  addEntry(state, 5, 5, 0, 5, 5);

  addEntry(state, 0, 6, 0, 6, 0);
  addEntry(state, 1, 6, 0, 6, 1);
  addEntry(state, 2, 6, 0, 6, 2);
  addEntry(state, 3, 6, 0, 6, 3);
  addEntry(state, 4, 6, 0, 6, 4);
  addEntry(state, 5, 6, 0, 6, 5);

  addEntry(state, 0, 7, 0, 7, 0);
  addEntry(state, 1, 7, 0, 7, 1);
  addEntry(state, 2, 7, 0, 7, 2);
  addEntry(state, 3, 7, 0, 7, 3);
  addEntry(state, 4, 7, 0, 7, 4);
  addEntry(state, 5, 7, 0, 7, 5);

  addEntry(state, 0, 8, 0, 8, 0);
  addEntry(state, 1, 8, 0, 8, 1);
  addEntry(state, 2, 8, 0, 8, 2);
  addEntry(state, 3, 8, 0, 8, 3);
  addEntry(state, 4, 8, 0, 8, 4);
  addEntry(state, 5, 8, 0, 8, 5);

  addEntry(state, 0, 9, 0, 9, 0);
  addEntry(state, 1, 9, 0, 9, 1);
  addEntry(state, 2, 9, 0, 9, 2);
  addEntry(state, 3, 9, 0, 9, 3);
  addEntry(state, 4, 9, 0, 9, 4);
  addEntry(state, 5, 9, 0, 9, 5);

  addEntry(state, 0, 10, 0, 10, 0);
  addEntry(state, 1, 10, 0, 10, 1);
  addEntry(state, 2, 10, 0, 10, 2);
  addEntry(state, 3, 10, 0, 10, 3);
  addEntry(state, 4, 10, 0, 10, 4);
  addEntry(state, 5, 10, 0, 10, 5);

  addEntry(state, 0, 11, 0, 11, 0);
  addEntry(state, 1, 11, 0, 11, 1);
  addEntry(state, 2, 11, 0, 11, 2);
  addEntry(state, 3, 11, 0, 11, 3);
  addEntry(state, 4, 11, 0, 11, 4);
  addEntry(state, 5, 11, 0, 11, 5);

  addEntry(state, 0, 12, 0, 12, 0);
  addEntry(state, 1, 12, 0, 12, 1);
  addEntry(state, 2, 12, 0, 12, 2);
  addEntry(state, 3, 12, 0, 12, 3);
  addEntry(state, 4, 12, 0, 12, 4);
  addEntry(state, 5, 12, 0, 12, 5);

  addEntry(state, 0, 13, 0, 13, 0);
  addEntry(state, 1, 13, 0, 13, 1);
  addEntry(state, 2, 13, 0, 13, 2);
  addEntry(state, 3, 13, 0, 13, 3);
  addEntry(state, 4, 13, 0, 13, 4);
  addEntry(state, 5, 13, 0, 13, 5);

  addEntry(state, 0, 14, 0, 14, 0);
  addEntry(state, 1, 14, 0, 14, 1);
  addEntry(state, 2, 14, 0, 14, 2);
  addEntry(state, 3, 14, 0, 14, 3);
  addEntry(state, 4, 14, 0, 14, 4);
  addEntry(state, 5, 14, 0, 14, 5);

  addEntry(state, 0, 15, 0, 15, 0);
  addEntry(state, 1, 15, 0, 15, 1);
  addEntry(state, 2, 15, 0, 15, 2);
  addEntry(state, 3, 15, 0, 15, 3);
  addEntry(state, 4, 15, 0, 15, 4);
  addEntry(state, 5, 15, 0, 15, 5);
  
  Serial.println("Finnished Loading Location Test Data");
}