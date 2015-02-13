#include "input.h"
#include "display.h"
#include "arm2avr.h"
#include "pit.h"

static enum input_t input_devices[4] = {
  NO_DEVICE,
  NO_DEVICE,
  NO_DEVICE,
  NO_DEVICE
};

void InputInit(void)
{
  input_devices[0] = LIGHT_SENSOR;
  input_devices[1] = NO_DEVICE;
  input_devices[2] = TOUCH_SENSOR;
  input_devices[3] = MICROPHONE;
}

void InputExit(void)
{
  input_devices[0] = NO_DEVICE;
  input_devices[1] = NO_DEVICE;
  input_devices[2] = NO_DEVICE;
  input_devices[3] = NO_DEVICE;
}

void InputGetSensorValue(UWORD *value, UBYTE port)
{
  switch (input_devices[port]) {
  case LIGHT_SENSOR:
    *value = IoFromAvr.AdValue[port];
    DisplayString(0, 0x28, (UBYTE *) "Light:");
    printHexWord(50, 0x28, *value);
    break;
  case MICROPHONE:
    *value = IoFromAvr.AdValue[port];
    DisplayString(0, 0x30, (UBYTE *) "Sound:");
    printHexWord(50, 0x30, *value);
    break;
  case TOUCH_SENSOR:
    *value = IoFromAvr.AdValue[port];
    DisplayString(0, 0x38, (UBYTE *) "Touch:");
    printHexWord(50, 0x38, *value);
    break;
  default:
    *value = 0;
    break;
  }
 
}

UBYTE InputIsTouchSensorPressed(void)
{
    UBYTE i;
    
#define TOUCH_SENSOR_PRESSED 0xB6
    
    for (i = 0; i < 4; i++)
        if (input_devices[i] == TOUCH_SENSOR &&
            IoFromAvr.AdValue[i] == TOUCH_SENSOR_PRESSED)
            return TRUE;

    return FALSE;
}

UBYTE inputIsPosEdgeTouchSensor(void) {
    static UBYTE PrevTouchSensorStatus = 0;
    
    if (PrevTouchSensorStatus) {
        PrevTouchSensorStatus = 0;
        return FALSE;
    } else if (InputIsTouchSensorPressed()) {
        PrevTouchSensorStatus = 1;
        return TRUE;
    }
    
    return FALSE;
}

UBYTE inputIsTouchSensorPressed_Sampled(void)
{
#define TOUCHSNSR_SMPL_INTRVL 50
    
    return ((timer % TOUCHSNSR_SMPL_INTRVL) ? 0 : InputIsTouchSensorPressed());
}

