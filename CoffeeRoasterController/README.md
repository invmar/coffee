# Coffee Roaster Controller

Arduino-based coffee roaster controller with PID temperature control, profile management, and touch interface.

## Features
- Dual temperature sensor monitoring
- PID-controlled heating
- Fan speed control
- Multiple roasting stages
- Profile recording and playback
- Emergency stop functionality
- Touch screen interface
- Temperature graphing
- Rate of Rise (RoR) calculation

## Dependencies
- Adafruit ILI9341
- XPT2046_Touchscreen
- MAX6675
- PID
- SD

## Installation
1. Download this library
2. Install required dependencies
3. Place in your Arduino libraries folder
4. Restart Arduino IDE

## Hardware Setup
See RoasterConfig.h for pin configurations:
- Temperature sensors (MAX6675)
- Display (ILI9341)
- Touch screen (XPT2046)
- Heat control
- Fan control
- Emergency stop button

## Usage
```cpp
#include <CoffeeRoasterController.h>

// See examples/BasicRoaster for complete setup
```

## License
MIT License