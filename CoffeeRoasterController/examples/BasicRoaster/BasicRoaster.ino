#include <CoffeeRoasterController.h>

#define SD_CS 53  // SD card chip select pin (for Mega 2560)

// Initialize display
MCUFRIEND_kbv tft;

// Touch screen configuration
const int MINPRESSURE = 200;
const int MAXPRESSURE = 1000;

// Create touch screen instance using pins defined in RoasterConfig.h
TouchScreen touch(XP, YP, XM, YM, 300);

// Temperature sensors
MAX6675 sensor1(TEMP1_SCK, TEMP1_CS, TEMP1_SO);
MAX6675 sensor2(TEMP2_SCK, TEMP2_CS, TEMP2_SO);

// Component instances
TempControl* tempControl = nullptr;
PIDController* pidControl = nullptr;
DisplayInterface* display = nullptr;
ProfileManager* profiles = nullptr;
RoasterControl* roaster = nullptr;

void setup() {
    // Initialize serial for debugging
    Serial.begin(115200);
    
    // Initialize communication buses
    SPI.begin();
    Wire.begin();

    // Initialize the TFT display
    uint16_t ID = tft.readID();
    tft.begin(ID);
    tft.setRotation(1); // Landscape mode
    
    // Initialize SD card
    if (!SD.begin(SD_CS)) {
        Serial.println("SD Card initialization failed!");
        return;
    }
    Serial.println("SD Card initialized successfully");
    
    // Initialize components in correct order
    tempControl = new TempControl(&sensor1, &sensor2);
    pidControl = new PIDController();
    display = new DisplayInterface(&tft, &touch);
    profiles = new ProfileManager();
    
    // Create roaster control last since it depends on other components
    roaster = new RoasterControl(tempControl, pidControl, display, profiles);
    
    // Initialize roaster control system
    roaster->begin();
}

void loop() {
    // Main control loop
    roaster->update();
    
    // Handle touch input if available
    TSPoint p = touch.getPoint();
    
    // Restore pins that are shared between touch and display
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
        int command = display->handleTouch();
        
        switch (command) {
            case 1: // Start
                roaster->startRoast(false);
                break;
            case 2: // Stop
                roaster->stopRoast();
                break;
            case 3: // Fan Up
                roaster->adjustFan(10);
                break;
            case 4: // Fan Down
                roaster->adjustFan(-10);
                break;
            case 5: // Heat Up
                roaster->adjustHeat(5);
                break;
            case 6: // Heat Down
                roaster->adjustHeat(-5);
                break;
            case 7: // Profile Mode
                roaster->toggleManualMode();
                break;
        }
    }
    
    // Small delay to prevent overwhelming the system
    delay(10);
}