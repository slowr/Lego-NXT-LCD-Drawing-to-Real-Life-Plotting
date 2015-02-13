#include "AT91SAM7S256.h"
#include  <stdconst.h>
#include  <string.h>
#include  <assert.h>
#include  "i2c.h"
#include  "arm2avr.h"
#include  "led.h"
#include  "display.h"
#include  "button.h"

#define BYTES_TO_TX     8
#define BYTES_TO_RX     12
#define TIMEOUT         2100
#define I2CClk          400000L
#define TIME400KHz      (((OSC/16L)/(I2CClk * 2)) + 1)
#define CLDIV           (((OSC/I2CClk)/2)-3)
#define DEVICE_ADR      0x01
#define COPYRIGHTSTRING "Let's samba nxt arm in arm, (c)LEGO System A/S"
#define COPYRIGHTSTRINGLENGTH 46
const   UBYTE CopyrightStr[] = {"\xCC"COPYRIGHTSTRING};
#define MAX(x,y) (((x)>(y))?(x):(y))
#define piir (*AT91C_PITC_PIIR & AT91C_PITC_CPIV)
#define pimr (*AT91C_PITC_PIMR & AT91C_PITC_CPIV)
#define DISABLEI2cIrqs *AT91C_TWI_IDR = 0x000001C7

#define SPEED_INCREMENT (10)
#define MAX_SPEED (100)

static UBYTE * ptr;
static volatile UBYTE counter;
static UBYTE OutBuffer[COPYRIGHTSTRINGLENGTH+1];
static UBYTE InBuffer[BYTES_TO_RX + 1];
static UBYTE rxChecksum, txChecksum;

ULONG txTimer;

IOTOAVR IoToAvr;
IOFROMAVR IoFromAvr;

enum state_t {
  HELLO,
  TX,
  TX_WAIT,
  RX,
  RX_WAIT
}; 
static enum state_t volatile State;

void TxStart(UBYTE *buf, UBYTE len) { 
  int i;
  State = TX_WAIT;
  for(i=0,txChecksum=0; i<len; i++){
    OutBuffer[i] = (UBYTE) buf[i];
    txChecksum += (UBYTE) buf[i];
  }
  OutBuffer[i] = ~txChecksum;
  ptr = OutBuffer;
  counter = BYTES_TO_TX;
  *AT91C_TWI_CR = AT91C_TWI_MSEN;  
  *AT91C_TWI_MMR = AT91C_TWI_IADRSZ_NO | DEVICE_ADR << 16; 
  *AT91C_TWI_IER = AT91C_TWI_TXRDY | AT91C_TWI_NACK | AT91C_TWI_TXCOMP;
  *AT91C_TWI_THR = *ptr;
  *AT91C_TWI_CR = AT91C_TWI_START;
  return;
}  

void RxStart(void) {
  State = RX_WAIT;
  rxChecksum = 0;
  ptr = InBuffer; 
  counter = BYTES_TO_RX+1;
  *AT91C_TWI_RHR;
  *AT91C_TWI_CR = AT91C_TWI_MSEN;  
  *AT91C_TWI_MMR = AT91C_TWI_MREAD | AT91C_TWI_IADRSZ_NO | DEVICE_ADR << 16;
  *AT91C_TWI_IER = AT91C_TWI_RXRDY | AT91C_TWI_NACK  | AT91C_TWI_TXCOMP;
  *AT91C_TWI_CR = AT91C_TWI_START;
  return;
}

__ramfunc void I2cHandler(void) {
  ULONG tmp;
  tmp = *AT91C_TWI_SR;
  if((tmp & AT91C_TWI_TXRDY) && (State == TX_WAIT)){
    if(counter--){
      ptr++;
      *AT91C_TWI_THR = *ptr;
    }else{
      *AT91C_TWI_CR = AT91C_TWI_STOP;
        State = RX;
      DISABLEI2cIrqs;
    }
  }
  if((tmp & AT91C_TWI_RXRDY) && (State == RX_WAIT)){
    if(counter--){
      *ptr = *AT91C_TWI_RHR;
      rxChecksum += *ptr;
      ptr++;
    }else{
      *AT91C_TWI_CR = AT91C_TWI_STOP;
        State = TX;
      DISABLEI2cIrqs;
    }
  }
  if(tmp & AT91C_TWI_NACK){
    I2CInit();
  }
  return;
}

void I2CTransfer(void) {
  ULONG tmp;
  tmp = *AT91C_TWI_SR;
  switch(State){
    case(HELLO):
          txTimer = 0;
      TxStart((UBYTE *) CopyrightStr,COPYRIGHTSTRINGLENGTH+1);
      break;
    case(TX):
      if(rxChecksum == 0xFF){
        memcpy((UBYTE *) &IoFromAvr,InBuffer,BYTES_TO_RX);
        reset_shutdown();
      }
      TxStart((UBYTE *) &IoToAvr, BYTES_TO_TX);
      break;
    case(RX):
      RxStart();
      break;
    case(TX_WAIT):
      if(tmp & AT91C_TWI_TXCOMP) State = RX;
      break;
    case(RX_WAIT):
      if(tmp & AT91C_TWI_TXCOMP) State = TX;
      break;
  }
  return;  
}

void I2CCtrl (enum power_t p) {
  switch(p){
    case (POWERDOWN):
      IoToAvr.Power = 0x5A;
      IoToAvr.PwmFreq = 0x00;
      break;
    case (REPROGRAM):
      IoToAvr.Power = 0xA5;
      IoToAvr.PwmFreq = 0x5A;
      break;
    case (NORMAL_OP):
      IoToAvr.Power = NORMAL_OP;
      IoToAvr.PwmFreq = 0x08;
      break;
  }
  return;
}

#define WAITClk(t) {\
    ULONG pit = piir + (t);\
          if (pit >= pimr) pit -= pimr;\
          while (piir < pit){;}\
        }

void I2CInit(void) { 
  //
  // disable I2C on PIO
  // this is called also during an error, so interrupts etc may be enabled
  //
  *AT91C_AIC_IDCR = (1L<<AT91C_ID_TWI);     /* disable AIC irq  */
  DISABLEI2cIrqs;                           /* disable TWI irq  */
  *AT91C_PMC_PCER  = (1L<<AT91C_ID_TWI);    /* enable TWI Clock */
  *AT91C_PIOA_OER  = AT91C_PA4_TWCK;          /* SCL is output    */
  *AT91C_PIOA_ODR  = AT91C_PA3_TWD;     /* SDA is input     */
  *AT91C_PIOA_MDER = (AT91C_PA4_TWCK | AT91C_PA3_TWD);  /* open drain       */
  *AT91C_PIOA_PPUDR = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* no pull up       */
  // 
  // synch I2C clocks using PIO
  // generate a 400KHz pulse on SCK and wait until both SCK and SDA are high, 
  // which means the slave ticks with this clock
  //
  *AT91C_PIOA_PER  = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* enable PIO control for these pins */
  while(((*AT91C_PIOA_PDSR & AT91C_PA3_TWD) == 0) || ((*AT91C_PIOA_PDSR & AT91C_PA4_TWCK) == 0)){
      *AT91C_PIOA_CODR = AT91C_PA4_TWCK; /* drive SCL Low  */
      WAITClk(TIME400KHz);
      *AT91C_PIOA_SODR = AT91C_PA4_TWCK; /* drive SCL High */
      WAITClk(TIME400KHz);
  }
  // 
  // init I2C on PIO
  //
  *AT91C_TWI_CR    =  AT91C_TWI_SWRST;      /* this has to happen before the rest */
  *AT91C_PIOA_PDR   = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* disable PIO control for these pins */
  *AT91C_PIOA_ASR   = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* select peripheral A = TWI */
  *AT91C_TWI_CWGR   = (CLDIV | (CLDIV << 8));           /* 400KHz clock    */
  *AT91C_AIC_ICCR   = (1L<<AT91C_ID_TWI);               /* clear AIC irq   */
  AT91C_AIC_SVR[AT91C_ID_TWI] = (unsigned int)I2cHandler;
  AT91C_AIC_SMR[AT91C_ID_TWI] = ((AT91C_AIC_PRIOR_HIGHEST) | (AT91C_AIC_SRCTYPE_INT_EDGE_TRIGGERED));
  *AT91C_AIC_IECR   = (1L<<AT91C_ID_TWI);               /* Enables AIC irq */

  State = HELLO;

  return;
}

void I2CExit(void) {
  DISABLEI2cIrqs;
  *AT91C_AIC_IDCR   = (1L<<AT91C_ID_TWI);               /* Disable AIC irq  */
  *AT91C_AIC_ICCR   = (1L<<AT91C_ID_TWI);               /* Clear AIC irq    */
  *AT91C_PMC_PCDR   = (1L<<AT91C_ID_TWI);               /* Disable clock    */
  *AT91C_PIOA_MDER  = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* Open drain       */
  *AT91C_PIOA_PPUDR = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* no pull up       */
  *AT91C_PIOA_PER   = (AT91C_PA4_TWCK | AT91C_PA3_TWD); /* Disable peripheal*/
}

void reset_shutdown(){
//    static UBYTE enter_counter = 0;
    
  button_pressed = ButtonRead_Sampled();
  LedReactOnButtonPressed(button_pressed);
    
  switch (button_pressed) {

  case BUTTON_ENTER:
              I2CCtrl(POWERDOWN);
     break;
  case BUTTON_EXIT:
              I2CCtrl(NORMAL_OP);
    break;
  case BUTTON_LEFT:
    I2CCtrl(NORMAL_OP);
    break;
  case BUTTON_RIGHT:
    I2CCtrl(NORMAL_OP);
    break;
  default:
    I2CCtrl(NORMAL_OP);
    break;
  }
}
