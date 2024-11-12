#ifndef COFFEE_ROASTER_CONTROLLER_H
#define COFFEE_ROASTER_CONTROLLER_H

// Include all required Arduino libraries first
#include <SPI.h>
#include <SD.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <max6675.h>
#include <Wire.h>
#include <PID_v1.h>
#include <Adafruit_GFX.h>
#include <TouchScreen.h>
#include <LiquidCrystal.h>
#include <Arduino.h>

// Include our component headers in correct dependency order
#include "RoasterConfig.h"
#include "TempControl.h"
#include "PIDController.h"
#include "DisplayInterface.h"
#include "ProfileManager.h"
#include "RoasterControl.h"

#endif