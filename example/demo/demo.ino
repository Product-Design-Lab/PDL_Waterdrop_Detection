#include <Arduino.h>
#include "WaterdropSensor.h"

APDS9960 apds(Wire, -1);
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

    dropSensor.setDebug(WaterdropSensor::DEBUG_INFO);
    dropSensor.setCrossCountTrigThreshold(4);
    dropSensor.setDropDetectedCallback(dropDetected, nullptr);
    dropSensor.init();
}

int previousCount = 0;

void loop()
{
    if (Serial.available())
    {
        char c = Serial.peek();
        if (c == 'p')
        {
            Serial.println("pause");
            dropSensor.pause();
        }
        else if (c == 'r')
        {
            Serial.println("resume");
            dropSensor.resume();
        }
        else if(c== 'd')
        {
            Serial.println("delete");
            dropSensor.deinit();
        }
        else if(c == 'i')
        {
            Serial.println("init");
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
    Serial.println("running");
    delay(1000);
}
