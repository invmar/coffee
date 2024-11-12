#ifndef ROASTER_CONTROL_H
#define ROASTER_CONTROL_H

#include "TempControl.h"
#include "PIDController.h"
#include "DisplayInterface.h"
#include "ProfileManager.h"
#include "RoasterConfig.h"

// Roasting stages
enum RoastStage {
    IDLE,
    CHARGING,
    DRYING,
    MAILLARD,
    FIRST_CRACK,
    DEVELOPMENT,
    COOLING,
    EMERGENCY_STOP
};

class RoasterControl {
    private:
        TempControl* tempControl;
        PIDController* pidControl;
        DisplayInterface* display;
        ProfileManager* profiles;
        
        // System state
        RoastStage currentStage;
        unsigned long roastStartTime;
        unsigned long stageStartTime;
        bool emergencyStop;
        bool manualMode;
        
        // Control values
        uint8_t fanSpeed;
        uint8_t heatPower;
        float targetTemp;
        
        /**
         * @brief Update roasting stage based on temperature and time
         */
        void updateStage();
        
        /**
         * @brief Handle emergency stop condition
         */
        void handleEmergencyStop();
        
        /**
         * @brief Log current roast data
         */
        void logRoastData();
        
    public:
        /**
         * @brief Constructor
         */
        RoasterControl(TempControl* temp, PIDController* pid, 
                      DisplayInterface* disp, ProfileManager* prof);
        
        /**
         * @brief Initialize roaster control system
         */
        void begin();
        
        /**
         * @brief Main control loop
         */
        void update();
        
        /**
         * @brief Start roasting process
         */
        void startRoast(bool useProfile = false);
        
        /**
         * @brief Stop roasting process
         */
        void stopRoast();
        
        /**
         * @brief Adjust fan speed
         */
        void adjustFan(int8_t adjustment);
        
        /**
         * @brief Adjust heat power
         */
        void adjustHeat(int8_t adjustment);
        
        /**
         * @brief Toggle manual/profile mode
         */
        void toggleManualMode();
        
        /**
         * @brief Get current roast stage
         */
        RoastStage getCurrentStage() { return currentStage; }
        
        /**
         * @brief Check if roasting is active
         */
        bool isRoasting() { return currentStage != IDLE && currentStage != EMERGENCY_STOP; }
};

#endif