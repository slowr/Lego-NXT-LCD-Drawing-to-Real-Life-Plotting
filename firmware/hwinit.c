#include "hwinit.h"

void HardwareInit(void){
  *AT91C_RSTC_RMR = 0xA5000401;         // configure reset controller mode register
  *AT91C_PMC_PCER = (1L<<AT91C_ID_PIOA);// enable peripheral clock for PIO
}
