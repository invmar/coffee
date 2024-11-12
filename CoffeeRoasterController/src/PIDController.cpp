#include "PIDController.h"

/**
 * Constructor: Initialize PID controller
 * Sets up PID with conservative tuning parameters
 */
PIDController::PIDController() {
    input = 0;
    output = 0;
    setpoint = 0;
    // Create PID controller with conservative tuning
    pid = new PID(&input, &output, &setpoint, KP_CONS, KI_CONS, KD_CONS, DIRECT);
}

/**
 * Initialize PID controller
 * Sets up automatic mode and output limits
 */
void PIDController::begin() {
    pid->SetMode(AUTOMATIC);    // Enable automatic PID control
    pid->SetOutputLimits(0, 255); // Set PWM output range
    aggressive = false;         // Start with conservative tuning
}

/**
 * Calculate PID output
 * Should be called regularly in the main loop
 */
void PIDController::compute() {
    pid->Compute();
}

/**
 * Update temperature setpoint
 * @param sp New target temperature
 */
void PIDController::setSetpoint(double sp) {
    setpoint = sp;
}

/**
 * Switch between aggressive and conservative PID tuning
 * @param agg true for aggressive tuning, false for conservative
 */
void PIDController::switchToAggressive(bool agg) {
    if (agg != aggressive) {
        aggressive = agg;
        if (aggressive) {
            // Switch to aggressive tuning for faster response
            pid->SetTunings(KP_AGG, KI_AGG, KD_AGG);
        } else {
            // Switch to conservative tuning for stability
            pid->SetTunings(KP_CONS, KI_CONS, KD_CONS);
        }
    }
}

/**
 * Get current PID output value
 * @return Current output value (0-255)
 */
double PIDController::getOutput() {
    return output;
}

/**
 * Update PID tuning parameters
 * @param kp New proportional gain
 * @param ki New integral gain
 * @param kd New derivative gain
 */
void PIDController::tune(double kp, double ki, double kd) {
    pid->SetTunings(kp, ki, kd);
}