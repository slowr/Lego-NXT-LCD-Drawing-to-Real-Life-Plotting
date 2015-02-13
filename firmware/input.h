#ifndef   _INPUT_H
#define   _INPUT_H

#include <stdconst.h>

enum input_t {
  NO_DEVICE,
  LIGHT_SENSOR,
  MICROPHONE,
  TOUCH_SENSOR
};

void InputInit(void);
void InputExit(void);
void InputGetSensorValue(UWORD *value, UBYTE port);
UBYTE InputIsTouchSensorPressed(void);
UBYTE inputIsPosEdgeTouchSensor(void);
UBYTE inputIsTouchSensorPressed_Sampled(void);

#endif
