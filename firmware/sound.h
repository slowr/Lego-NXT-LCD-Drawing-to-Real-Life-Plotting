#ifndef   SOUND
#define   SOUND

#include <stdconst.h>
#include "AT91SAM7S256.h"
#include "aic.h"
#include "display.h"

void SoundInit(void);
void SoundEnable(void);
void SoundIntEnable(void (*handler)(void));
void SoundDisable(void);
void SoundIntDisable(void);
void SoundSync(ULONG *pattern, UBYTE length, UBYTE rate, UWORD duration);
void SoundAsync(ULONG *pattern, UBYTE length, UBYTE rate, UWORD duration);
void SoundExit(void);
void ssc_handler(void);

#endif
