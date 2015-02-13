#ifndef _LED_H_
#define _LED_H_

#include "button.h"

/* use LED(port, 0)/LED(port, 1) to turn led off/on at port */
void LedSwitchOn(UBYTE port);
void LedSwitchOff(UBYTE port);
void LedReactOnButtonPressed(enum button_t);


#endif
