#ifndef LOCATIONS_H
#define LOCATIONS_H
#include <EEPROM.h>
#include <channels.h>

#define LOCATION_GRID_SIZE 255

struct LocationEntry {
  uint8_t channel;
  uint8_t pos;
  uint8_t row;
  uint8_t col;
  uint8_t height;
  LocationEntry * nextRow;
  LocationEntry * previousRow;
  LocationEntry * nextCol;
  LocationEntry * previousCol;
  LocationEntry * nextHeight;
  LocationEntry * previousHeight;
};

struct LocationState {
  LocationEntry * firstRowLocation;
  LocationEntry * lastRowLocation;
  LocationEntry * firstColLocation;
  LocationEntry * lastColLocation;
  LocationEntry * firstHeightLocation;
  LocationEntry * lastHeightLocation;
  int totalLocations;
  LocationEntry * rowIndexes[LOCATION_GRID_SIZE];
  LocationEntry * colIndexes[LOCATION_GRID_SIZE];
  LocationEntry * heightIndexes[LOCATION_GRID_SIZE];
  LocationEntry * garbage;
};

//General
void loadTestData(LocationState* state);

void setInitialState(LocationState* state);

int compareEntriesByRow(LocationEntry* entry1, LocationEntry* entry2);

int compareEntriesByCol(LocationEntry* entry1, LocationEntry* entry2);

int compareEntriesByHeight(LocationEntry* entry1, LocationEntry* entry2);

void updateRowIndexes(LocationState* state, LocationEntry* entry);

void rebuildRowIndex(LocationState* state);

void updateColIndexes(LocationState* state, LocationEntry* entry);

void rebuildColIndex(LocationState* state);

void updateHeightIndexes(LocationState* state, LocationEntry* entry);

void rebuildHeightIndex(LocationState* state);

void rebuildIndexes(LocationState* state);

void addEntry(LocationState* state,uint8_t row, uint8_t col, uint8_t height, uint8_t channel, uint8_t pos);

void removeAllEntries(LocationState* state);

//std::string getLocationGrid(LocationState* state);

//void describeState(LocationState* state);

void describeLocation(LocationEntry* entry);

int entryAtLocation(LocationState* state, uint8_t row, uint8_t col, uint8_t height);

//Light Commands

CRGB getColorAtLocation(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t row, uint8_t col, uint8_t height);

CRGB getColorAtRow(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t row);

CRGB getColorAtCol(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t col);

bool setColorAtLocationIgnoreHeight(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t row, uint8_t col, CRGB color);

bool setColorAtLocation(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t row, uint8_t col, uint8_t height, CRGB color);

bool setColorAll(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color);

bool setColorAtRow(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t row, CRGB color);

bool setColorAtCol(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t col, CRGB color);

bool setColorAtHeight(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t height, CRGB color);

void shiftColorUpRow(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color);

void shiftColorUpCol(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color);

void shiftColorUpHeight(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color);

void shiftColorDownCol(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color);

void shiftColorDownRow(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color);

void shiftColorDownHeight(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color);

void getChannelLocations(LocationState* state, uint8_t channel, uint8_t maxPos, uint8_t locations[]);
void getChannelLocationAtPosition(LocationState* state, uint8_t channel, uint8_t pos, uint8_t *x, uint8_t *y, uint8_t *z);
void getChannelLocationAtPosition(LocationState* state, uint8_t channel, uint8_t pos, std::string* row, std::string* col, std::string* height);

void setChannelLocations(LocationState* state, uint8_t channel, uint8_t maxPos, uint8_t locations[]);

void removeEntry(LocationState* state, LocationEntry * location);
void removeChannelEntries(LocationState* state, uint8_t channel);
void runGarbageCollector(LocationState* state);
void describeLocations(LocationState* state);
//bool validate(LocationState* state);

#endif
