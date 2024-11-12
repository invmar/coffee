#include "ProfileManager.h"

ProfileManager::ProfileManager() {
    profileLoaded = false;
    // Initialize empty profile
    memset(&currentProfile, 0, sizeof(RoastProfile));
}

bool ProfileManager::begin() {
    // Initialize SD card
    if (!SD.begin(SD_CS)) {
        return false;
    }
    
    // Create profiles directory if it doesn't exist
    if (!SD.exists("/profiles")) {
        SD.mkdir("/profiles");
    }
    
    return true;
}

String ProfileManager::getProfileFileName(int index) {
    return String("/profiles/profile_") + String(index) + ".dat";
}

bool ProfileManager::loadProfile(int index) {
    String fileName = getProfileFileName(index);
    
    // Open profile file
    profileFile = SD.open(fileName, FILE_READ);
    if (!profileFile) {
        return false;
    }
    
    // Read profile data
    profileFile.read((uint8_t*)&currentProfile, sizeof(RoastProfile));
    profileFile.close();
    
    profileLoaded = true;
    return true;
}

bool ProfileManager::saveProfile(const char* name) {
    // Find first available slot
    int slot = 0;
    while (slot < MAX_PROFILES && SD.exists(getProfileFileName(slot))) {
        slot++;
    }
    
    if (slot >= MAX_PROFILES) {
        return false;
    }
    
    // Update profile name
    strncpy(currentProfile.name, name, PROFILE_NAME_LENGTH - 1);
    currentProfile.name[PROFILE_NAME_LENGTH - 1] = '\0';
    
    // Save to file
    String fileName = getProfileFileName(slot);
    profileFile = SD.open(fileName, FILE_WRITE);
    if (!profileFile) {
        return false;
    }
    
    profileFile.write((uint8_t*)&currentProfile, sizeof(RoastProfile));
    profileFile.close();
    
    return true;
}

float ProfileManager::getTargetTemp(unsigned long timeSeconds) {
    if (!profileLoaded || timeSeconds >= 180) {
        return 0;
    }
    return currentProfile.tempCurve[timeSeconds];
}

uint8_t ProfileManager::getTargetFan(unsigned long timeSeconds) {
    if (!profileLoaded || timeSeconds >= 180) {
        return 0;
    }
    return currentProfile.fanCurve[timeSeconds];
}

int ProfileManager::getProfileList(char names[][PROFILE_NAME_LENGTH]) {
    int count = 0;
    
    // Iterate through possible profile slots
    for (int i = 0; i < MAX_PROFILES; i++) {
        String fileName = getProfileFileName(i);
        if (SD.exists(fileName)) {
            // Read profile name
            profileFile = SD.open(fileName, FILE_READ);
            if (profileFile) {
                profileFile.read((uint8_t*)names[count], PROFILE_NAME_LENGTH);
                profileFile.close();
                count++;
            }
        }
    }
    
    return count;
}

RoastProfile* ProfileManager::getCurrentProfile() {
    if (!profileLoaded) {
        return nullptr;
    }
    return &currentProfile;
}

void ProfileManager::createNewProfile() {
    // Reset current profile
    memset(&currentProfile, 0, sizeof(RoastProfile));
    profileLoaded = true;
}

void ProfileManager::updateProfilePoint(unsigned long timeSeconds, float temp, uint8_t fan) {
    if (timeSeconds < 180) {
        currentProfile.tempCurve[timeSeconds] = temp;
        currentProfile.fanCurve[timeSeconds] = fan;
        // Update total time if this is a later point
        if (timeSeconds / 60 + 1 > currentProfile.totalTime) {
            currentProfile.totalTime = timeSeconds / 60 + 1;
        }
    }
}