#ifndef TEMP_CONTROL_H
#define TEMP_CONTROL_H

#include <max6675.h>
#include "RoasterConfig.h" // Make sure this defines MAX_TEMP

/**
 * @class TempControl
 * @brief Manages dual temperature sensors and calculates rate of rise
 * 
 * This class handles temperature readings from two MAX6675 sensors,
 * provides averaging, and calculates the rate of temperature change.
 */
class TempControl {
    private:
        MAX6675 *sensor1;           // Primary temperature sensor
        MAX6675 *sensor2;           // Secondary temperature sensor
        float lastTemp1;            // Previous reading from sensor 1
        float lastTemp2;            // Previous reading from sensor 2
        float lastRoR;              // Last calculated Rate of Rise
        unsigned long lastTempTime; // Timestamp of last measurement
        
    public:
        /**
         * @brief Constructor initializing both temperature sensors
         * @param s1 Pointer to primary MAX6675 sensor
         * @param s2 Pointer to secondary MAX6675 sensor
         */
        TempControl(MAX6675 *s1, MAX6675 *s2);
        
        /**
         * @brief Initialize sensors and initial readings
         */
        void begin();
        
        /**
         * @brief Read temperature from sensor 1
         * @return Temperature in Celsius
         */
        float readTemp1();
        
        /**
         * @brief Read temperature from sensor 2
         * @return Temperature in Celsius
         */
        float readTemp2();
        
        /**
         * @brief Calculate average temperature from both sensors
         * @return Average temperature in Celsius
         */
        float getAverageTemp();
        
        /**
         * @brief Calculate rate of temperature change
         * @return Rate of Rise in °C/second
         */
        float getRateOfRise();
        
        /**
         * @brief Check if temperature is within safe limits
         * @return true if temperature is safe, false if exceeded
         */
        bool checkSafety();
};

#endif // TEMP_CONTROL_H
