#ifndef _HELP_FUNCS_H_
#define _HELP_FUNCS_H_

#include "AT91SAM7S256.h"
#include  <stdconst.h>
#include  "d_output.h"
#include  "arm2avr.h"
#include  "display.h"
#include  "pit.h"

void draw_line(ULONG *state, ULONG centimetre);
void calculate_angle(ULONG *state, ULONG degree);
void drop_stylo(void);
void raise_stylo(void);
void go_backwards(ULONG centimetre);
void go_forwards(ULONG centimetre);
void rotate_left(ULONG degree);
void rotate_right(ULONG degree);
ULONG dTimerRead(void);

#endif
