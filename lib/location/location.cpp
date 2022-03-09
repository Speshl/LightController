#include "location.h"

void setInitialState(LocationState* state){
  Serial.println("Start Setting Initial Location State");
  state->totalLocations = 0;

  for(int i=0; i<LOCATION_GRID_SIZE; i++){
    state->rowIndexes[i] = NULL;
    state->colIndexes[i] = NULL;
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
      return 0; //Identical
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
      return 0; //Identical
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

void updateRowIndexes(LocationState* state, LocationEntry* entry){
  /*Serial.println("Describe Entry to Index");
  describeLocation(entry);
  Serial.println("Updating Row Indexes");*/
  if (state->rowIndexes[entry->row] == NULL){
   /* Serial.print("Initial Index for Row ");
    Serial.print(entry->row);
    Serial.print(" = Channel - ");
    Serial.print(entry->channel);
    Serial.print(" Col - ");
    Serial.println(entry->col);*/
    state->rowIndexes[entry->row] = entry;
  }else{
    /*LocationEntry* currentIndex = state->rowIndexes[entry->row];
    Serial.println("Describe Current Index");
    describeLocation(currentIndex);*/
    if(compareEntriesByRow(state->rowIndexes[entry->row], entry) == 1){
      state->rowIndexes[entry->row] = entry;
      /*Serial.print("Updated Index for Row ");
      Serial.print(entry->row);
      Serial.print(" = Channel - ");
      Serial.print(entry->channel);
      Serial.print(" Col - ");
      Serial.println(entry->col);*/
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

bool validate(LocationState* state){
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
}


void rebuildIndexes(LocationState* state){
  rebuildColIndexes(state);
  rebuildRowIndexes(state);
}

void updateIndexes(LocationState* state, LocationEntry* entry){
  updateRowIndexes(state, entry);
  updateColIndexes(state, entry);
}

void removeFromIndexes(LocationState* state, LocationEntry* entry){
  removeFromRowIndexes(state, entry);
  removeFromColIndexes(state, entry);
}

LocationEntry * getRowStartPointForEntry(LocationState* state, LocationEntry* entry){
  if(state->rowIndexes[entry->row] != NULL){
    //return state->firstRowLocation;
    return state->rowIndexes[entry->row];
  }else{
    return state->firstRowLocation;
  }
}

LocationEntry * getColStartPointForEntry(LocationState* state, LocationEntry* entry){
  if(state->colIndexes[entry->col] != NULL){
    //return state->firstColLocation;
    return state->colIndexes[entry->col];
  }else{
    return state->firstColLocation;
  }
}

void addEntryByRow(LocationState* state, LocationEntry* entry){
  LocationEntry * currentLocation = getRowStartPointForEntry(state,entry);
  if(currentLocation == NULL){ //First entry
    //Serial.println("FRESH First Entry Found");
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
        //Serial.println("New FirstRow Found");
      }
      if(previousLocation != NULL){
        previousLocation->nextRow = entry;//links to previous node
        entry->previousRow = previousLocation;
        //Serial.println("Updated previous row");
      }
      entry->nextRow = currentLocation; //new entry points to currentLocation
      currentLocation->previousRow = entry;
      //Serial.println("Entry added successfully");
      //Serial.println("Record Before Added Entry");
      //describeLocation(entry->previousRow);
      //Serial.println("Record After Added Entry");
      //describeLocation(entry->nextRow);
      added = true;
      break;
    }
    previousLocation = currentLocation;
    currentLocation = currentLocation->nextRow; //Increment currentLocation for next iteration of loop
    //Serial.println("Looping for match");
  }
  if(added == false){
    entry->previousRow = state->lastRowLocation;
    if(state->lastRowLocation != NULL){
      state->lastRowLocation->nextRow = entry;
    }
    state->lastRowLocation = entry;
    //Serial.println("END OF ROW LIST BUT ENTRY NOT ADDED, ADDING TO END");
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

void addEntry(LocationState* state, uint8_t row, uint8_t col, uint8_t channel, uint8_t pos){
  if(row < LOCATION_GRID_SIZE && col < LOCATION_GRID_SIZE){
    LocationEntry * entry = (LocationEntry *) malloc(sizeof(LocationEntry));
    entry->row = row;
    entry->col = col;
    entry->channel = channel;
    entry->pos = pos;
    entry->nextRow = NULL;
    entry->previousRow = NULL;
    entry->nextCol = NULL;
    entry->previousCol = NULL;
    //Serial.println("Adding Entry");
    //describeLocation(entry);
    //validate(state);
    addEntryByRow(state, entry);
    addEntryByCol(state, entry);
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
  entry->previousCol = NULL;
  entry->previousRow = NULL;
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
    removeFromIndexes(state, location);
    state->totalLocations--;

    //Add to garbage list to be freeed later
    addToGarbage(state,location);
}

void removeChannelEntries(LocationState* state, uint8_t channel){
  validate(state);
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
  state->lastRowLocation = NULL;
  state->lastColLocation = NULL;
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

std::string getLocationGrid(LocationState* state){
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
      break;
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


CRGB getColorAtLocation(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t row, uint8_t col){
  //Serial.println("Get color at Location");
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

void setChannelLocations(LocationState* state, uint8_t channel, uint8_t maxPos, std::string locations){
  std::string leftToProcess = locations;
  /*Serial.print("Positions: ");
  Serial.println(locations.c_str());*/
  removeChannelEntries(state, channel);
  for(int i=0; i<maxPos; i++){
    char buffer[10];
    for(int j=0; j<leftToProcess.length(); j++){
      if(leftToProcess[j] != '|'){
        buffer[j] = leftToProcess[j];
      }else{
        leftToProcess = leftToProcess.substr(j+1, leftToProcess.length()); //remove this substring from front of leftToProcess
        break;
      }
    }
    //buffer now contains one location [X,Y]
    bool commaFound = false;
    std::string xString;
    std::string yString;
    for(int k=0; k<sizeof(buffer); k++){
      if(buffer[k] == '[' || buffer[k] == ' '){
        continue;
      }else if(buffer[k] == ']'){
        break;
      }else if(buffer[k] == ','){
        commaFound = true;
      }else{
        if(commaFound){
          yString += buffer[k];
        }else{
          xString += buffer[k];
        }
      }
    }
    addEntry(state,atoi(xString.c_str()),atoi(yString.c_str()),channel,i);
  }
}

std::string getChannelLocations(LocationState* state, uint8_t channel, uint8_t maxPos){
  std::string returnData;
  validate(state);
  for(int i=0; i<maxPos; i++){
    returnData += getChannelLocationAtPosition(state, channel, i);
    if(i != maxPos-1){
      returnData += "|";
    }
  }
  //Serial.println(returnData.c_str());
  return returnData;
}

std::string getChannelLocationAtPosition(LocationState* state, uint8_t channel, uint8_t pos){
  LocationEntry *currentLocation = state->firstRowLocation;
  while(currentLocation != NULL){
    if(currentLocation->channel == channel && currentLocation->pos == pos){
      char buffer[10];
      std::string stringRow = itoa(currentLocation->row, buffer, 10);
      std::string stringCol = itoa(currentLocation->col, buffer, 10);
      return "[" + stringRow + "," + stringCol + "]";
    }else{
      currentLocation = currentLocation->nextRow;
    }
  }
  return "";
}

void loadTestData(LocationState* state){
  Serial.println("Start Loading Location Test Data");

  //TOP LEFT
  addEntry(state, 0, 0, 0, 0);
  addEntry(state, 1, 0, 0, 1);
  addEntry(state, 2, 0, 0, 2);
  addEntry(state, 3, 0, 0, 3);
  addEntry(state, 4, 0, 0, 4);
  addEntry(state, 5, 0, 0, 5);

  addEntry(state, 0, 1, 1, 0);
  addEntry(state, 1, 1, 1, 1);
  addEntry(state, 2, 1, 1, 2);
  addEntry(state, 3, 1, 1, 3);
  addEntry(state, 4, 1, 1, 4);
  addEntry(state, 5, 1, 1, 5);

  addEntry(state, 0, 2, 2, 0);
  addEntry(state, 1, 2, 2, 1);
  addEntry(state, 2, 2, 2, 2);
  addEntry(state, 3, 2, 2, 3);
  addEntry(state, 4, 2, 2, 4);
  addEntry(state, 5, 2, 2, 5);

  addEntry(state, 0, 3, 3, 0);
  addEntry(state, 1, 3, 3, 1);
  addEntry(state, 2, 3, 3, 2);
  addEntry(state, 3, 3, 3, 3);
  addEntry(state, 4, 3, 3, 4);
  addEntry(state, 5, 3, 3, 5);
  
  addEntry(state, 0, 4, 4, 0);
  addEntry(state, 1, 4, 4, 1);
  addEntry(state, 2, 4, 4, 2);
  addEntry(state, 3, 4, 4, 3);
  addEntry(state, 4, 4, 4, 4);
  addEntry(state, 5, 4, 4, 5);

  addEntry(state, 0, 5, 5, 0);
  addEntry(state, 1, 5, 5, 1);
  addEntry(state, 2, 5, 5, 2);
  addEntry(state, 3, 5, 5, 3);
  addEntry(state, 4, 5, 5, 4);
  addEntry(state, 5, 5, 5, 5);

  addEntry(state, 0, 6, 6, 0);
  addEntry(state, 1, 6, 6, 1);
  addEntry(state, 2, 6, 6, 2);
  addEntry(state, 3, 6, 6, 3);
  addEntry(state, 4, 6, 6, 4);
  addEntry(state, 5, 6, 6, 5);

  addEntry(state, 0, 7, 7, 0);
  addEntry(state, 1, 7, 7, 1);
  addEntry(state, 2, 7, 7, 2);
  addEntry(state, 3, 7, 7, 3);
  addEntry(state, 4, 7, 7, 4);
  addEntry(state, 5, 7, 7, 5);

  addEntry(state, 0, 8, 8, 0);
  addEntry(state, 1, 8, 8, 1);
  addEntry(state, 2, 8, 8, 2);
  addEntry(state, 3, 8, 8, 3);
  addEntry(state, 4, 8, 8, 4);
  addEntry(state, 5, 8, 8, 5);

  addEntry(state, 0, 9, 9, 0);
  addEntry(state, 1, 9, 9, 1);
  addEntry(state, 2, 9, 9, 2);
  addEntry(state, 3, 9, 9, 3);
  addEntry(state, 4, 9, 9, 4);
  addEntry(state, 5, 9, 9, 5);

  addEntry(state, 0, 10, 10, 0);
  addEntry(state, 1, 10, 10, 1);
  addEntry(state, 2, 10, 10, 2);
  addEntry(state, 3, 10, 10, 3);
  addEntry(state, 4, 10, 10, 4);
  addEntry(state, 5, 10, 10, 5);

  addEntry(state, 0, 11, 11, 0);
  addEntry(state, 1, 11, 11, 1);
  addEntry(state, 2, 11, 11, 2);
  addEntry(state, 3, 11, 11, 3);
  addEntry(state, 4, 11, 11, 4);
  addEntry(state, 5, 11, 11, 5);

  addEntry(state, 0, 12, 12, 0);
  addEntry(state, 1, 12, 12, 1);
  addEntry(state, 2, 12, 12, 2);
  addEntry(state, 3, 12, 12, 3);
  addEntry(state, 4, 12, 12, 4);
  addEntry(state, 5, 12, 12, 5);

  addEntry(state, 0, 13, 13, 0);
  addEntry(state, 1, 13, 13, 1);
  addEntry(state, 2, 13, 13, 2);
  addEntry(state, 3, 13, 13, 3);
  addEntry(state, 4, 13, 13, 4);
  addEntry(state, 5, 13, 13, 5);

  addEntry(state, 0, 14, 14, 0);
  addEntry(state, 1, 14, 14, 1);
  addEntry(state, 2, 14, 14, 2);
  addEntry(state, 3, 14, 14, 3);
  addEntry(state, 4, 14, 14, 4);
  addEntry(state, 5, 14, 14, 5);

  addEntry(state, 0, 15, 15, 0);
  addEntry(state, 1, 15, 15, 1);
  addEntry(state, 2, 15, 15, 2);
  addEntry(state, 3, 15, 15, 3);
  addEntry(state, 4, 15, 15, 4);
  addEntry(state, 5, 15, 15, 5);
  
  Serial.println("Finnished Loading Location Test Data");
}

void loadCanAMData(LocationState* state){
  Serial.println("Start Loading Location Test Data");

  //Rock Light Right Front
  /*addEntry(state, 0, 63, 0, 0);
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
  addEntry(state, 17, 0, 7, 17);*/
  Serial.println("Finnished Loading Location Test Data");
  //describeRowIndexes(state);
}
