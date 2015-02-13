#ifndef _DRAWING_H_
#define _DRAWING_H_

#include "AT91SAM7S256.h"
#include <stdconst.h>
#include "helpfunc.h"
#include "display.h"

typedef enum draw_state {
  DRAW_LINE,
  DRAW_ANGLE
} draw_state;

#define PROJECT_SIZE 10

ULONG lineWindow[PROJECT_SIZE];
ULONG angleWindow[PROJECT_SIZE];

void initializeWindow(void);
UBYTE startDrawing(void);
UBYTE setLineAngle(ULONG centimetre,ULONG degree);

#endif
