#ifndef STORAGE_H
#define STORAGE_H
#include <Preferences.h>

void initializeStorage(Preferences* preferences);

bool isFirstRun(Preferences* preferences);

void saveSwitchState(Preferences* preferences, std::string value);

void saveAnimationState(Preferences* preferences, std::string value);

void saveChannelDetailState(Preferences* preferences, int index, std::string value);

void saveChannelLocationState(Preferences* preferences, int index, std::string value);

std::string loadSwitchState(Preferences* preferences);

std::string loadAnimationState(Preferences* preferences);

std::string loadChannelDetailState(Preferences* preferences, int index);

std::string loadChannelLocationState(Preferences* preferences, int index);

#endif
