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
  LocationEntry * nextCol;
};

struct LocationState {
  LocationEntry * firstRowLocation;
  LocationEntry * lastRowLocation;
  LocationEntry * firstColLocation;
  LocationEntry * lastColLocation;
  int totalLocations;
  LocationEntry * rowIndexes[LOCATION_GRID_SIZE];
  LocationEntry * colIndexes[LOCATION_GRID_SIZE];
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

void addChannelToEntries(LocationState* state, uint8_t channel, std::string inputData);

std::string getChannelEntriesData(LocationState* state, uint8_t channel, uint16_t numPos);

void removeAllEntries(LocationState* state);

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

#endif
