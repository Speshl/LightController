#ifndef LOCATIONS_H
#define LOCATIONS_H
#include <EEPROM.h>
#include <channels.h>

#define LOCATION_GRID_SIZE 64

struct LocationEntry {
  uint8_t channel;
  uint8_t pos;
  uint8_t row;
  uint8_t col;
  LocationEntry * nextRow;
  LocationEntry * previousRow;
  LocationEntry * nextCol;
  LocationEntry * previousCol;
};

struct LocationState {
  LocationEntry * firstRowLocation;
  LocationEntry * lastRowLocation;
  LocationEntry * firstColLocation;
  LocationEntry * lastColLocation;
  int totalLocations;
  LocationEntry * rowIndexes[LOCATION_GRID_SIZE];
  LocationEntry * colIndexes[LOCATION_GRID_SIZE];
  LocationEntry * garbage;
};

//General
void loadTestData(LocationState* state);

void setInitialState(LocationState* state);

int compareEntriesByRow(LocationEntry* entry1, LocationEntry* entry2);

int compareToEntryByRow(LocationEntry* entry, uint8_t row, uint8_t col);

int compareEntriesByCol(LocationEntry* entry1, LocationEntry* entry2);

int compareToEntryByCol(LocationEntry* entry, uint8_t row, uint8_t col);

void updateRowIndexes(LocationState* state, LocationEntry* entry);

void rebuildRowIndex(LocationState* state);

void updateColIndexes(LocationState* state, LocationEntry* entry);

void rebuildColIndex(LocationState* state);

void rebuildIndexes(LocationState* state);

void addEntry(LocationState* state,uint8_t row, uint8_t col, uint8_t channel, uint8_t pos);

bool removeEntryByRow(LocationState* state, uint8_t row, uint8_t col, uint8_t channel, uint8_t pos);

void removeAllEntries(LocationState* state);

std::string getLocationGrid(LocationState* state);

void describeState(LocationState* state);

void describeLocation(LocationEntry* entry);

int entryAtLocation(LocationState* state, uint8_t row, uint8_t col);

//Light Commands

CRGB getColorAtLocation(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t row, uint8_t col);

CRGB getColorAtRow(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t row);

CRGB getColorAtCol(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t col);

bool setColorAtLocation(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t row, uint8_t col, CRGB color);

bool setColorAll(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color);

bool setColorAtRow(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t row, CRGB color);

bool setColorAtCol(LocationState* state, ChannelState channels[MAX_CHANNELS], uint8_t col, CRGB color);

void shiftColorUpRow(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color);

void shiftColorUpCol(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color);

void shiftColorDownCol(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color);

void shiftColorDownRow(LocationState* state, ChannelState channels[MAX_CHANNELS], CRGB color);

std::string getChannelLocations(LocationState* state, uint8_t channel, uint8_t maxPos);
std::string getChannelLocationAtPosition(LocationState* state, uint8_t channel, uint8_t pos);
void getChannelLocationAtPosition(LocationState* state, uint8_t channel, uint8_t pos, std::string* row, std::string* col);

void setChannelLocations(LocationState* state, uint8_t channel, uint8_t maxPos, std::string locations);

// UPDATED FOR DOUBLY LINKED LIST
void removeEntry(LocationState* state, LocationEntry * location);
void removeChannelEntries(LocationState* state, uint8_t channel);
void runGarbageCollector(LocationState* state);
void describeLocations(LocationState* state);
bool validate(LocationState* state);

#endif
