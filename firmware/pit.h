#ifndef   PIT
#define   PIT

#include <stdconst.h>
#include "AT91SAM7S256.h"
#include "aic.h"
#include "d_output.h"
#include "i2c.h"
#include "display.h"

extern ULONG timer;
__ramfunc void pit_handler(void);

void	  PITinit(ULONG);
void      PITEnable(void);
void      PITDisable(void);
ULONG     PITRead(void);
ULONG     PITReadReset(void);
void      PITInterruptEnable(void (*handler)(void));
void      PITInterruptDisable(void);
void      PITAckInterrupt(void);
UWORD	  PITTicks2ms(ULONG ticks);
UWORD	  PITTicks2s(ULONG ticks);
void 	  spindelayms(ULONG ms);

#endif
