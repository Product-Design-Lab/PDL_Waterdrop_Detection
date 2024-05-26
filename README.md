 # PDL_Waterdrop_Detection
 
 PDL_Waterdrop_Detection is an Arduino library that allows you to detect water drops using the APDS9960 sensor. This library is designed to work with a modified version of the Arduino_APDS9960 library.
 
 ## Features
 
 - Detect water drops
 - Measure color
 - Measure proximity
 
 ## Installation
 
 To use this library, you need to install the following dependencies:
 
 1. [Arduino_APDS9960](https://github.com/Product-Design-Lab/Arduino_APDS9960)
 2. [MovingAverage](https://github.com/pilotak/MovingAverage.git)
 
 ### Installing Dependencies
 
 - **Arduino_APDS9960**
   - Download or clone the modified version from [https://github.com/Product-Design-Lab/Arduino_APDS9960](https://github.com/Product-Design-Lab/Arduino_APDS9960).
   - Unzip the library and place it in the `libraries` folder of your Arduino sketchbook.
 
 - **MovingAverage**
   - Download or clone the library from [https://github.com/pilotak/MovingAverage.git](https://github.com/pilotak/MovingAverage.git).
   - Unzip the library and place it in the `libraries` folder of your Arduino sketchbook.
 
 ## Usage
 
 Include the library in your sketch and initialize it.
 
 ## API
 
 ### `PDL_Waterdrop_Detection`
 
 #### Constructor
 
 ```cpp
 PDL_Waterdrop_Detection(TwoWire& wire, int intPin);
 ```
 Creates an instance of the water drop detection on the specified wire and interrupt pin.
 
 #### Methods
 
 - `bool begin();`
   Initializes the sensor and checks its connection.
 - `void end();`
   Ends the sensor communication and disables all functionalities.
 - `void setPin(uint8_t pin, uint8_t mode);`
   Sets the pin and its mode (HIGH or LOW).
 - `void setDebounceTime(uint32_t debounceTime);`
   Sets the debounce time in milliseconds.
 - `void setLongPressTime(uint32_t longPressTime);`
   Sets the long press time in milliseconds.
 - `void setShortPressCallback(void (*shortPressCallback)());`
   Sets the callback function for short press detection.
 - `void setLongPressCallback(void (*longPressCallback)());`
   Sets the callback function for long press detection.
 - `void init();`
   Initializes the button.
 - `void update();`
   Updates the button state. Call this method in your `loop()`.
 - `uint8_t getState(int *shortPressCount, int *longPressCount);`
   Gets the current state of the button along with the counts of short and long presses.
 - `bool shortPress();`
   Returns `true` if a short press is detected.
 - `bool longPress();`
   Returns `true` if a long press is detected.
 - `bool setLEDBoost(uint8_t boost);`
   Sets the LED current boost value.
 - `void setGestureSensitivity(uint8_t sensitivity);`
   Sets the gesture sensitivity value.
 - `void setInterruptPin(int pin);`
   Sets the interrupt pin.
 - `bool setGestureIntEnable(bool en);`
   Enables or disables the gesture interrupt.
 - `bool setGestureMode(bool en);`
   Enables or disables the gesture mode.
 - `bool enablePower();`
   Enables the power for the sensor.
 - `bool disablePower();`
   Disables the power for the sensor.
 - `bool enableColor();`
   Enables the color detection.
 - `bool disableColor();`
   Disables the color detection.
 - `bool enableProximity();`
   Enables the proximity detection.
 - `bool disableProximity();`
   Disables the proximity detection.
 - `bool enableWait();`
   Enables the wait functionality.
 - `bool disableWait();`
   Disables the wait functionality.
 - `bool enableGesture();`
   Enables the gesture detection.
 - `bool disableGesture();`
   Disables the gesture detection.
 - `bool write(uint8_t val);`
   Writes a value to the sensor.
 - `bool write(uint8_t reg, uint8_t val);`
   Writes a value to a specific register on the sensor.
 - `bool read(uint8_t reg, uint8_t *val);`
   Reads a value from a specific register on the sensor.
 - `size_t readBlock(uint8_t reg, uint8_t *val, unsigned int len);`
   Reads a block of data from the sensor.
 - `int gestureFIFOAvailable();`
   Checks if the gesture FIFO has available data.
 - `int handleGesture();`
   Handles the gesture detection process.
 - `int gestureAvailable();`
   Checks if a gesture is available.
 - `int readGesture();`
   Reads the detected gesture.
 - `int colorAvailable();`
   Checks if color data is available.
 - `bool readColor(int& r, int& g, int& b);`
   Reads the color data.
 - `bool readColor(int& r, int& g, int& b, int& c);`
   Reads the color data with clear channel.
 - `int proximityAvailable();`
   Checks if proximity data is available.
 - `int readProximity();`
   Reads the proximity data.
 
 ## License
 
 This library is licensed under the GNU Lesser General Public License. See [LICENSE](LICENSE) for more details.
 
 ## Contributing
 
 Feel free to submit issues or pull requests for any bugs or feature requests.
 
 ## Credits
 
 Developed by Xuteng Lin.
