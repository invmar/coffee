#include "TempControl.h"

/**
 * Constructor: Initialize temperature control system
 * Sets up initial values for temperature tracking
 */
TempControl::TempControl(MAX6675 *s1, MAX6675 *s2) {
    sensor1 = s1;
    sensor2 = s2;
    lastTemp1 = 0;
    lastTemp2 = 0;
    lastRoR = 0;
    lastTempTime = 0;
}

/**
 * Initialize the temperature sensors
 * Includes warm-up delay and initial readings
 */
void TempControl::begin() {
    delay(500);  // Allow MAX6675 sensors to stabilize
    lastTemp1 = readTemp1();
    lastTemp2 = readTemp2();
    lastTempTime = millis();
}

/**
 * Read temperature from sensor 1
 * @return Temperature in Celsius from primary sensor
 */
float TempControl::readTemp1() {
    return sensor1->readCelsius();
}

/**
 * Read temperature from sensor 2
 * @return Temperature in Celsius from secondary sensor
 */
float TempControl::readTemp2() {
    return sensor2->readCelsius();
}

/**
 * Calculate average temperature from both sensors
 * @return Average temperature in Celsius
 */
float TempControl::getAverageTemp() {
    float temp1 = readTemp1();
    float temp2 = readTemp2();
    return (temp1 + temp2) / 2.0;
}

/**
 * Calculate Rate of Rise (RoR)
 * Measures temperature change rate over time
 * @return Rate of temperature change in °C/second
 */
float TempControl::getRateOfRise() {
    float currentTemp = getAverageTemp();
    unsigned long currentTime = millis();
    float timeDiff = (currentTime - lastTempTime) / 1000.0; // Convert to seconds
    
    // Update RoR calculation every second
    if (timeDiff >= 1.0) {
        // Calculate temperature change rate
        lastRoR = (currentTemp - ((lastTemp1 + lastTemp2) / 2.0)) / timeDiff;
        // Update historical values
        lastTemp1 = readTemp1();
        lastTemp2 = readTemp2();
        lastTempTime = currentTime;
    }
    
    return lastRoR;
}

/**
 * Safety check for maximum temperature
 * @return true if temperature is below MAX_TEMP, false if exceeded
 */
bool TempControl::checkSafety() {
    float temp = getAverageTemp();
    return temp < MAX_TEMP;
}
