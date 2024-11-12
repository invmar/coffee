#ifndef DISPLAY_INTERFACE_H
#define DISPLAY_INTERFACE_H

#include <stdint.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include "RoasterConfig.h"
#include <LiquidCrystal.h>

// Button structure definition
struct Button {
    int x;
    int y;
    int w;
    int h;
    bool pressed;
    char label[15];
    
    Button() : x(0), y(0), w(0), h(0), pressed(false) {
        label[0] = '\0';
    }
    
    Button(int _x, int _y, int _w, int _h, bool _pressed, const char* _label) 
        : x(_x), y(_y), w(_w), h(_h), pressed(_pressed) {
        strncpy(label, _label, 14);
        label[14] = '\0';
    }
};

class DisplayInterface {
    private:
        MCUFRIEND_kbv* tft;         // Pointer to TFT display
        TouchScreen* touch;         // Pointer to touchscreen

        // Screen areas (for UI layout)
        int graphX, graphY;
        int controlsX, controlsY;

        // Touch pressure thresholds
        const int MINPRESSURE = 200;
        const int MAXPRESSURE = 1000;

        // Button objects for the display
        Button startButton;
        Button stopButton;
        Button fanUpButton;
        Button fanDownButton;
        Button heatUpButton;
        Button heatDownButton;
        Button profileButton;
        Button settingsButton;

        // Current values to display
        float currentTemp;
        float targetTemp;
        float rorValue;
        uint8_t fanSpeed;
        uint8_t heatPower;
        bool isRoasting;

        // Graph data arrays
        float tempHistory[GRAPH_WIDTH];
        float rorHistory[GRAPH_WIDTH];
        int historyIndex;

        // Internal helper functions
        void drawGraph();
        void drawButtons();
        void drawStatus();
        
        // Check for button press
        Button* checkButtonPress(int16_t x, int16_t y);

    public:
        // Constructor, accepts pointers to the TFT display and touchscreen
        DisplayInterface(MCUFRIEND_kbv* display, TouchScreen* touchscreen);

        // Public methods
        void begin();               // Initialize display
        void update(float temp, float ror, uint8_t fan, uint8_t heat); // Update display values
        int handleTouch();          // Handle touch events
        void setStageColor(uint16_t color); // Change the color of the stage
        void showWarning(const char* message); // Show a warning message on the display
        void clearWarning();        // Clear any warning message on the display
};

#endif