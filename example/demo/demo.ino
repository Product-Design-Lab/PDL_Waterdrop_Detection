#include "WaterdropSensor.h"
#include <Arduino.h>

APDS9960 apds(Wire, -1);
WaterdropSensor dropSensor(apds);

void dropDetected(void *context)
{
    static int count = 0;
    Serial.printf("drop detected %d\n", count++);
}

void ApdsEventCallback(APDS_Data::data_crossing_state_t state)
{
    static APDS_Data::data_crossing_state_t state_prev = state;
    static int pair_crossing_count_prev = 0, dot_crossing_count_prev = 0, lp_crossing_count_prev = 0;
    int pair_crossing_count = APDS_Data::sum_pair_cross_count(state);
    int dot_crossing_count = APDS_Data::sum_dot_cross_count(state);
    int lp_crossing_count = APDS_Data::sum_lp_cross_count(state);

    // tune the threshold to get the desired sensitivity
    if (lp_crossing_count >= 1 && lp_crossing_count_prev == 0)
    {
        Serial.printf("near drop from Low Pass data: %d\n", lp_crossing_count);
    }
    if (pair_crossing_count >= 1 && pair_crossing_count_prev == 0)
    {
        Serial.printf("near drop from Pair data: %d\n", pair_crossing_count);
    }
    if (dot_crossing_count >= 1 && dot_crossing_count_prev == 0)
    {
        Serial.printf("near drop from Dot data: %d\n", dot_crossing_count);
    }

    pair_crossing_count_prev = pair_crossing_count;
    dot_crossing_count_prev = dot_crossing_count;
    lp_crossing_count_prev = lp_crossing_count;
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
    dropSensor.setSensorDataCrossingCallback(ApdsEventCallback);
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
        else if (c == 'd')
        {
            Serial.println("delete");
            dropSensor.deinit();
        }
        else if (c == 'i')
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
