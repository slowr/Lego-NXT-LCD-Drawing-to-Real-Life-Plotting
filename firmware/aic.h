#ifndef AIC
#define AIC

#include <stdconst.h>
#include "AT91SAM7S256.h"

void		AICInit(unsigned int irqid, unsigned int srctype, unsigned int prior, void (*handler)(void));
void		AICInterruptEnable(unsigned int irqid);
void		AICInterruptDisable(unsigned int irqid);

#endif
