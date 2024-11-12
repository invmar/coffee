#ifndef PROFILE_MANAGER_H
#define PROFILE_MANAGER_H

#include <SD.h>
#include "RoasterConfig.h"

class ProfileManager {
    private:
        File profileFile;
        RoastProfile currentProfile;
        bool profileLoaded;
        
        /**
         * @brief Generate filename for profile
         */
        String getProfileFileName(int index);
        
    public:
        ProfileManager();
        
        /**
         * @brief Initialize SD card and profile storage
         */
        bool begin();
        
        /**
         * @brief Load profile by index
         */
        bool loadProfile(int index);
        
        /**
         * @brief Save current profile
         */
        bool saveProfile(const char* name);
        
        /**
         * @brief Get target temperature for current time
         */
        float getTargetTemp(unsigned long timeSeconds);
        
        /**
         * @brief Get target fan speed for current time
         */
        uint8_t getTargetFan(unsigned long timeSeconds);
        
        /**
         * @brief Get list of available profiles
         */
        int getProfileList(char names[][PROFILE_NAME_LENGTH]);
        
        /**
         * @brief Get current profile
         */
        RoastProfile* getCurrentProfile();
        
        /**
         * @brief Create new profile
         */
        void createNewProfile();
        
        /**
         * @brief Update profile point
         */
        void updateProfilePoint(unsigned long timeSeconds, float temp, uint8_t fan);
};

#endif