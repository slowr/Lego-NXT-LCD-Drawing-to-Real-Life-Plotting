#include "sound.h"

void SoundInit(){
  SoundIntDisable();
  SoundDisable();
  *AT91C_PMC_PCER   = (1L << AT91C_ID_SSC); /* Enable MCK clock   */
  *AT91C_PIOA_ODR   = AT91C_PA17_TD;
  *AT91C_PIOA_OWDR  = AT91C_PA17_TD;
  *AT91C_PIOA_MDDR  = AT91C_PA17_TD;
  *AT91C_PIOA_PPUDR = AT91C_PA17_TD;
  *AT91C_PIOA_IFDR  = AT91C_PA17_TD;
  *AT91C_PIOA_CODR  = AT91C_PA17_TD;
  *AT91C_PIOA_IDR   = AT91C_PA17_TD;
  *AT91C_SSC_CR   = AT91C_SSC_SWRST;
  *AT91C_SSC_TCMR = AT91C_SSC_CKS_DIV + 
                    AT91C_SSC_CKO_CONTINOUS + AT91C_SSC_START_CONTINOUS;
  *AT91C_SSC_TFMR = (-1)+(((sizeof(ULONG)*8) & 0xF) << 8) + AT91C_SSC_MSBF;
  *AT91C_SSC_CR   = AT91C_SSC_TXEN;         /* TX enable */
}

void SoundEnable(){
  *AT91C_PIOA_PDR = AT91C_PA17_TD;
}

void SoundDisable(){
  *AT91C_PIOA_PER = AT91C_PA17_TD;
}

void SoundIntEnable(void (*handler)(void)){
  SoundIntDisable();
  AICInit(AT91C_ID_SSC, AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED, AT91C_AIC_PRIOR_LOWEST, handler);
  *AT91C_SSC_IER = AT91C_SSC_ENDTX;
}

void SoundIntDisable(){
  AICInterruptDisable(AT91C_ID_SSC);
  *AT91C_SSC_IDR = 0xFFFF;
}

void SoundSync(ULONG *pattern, UBYTE length, UBYTE rate, UWORD duration) {
  UBYTE i=0;
  SoundEnable();
  *AT91C_SSC_CMR = -4700*rate + 1200000;
  while(duration-- > 0){
    for(i=0; i<length; i++){
      while(!(*AT91C_SSC_SR & AT91C_SSC_TXRDY)){/* wait */;};
      *AT91C_SSC_THR = pattern[i];
    }
  }
  SoundDisable();
  return;
}

void SoundAsync(ULONG *pattern, UBYTE length, UBYTE rate, UWORD duration) {
  SoundEnable();
  *AT91C_SSC_CMR = -4700*rate + 1200000;
  
  *AT91C_SSC_TPR = (unsigned int) &pattern[0];
  *AT91C_SSC_TCR = sizeof(ULONG);

  if(length >= 2){
    *AT91C_SSC_TNPR = (unsigned int) &pattern[1];
    *AT91C_SSC_TNCR = sizeof(ULONG);
  }

  *AT91C_SSC_PTCR = AT91C_PDC_TXTEN;
  return;
}

void SoundExit(void)
{ }
