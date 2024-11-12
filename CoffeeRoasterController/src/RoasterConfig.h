#ifndef ROASTER_CONFIG_H
#define ROASTER_CONFIG_H

#include <stdint.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <SPI.h>
#include <Wire.h>      // this is needed even tho we aren't using it
#include <TouchScreen.h>
#include <LiquidCrystal.h>

// SD Card Configuration
#define SD_CS 53  // Chip Select for SD card on Mega 2560

// Forward declarations of all classes
class TempControl;
class PIDController;
class DisplayInterface;
class ProfileManager;
class RoasterControl;

//===========================================
// Hardware Pin Configurations
//===========================================

// Display Pins (MCUFRIEND Shield)
const int XP=7, XM=A1, YP=A2, YM=6; //240x320 ID=0x9341
const int TS_LEFT=937, TS_RT=157, TS_TOP=951, TS_BOT=181; //240x320 ID=0x9341

// MAX6675 Temperature Sensor Configuration
// Sensor 1 - Primary temperature sensor
#define TEMP1_CS  47    // Chip Select
#define TEMP1_SCK 46    // Clock
#define TEMP1_SO  45    // Serial Output (MISO)
// Sensor 2 - Secondary/verification sensor
#define TEMP2_CS  44    // Chip Select
#define TEMP2_SCK 43    // Clock
#define TEMP2_SO  42    // Serial Output (MISO)

// System Control Pins
#define HEAT_PIN  50     // PWM output for AC heating element control
#define FAN_PIN   52     // PWM output for DC fan speed control
#define EMERGENCY_STOP_PIN 18  // Emergency stop button input

//===========================================
// Display Colors (16-bit RGB565 format)
//===========================================
#define TFT_BLACK       0x0000
#define TFT_BLUE        0x001F
#define TFT_RED         0xF800
#define TFT_GREEN       0x07E0
#define TFT_CYAN        0x07FF
#define TFT_MAGENTA     0xF81F
#define TFT_YELLOW      0xFFE0
#define TFT_WHITE       0xFFFF
#define TFT_GREY        0x8410
#define TFT_DARKGREY    0x4208
#define TFT_PURPLE      0x780F

//===========================================
// Safety and Operating Parameters
//===========================================

// Temperature Safety Limits (in Celsius)
#define MAX_TEMP 290.0      // Absolute maximum allowed temperature
#define WARNING_TEMP 280.0  // Temperature to trigger warnings
#define MIN_TEMP 0.0        // Minimum valid temperature reading

// PID Control Parameters
// Aggressive tuning - Used when far from setpoint
#define KP_AGG 120.0    // Proportional gain
#define KI_AGG 30.0     // Integral gain
#define KD_AGG 60.0     // Derivative gain

// Conservative tuning - Used near setpoint
#define KP_CONS 70.0    // Proportional gain
#define KI_CONS 15.0    // Integral gain
#define KD_CONS 10.0    // Derivative gain

//===========================================
// System Control Parameters
//===========================================

// PWM Control Range
#define PWM_MAX 255     // Maximum PWM value (100% power)
#define PWM_MIN 0       // Minimum PWM value (0% power)

// Control System Parameters
#define TEMP_THRESHOLD 5.0   // Temperature difference threshold for PID switching
#define LOG_INTERVAL 1000    // Data logging interval in milliseconds

//===========================================
// Display Configuration
//===========================================

// Screen Dimensions and Layout
#define SCREEN_WIDTH 320     // Display width in pixels
#define SCREEN_HEIGHT 240    // Display height in pixels
#define GRAPH_WIDTH 200      // Width of temperature graph
#define GRAPH_HEIGHT 160     // Height of temperature graph
#define MARGIN 5            // General margin for UI elements

// Roasting Stage Colors
#define COLOR_DRYING      0x7BEF  // Light Green  - Drying/Green phase
#define COLOR_MAILLARD    0xFD20  // Light Orange - Maillard reaction phase
#define COLOR_FIRST_CRACK 0xF800  // Red         - First crack phase
#define COLOR_DEVELOPMENT 0xFFE0  // Yellow      - Development phase

//===========================================
// Profile Management
//===========================================

// Profile Storage Parameters
#define MAX_PROFILES 10              // Maximum number of stored profiles
#define PROFILE_NAME_LENGTH 20       // Maximum length of profile names

// Roast Profile Data Structure
struct RoastProfile {
    char name[PROFILE_NAME_LENGTH];  // Profile name/identifier
    float tempCurve[180];           // Temperature points (3 minutes in seconds)
    uint8_t fanCurve[180];          // Fan speed curve (0-255 for each second)
    uint8_t totalTime;              // Total roast duration in minutes
};

#endif // ROASTER_CONFIG_H
