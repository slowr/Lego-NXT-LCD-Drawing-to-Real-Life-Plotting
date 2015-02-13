#include "aic.h"

void		AICInit(unsigned int irqid, unsigned int srctype, unsigned int prior, void (*handler)(void)){
	*AT91C_AIC_IDCR = 1 << irqid;
	AT91C_AIC_SVR[irqid] = (unsigned int) handler;
	AT91C_AIC_SMR[irqid] = srctype | prior;
	*AT91C_AIC_ICCR = 1 << irqid;
	*AT91C_AIC_IECR = 1 << irqid;
}

void		AICInterruptEnable(unsigned int irqid){
	*AT91C_AIC_IECR = 1 << irqid;
}

void		AICInterruptDisable(unsigned int irqid){
	*AT91C_AIC_IDCR = 1 << irqid;
	*AT91C_AIC_ICCR = 1 << irqid;
}
