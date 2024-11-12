#include "DisplayInterface.h"
#include <string.h>
#include <MCUFRIEND_kbv.h>  // Include the correct display library
#include "TempControl.h"

extern MCUFRIEND_kbv tft; // Reference the existing tft defined elsewhere

// Constructor initializes the display and touchscreen pointers
DisplayInterface::DisplayInterface(MCUFRIEND_kbv* display, TouchScreen* touchscreen) {
    tft = display;
    touch = touchscreen;
    isRoasting = false;
    historyIndex = 0;

    graphX = MARGIN;                                                                                                                         
    graphY = MARGIN;
    controlsX = GRAPH_WIDTH + (2 * MARGIN);
    controlsY = MARGIN;

    // Initialize buttons
    startButton = Button(controlsX, 30, 50, 25, false, "START");
    stopButton = Button(controlsX + 55, 30, 50, 25, false, "STOP");
    fanUpButton = Button(controlsX, 65, 30, 25, false, "F+");
    fanDownButton = Button(controlsX + 35, 65, 30, 25, false, "F-");
    heatUpButton = Button(controlsX, 100, 30, 25, false, "H+");
    heatDownButton = Button(controlsX + 35, 100, 30, 25, false, "H-");
    profileButton = Button(controlsX, 135, 50, 25, false, "PROF");
    settingsButton = Button(controlsX + 55, 135, 50, 25, false, "SET");

    // Initialize history arrays
    memset(tempHistory, 0, sizeof(tempHistory));
    memset(rorHistory, 0, sizeof(rorHistory));
}

// Initialize TFT screen and set up basic drawing
void DisplayInterface::begin() {
    uint16_t ID = tft->readID();  // Read the ID from the screen
    tft->begin(ID);  // Initialize the TFT display
    tft->setRotation(1);  // Set to landscape mode
    tft->fillScreen(TFT_BLACK);  // Clear the screen

    // Draw initial layout
    drawButtons();
    drawGraph();
    drawStatus();
}

// Update the display with new temperature, rate of rise, fan, and heat values
void DisplayInterface::update(float temp, float ror, uint8_t fan, uint8_t heat) {
    currentTemp = temp;
    rorValue = ror;
    fanSpeed = fan;
    heatPower = heat;

    // Update history arrays
    tempHistory[historyIndex] = temp;
    rorHistory[historyIndex] = ror;
    historyIndex = (historyIndex + 1) % GRAPH_WIDTH;

    // Refresh display
    drawGraph();
    drawStatus();
}

// Handle touch input from the touchscreen
int DisplayInterface::handleTouch() {
    TSPoint p = touch->getPoint();
    
    // Restore pins that are shared between touch and display
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

    // Check if touch pressure is within range
    if (p.z < MINPRESSURE || p.z > MAXPRESSURE) {
        return 0;
    }

    // Map touch coordinates to screen coordinates
    int16_t x = map(p.x, TS_LEFT, TS_RT, 0, tft->width());
    int16_t y = map(p.y, TS_TOP, TS_BOT, 0, tft->height());

    // Check which button was pressed
    Button* pressed = checkButtonPress(x, y);
    if (pressed) {
        if (pressed == &startButton) return 1;
        if (pressed == &stopButton) return 2;
        if (pressed == &fanUpButton) return 3;
        if (pressed == &fanDownButton) return 4;
        if (pressed == &heatUpButton) return 5;
        if (pressed == &heatDownButton) return 6;
        if (pressed == &profileButton) return 7;
        if (pressed == &settingsButton) return 8;
    }

    return 0;
}

// Draw the graph (temperature curve) on the screen
void DisplayInterface::drawGraph() {
    // Draw graph background
    tft->fillRect(graphX, graphY, GRAPH_WIDTH, GRAPH_HEIGHT, TFT_BLACK);

    // Draw grid
    for (int i = 0; i < GRAPH_WIDTH; i += 20) {
        tft->drawFastVLine(graphX + i, graphY, GRAPH_HEIGHT, TFT_DARKGREY);
    }
    for (int i = 0; i < GRAPH_HEIGHT; i += 20) {
        tft->drawFastHLine(graphX, graphY + i, GRAPH_WIDTH, TFT_DARKGREY);
    }

    // Draw temperature curve
    for (int i = 1; i < GRAPH_WIDTH; i++) {
        int idx = (historyIndex + i) % GRAPH_WIDTH;
        int prevIdx = (historyIndex + i - 1) % GRAPH_WIDTH;

        if (tempHistory[idx] > 0 && tempHistory[prevIdx] > 0) {
            int y1 = map(tempHistory[prevIdx], 0, MAX_TEMP, GRAPH_HEIGHT + graphY, graphY);
            int y2 = map(tempHistory[idx], 0, MAX_TEMP, GRAPH_HEIGHT + graphY, graphY);
            tft->drawLine(graphX + i - 1, y1, graphX + i, y2, TFT_YELLOW);
        }
    }
}

// Draw buttons on the screen
void DisplayInterface::drawButtons() {
    tft->setTextSize(1);

    // Function to draw individual button
    auto drawButton = [this](Button& btn, uint16_t color) {
        tft->fillRoundRect(btn.x, btn.y, btn.w, btn.h, 3, color);
        tft->drawRoundRect(btn.x, btn.y, btn.w, btn.h, 3, TFT_WHITE);

        // Center text
        int16_t x1, y1;
        uint16_t w, h;
        tft->getTextBounds(btn.label, 0, 0, &x1, &y1, &w, &h);
        int textX = btn.x + (btn.w - w) / 2;
        int textY = btn.y + (btn.h - h) / 2 + 4;

        tft->setCursor(textX, textY);
        tft->setTextColor(TFT_WHITE);
        tft->print(btn.label);
    };

    // Draw all buttons
    drawButton(startButton, isRoasting ? TFT_DARKGREY : TFT_GREEN);
    drawButton(stopButton, isRoasting ? TFT_RED : TFT_DARKGREY);
    drawButton(fanUpButton, TFT_BLUE);
    drawButton(fanDownButton, TFT_BLUE);
    drawButton(heatUpButton, TFT_RED);
    drawButton(heatDownButton, TFT_RED);
    drawButton(profileButton, TFT_PURPLE);
    drawButton(settingsButton, TFT_PURPLE);
}

// Draw the status information on the screen (temperature, fan speed, etc.)
void DisplayInterface::drawStatus() {
    // Clear status area
    tft->fillRect(controlsX, MARGIN, SCREEN_WIDTH - controlsX - MARGIN, 25, TFT_BLACK);

    // Draw current values
    tft->setTextSize(1);
    tft->setTextColor(TFT_WHITE);

    char buffer[30];

    // Temperature
    sprintf(buffer, "Temp: %.1fC", currentTemp);
    tft->setCursor(controlsX, MARGIN);
    tft->print(buffer);

    // RoR
    sprintf(buffer, "RoR: %.1fC/min", rorValue * 60);
    tft->setCursor(controlsX, MARGIN + 10);
    tft->print(buffer);

    // Fan and Heat
    sprintf(buffer, "Fan: %d%%", map(fanSpeed, 0, 255, 0, 100));
    tft->setCursor(controlsX + 55, MARGIN + 20);
    tft->print(buffer);
}

// Show warning message
void DisplayInterface::showWarning(const char* message) {
    tft->setTextColor(TFT_RED);
    tft->setTextSize(2);
    tft->setCursor(10, 10);
    tft->print(message);
}

// Clear warning message
void DisplayInterface::clearWarning() {
    tft->fillRect(0, 0, tft->width(), 20, TFT_BLACK);  // Clear warning area
}

// Set stage color
void DisplayInterface::setStageColor(uint16_t color) {
    tft->fillScreen(color);  // Set the entire screen to the given color
}

// Check button press
Button* DisplayInterface::checkButtonPress(int16_t x, int16_t y) {
    // Check if touch is inside the button area
    if (x >= startButton.x && x <= startButton.x + startButton.w && y >= startButton.y && y <= startButton.y + startButton.h) {
        return &startButton;
    }
    if (x >= stopButton.x && x <= stopButton.x + stopButton.w && y >= stopButton.y && y <= stopButton.y + stopButton.h) {
        return &stopButton;
    }
    if (x >= fanUpButton.x && x <= fanUpButton.x + fanUpButton.w && y >= fanUpButton.y && y <= fanUpButton.y + fanUpButton.h) {
        return &fanUpButton;
    }
    if (x >= fanDownButton.x && x <= fanDownButton.x + fanDownButton.w && y >= fanDownButton.y && y <= fanDownButton.y + fanDownButton.h) {
        return &fanDownButton;
    }
    if (x >= heatUpButton.x && x <= heatUpButton.x + heatUpButton.w && y >= heatUpButton.y && y <= heatUpButton.y + heatUpButton.h) {
        return &heatUpButton;
    }
    if (x >= heatDownButton.x && x <= heatDownButton.x + heatDownButton.w && y >= heatDownButton.y && y <= heatDownButton.y + heatDownButton.h) {
        return &heatDownButton;
    }
    if (x >= profileButton.x && x <= profileButton.x + profileButton.w && y >= profileButton.y && y <= profileButton.y + profileButton.h) {
        return &profileButton;
    }
    if (x >= settingsButton.x && x <= settingsButton.x + settingsButton.w && y >= settingsButton.y && y <= settingsButton.y + settingsButton.h) {
        return &settingsButton;
    }
    return nullptr;
}