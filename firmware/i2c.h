#ifndef   _I2C_H
#define   _I2C_H

#include "pit.h"

enum power_t {
  NORMAL_OP = 0,
  POWERDOWN,
  REPROGRAM
};

void I2CInit(void);
void I2CExit(void);
void I2CTransfer(void);
void I2CCtrl(enum power_t);
void reset_shutdown(void);
void controlMotor(SBYTE);


#endif
