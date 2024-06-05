#include "WaterdropSensor.h"
#include "Adafruit_TinyUSB.h"

WaterdropSensor::WaterdropSensor(APDS9960 &apds)
    : debounceWindowSize(DEFAULT_DEBOUNCE_WINDOW_SIZE),
      crossingCountTrigThreshhold(DEFAULT_CROSS_COUNT_TRIG_THRESHOLD),
      debugFlag(DEBUG_INFO),
      dropCount(0),
      dropDetectionTaskHandle(nullptr),
      X_FREQUENCY(pdMS_TO_TICKS(20)),
      xLastWakeTime(0),
      dropDetectedCallback(nullptr),
      callbackContext(nullptr),
      APDS(apds)
{
}

WaterdropSensor::~WaterdropSensor()
{
    deinit();
}

void WaterdropSensor::init(uint32_t priority)
{
    eTaskState task_state = eTaskGetState(dropDetectionTaskHandle);
    if (dropDetectionTaskHandle == nullptr || task_state == eInvalid || task_state == eDeleted)
    {
        if (!APDS.begin())
        {
            debugPrintf("Error initializing APDS-9960 sensor!\n");
            return;
        }
        else
        {
            debugPrintf("APDS-9960 sensor initialized\n");
        }
        dropDetectionTaskHandle = xTaskCreateStatic(
            dropDetectionTask, "DropDetectionTask", TASK_STACK_SIZE, this, priority, xStack, &xTaskBuffer);
    }
    else
    {
        debugPrintf("Drop detection task already created\n");
    }
}

void WaterdropSensor::deinit()
{
    eTaskState task_state = eTaskGetState(dropDetectionTaskHandle);
    if (task_state != eDeleted && task_state != eInvalid)
    {
        if (task_state != eSuspended)
        {
            APDS.end(); // end() is not idempotent, calling it a second time will cause a crash
        }
        vTaskDelete(dropDetectionTaskHandle);
        debugPrintf("Drop detection task deleted\n");
    }
    else
    {
        debugPrintf("Drop detection task already deleted\n");
    }
}

void WaterdropSensor::resume()
{
    eTaskState task_state = eTaskGetState(dropDetectionTaskHandle);
    if (task_state == eSuspended)
    {
        APDS.begin();
        vTaskResume(dropDetectionTaskHandle);
        debugPrintf("Resuming drop detection task\n");
    }
    else if (task_state == eRunning || task_state == eReady || task_state == eBlocked)
    {
        debugPrintf("drop detection task already running\n");
    }
    else
    {
        debugPrintf("drop detection task in state %d, cannot resume\n", (int)task_state);
    }
}

void WaterdropSensor::pause()
{
    eTaskState task_state = eTaskGetState(dropDetectionTaskHandle);
    if (task_state == eDeleted || task_state == eInvalid)
    {
        debugPrintf("Drop detection task not created\n");
        return;
    }
    else if (task_state == eSuspended)
    {
        debugPrintf("drop detection task already suspended\n");
        return;
    }
    else
    {
        APDS.end();
        vTaskSuspend(dropDetectionTaskHandle);
        debugPrintf("Suspending drop detection task\n");
    }
}

int WaterdropSensor::getDropCount() const
{
    return dropCount;
}

void WaterdropSensor::setDropCount(int count)
{
    dropCount = count;
}

void WaterdropSensor::setDebug(uint8_t debug)
{
    if (debug >= DEBUG_NONE && debug < DEBUG_MAX)
    {
        debugFlag = debug;
        const char *debugMessages[] = {
            "Debug mode: NONE",
            "Debug mode: INFO",
            "Debug mode: RAW",
            "Debug mode: CALIB",
            "Debug mode: ZEROING",
            "Debug mode: LOWPASS",
            "Debug mode: DOT",
            "Debug mode: LR",
            "Debug mode: CROSSING_STATE_PRINT",
            "Debug mode: CROSSING_STATE_PLOT",
            "Debug mode: FREQ"};
        Serial.println(debugMessages[debugFlag]);
    }
    else
    {
        debugFlag = DEBUG_NONE;
    }
}

void WaterdropSensor::setCrossCountTrigThreshold(int threshold)
{
    crossingCountTrigThreshhold = threshold;
}

void WaterdropSensor::setDebouceWindowSize(uint8_t count)
{
    debounceWindowSize = count;
}

void WaterdropSensor::setLoopDelayMs(uint32_t ms)
{
    X_FREQUENCY = pdMS_TO_TICKS(ms);
}

void WaterdropSensor::setBoundsLR(uint8_t bound)
{
    data.set_bounds_lr(static_cast<int>(bound), static_cast<int>(-bound));
}

void WaterdropSensor::setBoundsLP(uint8_t bound)
{
    data.u.set_bounds_lp(static_cast<int>(bound), static_cast<int>(-bound));
    data.d.set_bounds_lp(static_cast<int>(bound), static_cast<int>(-bound));
    data.l.set_bounds_lp(static_cast<int>(bound), static_cast<int>(-bound));
    data.r.set_bounds_lp(static_cast<int>(bound), static_cast<int>(-bound));
}

void WaterdropSensor::setBoundsDot(uint8_t bound)
{
    data.u.set_bounds_dot(static_cast<int>(bound), static_cast<int>(-bound));
    data.d.set_bounds_dot(static_cast<int>(bound), static_cast<int>(-bound));
    data.l.set_bounds_dot(static_cast<int>(bound), static_cast<int>(-bound));
    data.r.set_bounds_dot(static_cast<int>(bound), static_cast<int>(-bound));
}

void WaterdropSensor::setDropDetectedCallback(DropDetectedCallback callback, void *context)
{
    dropDetectedCallback = callback;
    callbackContext = context;
}

void WaterdropSensor::printDebug()
{
    switch (debugFlag)
    {
    case DEBUG_RAW:
        data.printRaw();
        break;
    case DEBUG_CALIB:
        data.printCalib();
        break;
    case DEBUG_ZEROING:
        data.printRaw_i32();
        break;
    case DEBUG_LOWPASS:
        data.printLP();
        break;
    case DEBUG_DOT:
        data.printDot();
        break;
    case DEBUG_LR:
        data.printLR();
        break;
    case DEBUG_CROSSING_STATE_PRINT:
        data.printCrossingState(crossing_state.state);
        break;
    case DEBUG_CROSSING_STATE_PLOT:
        data.plotCrossingState(crossing_state.state);
        break;
    case DEBUG_FREQ:
        Serial.printf("xLastWakeTime=%d, sample_count=%d\n", xLastWakeTime, data.sample_count);
        break;
    default:
        break;
    }
}

void WaterdropSensor::debugPrintf(const char *format, ...)
{
    if (debugFlag == DEBUG_INFO)
    {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}

void WaterdropSensor::dropDetectionTask(void *pvParameters)
{
    WaterdropSensor *sensor = static_cast<WaterdropSensor *>(pvParameters);
    while (true)
    {
        sensor->runMainTaskLogic();
    }
}

void WaterdropSensor::runMainTaskLogic()
{
    static int _debounce_sample_count = 0;

    vTaskDelayUntil(&xLastWakeTime, X_FREQUENCY);

    data.sample_count = APDS.gestureAvailable(data.u.get_raw_u8(), data.d.get_raw_u8(), data.l.get_raw_u8(), data.r.get_raw_u8());
    data.process_all_channel();
    crossing_state.state = data.get_crossing_state().state;

    printDebug();

    _debounce_sample_count += data.sample_count;

    if (_debounce_sample_count >= debounceWindowSize)
    {
        uint8_t dot_crossing_count = crossing_state.u.DOT_CROSS_UPPER_BOUND + crossing_state.u.DOT_CROSS_LOWER_BOUND +
                                     crossing_state.d.DOT_CROSS_UPPER_BOUND + crossing_state.d.DOT_CROSS_LOWER_BOUND +
                                     crossing_state.l.DOT_CROSS_UPPER_BOUND + crossing_state.l.DOT_CROSS_LOWER_BOUND +
                                     crossing_state.r.DOT_CROSS_UPPER_BOUND + crossing_state.r.DOT_CROSS_LOWER_BOUND;

        if (dot_crossing_count > DEFAULT_CROSS_COUNT_TRIG_THRESHOLD)
        {
            dropCount++;
            _debounce_sample_count = 0;
            crossing_state.state = 0;
            if (dropDetectedCallback)
            {
                dropDetectedCallback(callbackContext);
            }
        }
    }
}
