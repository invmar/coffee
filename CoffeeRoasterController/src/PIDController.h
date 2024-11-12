#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include <PID_v1.h>  // Make sure this matches the installed library name
#include "RoasterConfig.h"

/**
 * @class PIDController
 * @brief Manages PID control with dual tuning modes
 * 
 * Implements PID control with switchable aggressive and conservative
 * tuning parameters for optimal temperature control during different
 * phases of the roast.
 */
class PIDController {
    private:
        double input;     // Current temperature input
        double output;    // Calculated PID output
        double setpoint;  // Target temperature
        PID *pid;        // PID controller instance
        bool aggressive;  // Current PID mode flag
        
    public:
        /**
         * @brief Constructor - Creates PID controller with default settings
         */
        PIDController();
        
        /**
         * @brief Initialize PID controller
         */
        void begin();
        
        /**
         * @brief Calculate new PID output
         */
        void compute();
        
        /**
         * @brief Set target temperature
         * @param sp Desired temperature setpoint
         */
        void setSetpoint(double sp);
        
        /**
         * @brief Switch between aggressive and conservative PID modes
         * @param agg true for aggressive, false for conservative
         */
        void switchToAggressive(bool agg);
        
        /**
         * @brief Get current PID output value
         * @return Current output (0-255)
         */
        double getOutput();
        
        /**
         * @brief Update PID tuning parameters
         * @param kp Proportional gain
         * @param ki Integral gain
         * @param kd Derivative gain
         */
        void tune(double kp, double ki, double kd);
};

#endif