#include "RoasterControl.h"

RoasterControl::RoasterControl(TempControl* temp, PIDController* pid, 
                             DisplayInterface* disp, ProfileManager* prof) {
    tempControl = temp;
    pidControl = pid;
    display = disp;
    profiles = prof;
    
    currentStage = IDLE;
    emergencyStop = false;
    manualMode = true;
    fanSpeed = 0;
    heatPower = 0;
    targetTemp = 0;
}

void RoasterControl::begin() {
    // Initialize components
    tempControl->begin();
    pidControl->begin();
    display->begin();
    profiles->begin();
    
    // Set up emergency stop pin
    pinMode(EMERGENCY_STOP_PIN, INPUT_PULLUP);
    pinMode(HEAT_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);
    
    // Initial state
    analogWrite(HEAT_PIN, 0);
    analogWrite(FAN_PIN, 0);
}

void RoasterControl::update() {
    // Check emergency stop
    if (digitalRead(EMERGENCY_STOP_PIN) == LOW) {
        handleEmergencyStop();
        return;
    }
    
    // Only process if roasting
    if (currentStage != IDLE && currentStage != EMERGENCY_STOP) {
        // Read temperatures and calculate RoR
        float currentTemp = tempControl->getAverageTemp();
        float ror = tempControl->getRateOfRise();
        
        // Safety check
        if (!tempControl->checkSafety()) {
            handleEmergencyStop();
            return;
        }
        
        // Update stage based on temperature and time
        updateStage();
        
        // In profile mode, get target values from profile
        if (!manualMode) {
            unsigned long roastTime = (millis() - roastStartTime) / 1000;
            targetTemp = profiles->getTargetTemp(roastTime);
            fanSpeed = profiles->getTargetFan(roastTime);
        }
        
        // PID control for heat
        pidControl->setSetpoint(targetTemp);
        pidControl->compute();
        heatPower = pidControl->getOutput();
        
        // Apply controls
        analogWrite(HEAT_PIN, heatPower);
        analogWrite(FAN_PIN, fanSpeed);
        
        // Update display
        display->update(currentTemp, ror, fanSpeed, heatPower);
        
        // Log data
        logRoastData();
    }
}

void RoasterControl::updateStage() {
    float currentTemp = tempControl->getAverageTemp();
    unsigned long stageTime = (millis() - stageStartTime) / 1000;
    
    switch (currentStage) {
        case CHARGING:
            if (currentTemp >= 100) {
                currentStage = DRYING;
                stageStartTime = millis();
                display->setStageColor(COLOR_DRYING);
            }
            break;
            
        case DRYING:
            if (currentTemp >= 160) {
                currentStage = MAILLARD;
                stageStartTime = millis();
                display->setStageColor(COLOR_MAILLARD);
            }
            break;
            
        case MAILLARD:
            if (currentTemp >= 200) {
                currentStage = FIRST_CRACK;
                stageStartTime = millis();
                display->setStageColor(COLOR_FIRST_CRACK);
            }
            break;
            
        case FIRST_CRACK:
            if (stageTime >= 30) { // After 30 seconds in first crack
                currentStage = DEVELOPMENT;
                stageStartTime = millis();
                display->setStageColor(COLOR_DEVELOPMENT);
            }
            break;
            
        default:
            break;
    }
}

void RoasterControl::handleEmergencyStop() {
    // Cut power to heater
    analogWrite(HEAT_PIN, 0);
    // Set fan to full for cooling
    analogWrite(FAN_PIN, 255);
    
    currentStage = EMERGENCY_STOP;
    display->showWarning("EMERGENCY STOP!");
    
    // Reset control values
    heatPower = 0;
    fanSpeed = 255;
    targetTemp = 0;
}

void RoasterControl::startRoast(bool useProfile) {
    if (currentStage == IDLE || currentStage == EMERGENCY_STOP) {
        manualMode = !useProfile;
        currentStage = CHARGING;
        roastStartTime = millis();
        stageStartTime = roastStartTime;
        
        // Initial settings
        fanSpeed = 128; // 50% fan to start
        targetTemp = 100; // Initial target for charging
        
        display->setStageColor(COLOR_DRYING);
        display->clearWarning();
    }
}

void RoasterControl::stopRoast() {
    if (currentStage != IDLE) {
        // Cut heat
        analogWrite(HEAT_PIN, 0);
        // Full fan for cooling
        analogWrite(FAN_PIN, 255);
        
        currentStage = COOLING;
        fanSpeed = 255;
        heatPower = 0;
        
        // After temperature drops below 50°C, stop completely
        if (tempControl->getAverageTemp() < 50) {
            currentStage = IDLE;
            analogWrite(FAN_PIN, 0);
            fanSpeed = 0;
        }
    }
}

void RoasterControl::adjustFan(int8_t adjustment) {
    if (manualMode && currentStage != IDLE && currentStage != EMERGENCY_STOP) {
        int16_t newSpeed = fanSpeed + adjustment;
        fanSpeed = constrain(newSpeed, 0, 255);
        analogWrite(FAN_PIN, fanSpeed);
    }
}

void RoasterControl::adjustHeat(int8_t adjustment) {
    if (manualMode && currentStage != IDLE && currentStage != EMERGENCY_STOP) {
        int16_t newTarget = targetTemp + adjustment;
        targetTemp = constrain(newTarget, 0, MAX_TEMP);
    }
}

void RoasterControl::toggleManualMode() {
    if (currentStage != IDLE && currentStage != EMERGENCY_STOP) {
        manualMode = !manualMode;
    }
}

void RoasterControl::logRoastData() {
    // Log data every LOG_INTERVAL milliseconds
    static unsigned long lastLog = 0;
    unsigned long now = millis();
    
    if (now - lastLog >= LOG_INTERVAL) {
        // If in profile mode, store current values for future replay
        if (!manualMode) {
            unsigned long roastTime = (now - roastStartTime) / 1000;
            profiles->updateProfilePoint(roastTime, 
                                      tempControl->getAverageTemp(),
                                      fanSpeed);
        }
        lastLog = now;
    }
}