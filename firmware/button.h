#ifndef   _BUTTON_H_
#define   _BUTTON_H_

#include <stdconst.h>

enum button_t {
  BUTTON_NONE,
  BUTTON_LEFT,
  BUTTON_ENTER,
  BUTTON_RIGHT,
  BUTTON_EXIT 
};

void   ButtonInit(void);
void   ButtonExit(void);
enum button_t ButtonRead(void);

enum button_t ButtonRead_Sampled(void);
UBYTE ButtonPosEdge(enum button_t);
UBYTE ButtonActivatedState(enum button_t);

extern enum button_t button_pressed;

#endif
