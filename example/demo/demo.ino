#include <Arduino.h>
#include "WaterdropSensor.h"

APDS9960 apds(Wire, 9);
WaterdropSensor dropSensor(apds);

void dropDetected(void *context)
{
    static int count = 0;
    Serial.printf("drop detected %d\n", count++);
}

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        ; // Wait for serial port to connect
    }

    dropSensor.init();
    dropSensor.setDropDetectedCallback(dropDetected, nullptr);
}

int previousCount = 0;

void loop()
{
    if (Serial.available())
    {
        char c = Serial.peek();
        if (c == 'p')
        {
            Serial.println("paused");
            dropSensor.deinit();
        }
        else if (c == 'r')
        {
            Serial.println("resumed");
            dropSensor.init();
        }
        else
        {
            int num = Serial.parseInt();
            dropSensor.setDebug(static_cast<uint8_t>(num));
        }
        Serial.read();
    }

    // int count = dropSensor.getDropCount();
    // if (count != previousCount)
    // {
    //     Serial.printf("dropCount=%d\n", count);
    //     previousCount = count;
    // }

    delay(200);
}
