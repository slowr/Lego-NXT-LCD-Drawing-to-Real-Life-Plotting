#include "pit.h"

ULONG timer = 0;

__ramfunc void pit_handler(){
	static UBYTE counter = 0;
	timer++;
	dOutputCtrl();
	if(counter++){
		I2CTransfer();
		counter = 0;
	}
	PITReadReset();
}

void	  PITinit(ULONG period){
	*AT91C_PITC_PIMR = (unsigned int) period << 0;
	PITEnable();
	PITInterruptEnable(pit_handler);
}

void      PITEnable(){
	*AT91C_PITC_PIMR |= AT91C_PITC_PITEN;
}

void      PITDisable(){
	*AT91C_PITC_PIMR &= ~AT91C_PITC_PITEN;
}

ULONG     PITRead(){
	return(*AT91C_PITC_PIIR);
}

ULONG     PITReadReset(){
	return(*AT91C_PITC_PIVR);
}

void      PITInterruptEnable(void (*handler)(void)){
	PITInterruptDisable();
	AICInit(AT91C_ID_SYS, AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED, AT91C_AIC_PRIOR_LOWEST, handler);
	*AT91C_PITC_PIMR |= AT91C_PITC_PITIEN;
}

void      PITInterruptDisable(){
	AICInterruptDisable(AT91C_ID_SYS);
	*AT91C_PITC_PIMR &= ~AT91C_PITC_PITIEN;
}
