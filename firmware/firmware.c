#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "AT91SAM7S256.h"
#include "hwinit.h"
#include "pit.h"
#include "aic.h"
#include "display.h"
#include "sound.h"
#include "i2c.h"
#include "arm2avr.h"
#include "button.h"
#include "input.h"
#include "led.h"
#include "d_output.h"
#include "helpfunc.h"
#include "drawing.h"
#include "drafting.h"

static volatile ULONG running_state = 0;
static volatile draw_state state = DRAW_LINE;
static volatile UBYTE counter = 0;
static volatile UBYTE position = 0;


//ULONG computeLength(ULONG, ULONG, ULONG, ULONG);

//ULONG computeAngle(ULONG, ULONG, ULONG, ULONG, ULONG, ULONG);

ULONG xx0 = 0;
ULONG yy0 = 0;
ULONG xx1 = 100;
ULONG yy1 = 0;
ULONG xx2 = 50;
ULONG yy2 = 87;

ULONG xxx[4], yyy[4];

//#define INPUT_DEBUG

int main(void) {
    enum operation_state_t {
        IDLE_OPSTATE,
        DRAWING_OPSTATE,
        DISPLAY_OPSTATE,
        STANDBY_OPSTATE,
        PLOTTING_OPSTATE
    };

    UWORD value;
    
    enum operation_state_t current_opstate = IDLE_OPSTATE;
    //draft_t ThisDraft = DRAFT_ERASE;
    
  HardwareInit();
  DisplayInit();
  PITinit(0xBB8);
  PITEnable();
  SoundInit();
  I2CInit();
  InputInit();
  ButtonInit();
  dOutputInit();
  initializeWindow();

    
#ifdef INPUT_DEBUG
    
    while(TRUE) {
        DisplayErase();

        switch (ButtonRead_Sampled()) {

//        switch (button_pressed) {
            case BUTTON_LEFT:
                DisplayString(0x0,0x0,(UBYTE *) "Button: <-");
                break;
            case BUTTON_RIGHT:
                DisplayString(0x0,0x0,(UBYTE *) "Button: ->");
                break;
            case BUTTON_EXIT:
                DisplayString(0x0,0x0,(UBYTE *) "Button: X");
                break;
            case BUTTON_ENTER:
                DisplayString(0x0,0x0,(UBYTE *) "Button: O");
                break;
            default:
                DisplayString(0x0,0x0,(UBYTE *) "Button: NONE");
                break;
        }
        DisplayString(0x0,0x8, (UBYTE *) "Button :");
        DisplayNum(0x1,0x30,0x8,IoFromAvr.Buttons);
        DisplayString(0x0,0x10,(UBYTE *) "Motor 0:");
        DisplayNum(0x1,0x30,0x10,IoToAvr.PwmValue[0]);
        DisplayString(0x0,0x18,(UBYTE *) "Motor 1:");
        DisplayNum(0x1,0x30,0x18,IoToAvr.PwmValue[1]);
        DisplayString(0x0,0x20,(UBYTE *) "Motor 2:");
        DisplayNum(0x1,0x30,0x20,IoToAvr.PwmValue[2]);
    
        InputGetSensorValue(&value, 0);
        InputGetSensorValue(&value, 1);
        InputGetSensorValue(&value, 2);
        InputGetSensorValue(&value, 3);
    
        DisplayUpdateSync();
    }
    

#else

SELECT:
    //I2CTransfer();

    switch (current_opstate) {
        case DRAWING_OPSTATE:
            goto DRAWING;
        case DISPLAY_OPSTATE:
            goto DISPLAY_DRW;
        case PLOTTING_OPSTATE:
            goto PLOTTING;
        case STANDBY_OPSTATE:
            goto STANDBY;
        case IDLE_OPSTATE:
            if (isDraftReady()) {
                current_opstate = STANDBY_OPSTATE;
                goto STANDBY;
            } else {
                current_opstate = DRAWING_OPSTATE;
                goto DRAWING;
            }
        default:
            goto EXIT;
    }

STANDBY:
    while (current_opstate == STANDBY_OPSTATE) {
        //I2CTransfer();
        DisplayErase();
        DisplayString(0x0, 0x00, (UBYTE *) "BUTTON OPTIONS");
        DisplayString(0x0, 0x10, (UBYTE *) "LEFT:    Draw");
        DisplayString(0x0, 0x18, (UBYTE *) "RIGHT:   Display");
        DisplayString(0x0, 0x20, (UBYTE *) "TOUCH:   Plot");
        DisplayString(0x0, 0x28, (UBYTE *) "TOP Followed by");
        DisplayString(0xD, 0x30,   (UBYTE *) "TOP:    Reset");
        DisplayString(0xD, 0x38,   (UBYTE *) "BOTTOM: Off");
        DisplayUpdateSync();
        
        if (inputIsTouchSensorPressed_Sampled()) {
            current_opstate = PLOTTING_OPSTATE;
            goto PLOTTING;
        } else if (ButtonRead_Sampled() == BUTTON_RIGHT) {
            current_opstate = DISPLAY_OPSTATE;
            goto DISPLAY_DRW;
        } else if (ButtonRead_Sampled() == BUTTON_LEFT) {
            current_opstate = DRAWING_OPSTATE;
            goto DRAWING;
        } else if (ButtonRead_Sampled() == BUTTON_EXIT) {
            I2CCtrl(POWERDOWN);
        }
    }
    
    goto SELECT;
    
DRAWING:
    //I2CTransfer();
    DisplayErase();
    draft_polyline(DRAFT_ERASE);
    
    while (current_opstate == DRAWING_OPSTATE) {
        //I2CTransfer();

        draft_polyline(DRAFT_CREATE);

        if (ButtonRead_Sampled() == BUTTON_EXIT) {
            current_opstate = STANDBY_OPSTATE;
            break;
        }
        
//        if (ButtonPosEdge(BUTTON_EXIT, EXAMINE_BUTTON_STATE)) {
//            current_opstate = STANDBY_OPSTATE;
//            break;
//        }

        //if (inputIsPosEdgeTouchSensor()) {
         //   if (CheckDraftSetInactive())
          //      current_opstate = PLOTTING_OPSTATE;
           // else
            //    current_opstate = STANDBY_OPSTATE;
            
           // break;
        //}
    }
    
    goto SELECT;
    
DISPLAY_DRW:
    while (current_opstate == DISPLAY_OPSTATE) {
        //I2CTransfer();
        if (!isDraftReady()) {
            DisplayErase();
            DisplayString(0xC, 0x18, (UBYTE *) "EMPTY DRAWING");
            DisplayUpdateSync();
        } else {
            draft_polyline(DRAFT_RESTORE);
        }

        if (ButtonRead_Sampled() == BUTTON_EXIT) {
            current_opstate = STANDBY_OPSTATE;
            break;
        }
        
        if (inputIsTouchSensorPressed_Sampled()) {
            current_opstate = PLOTTING_OPSTATE;
            break;
        }
    }

    goto SELECT;
    
PLOTTING:
    //if (!CheckDraftSetInactive()) {
    //    current_opstate = DISPLAY_OPSTATE;
    //    goto SELECT;
    //}
    
    updateLineAngle(INVALID_LENGTH, -1);
    
#if(0)
    DisplayErase();
    xxx[0] = 0;
    yyy[0] = 0;
    xxx[1] = 20;
    yyy[1] = 0;
    xxx[2] = 20;
    yyy[2] = 20;
    xxx[3] = 30;
    yyy[3] = 20;
    DisplayNum(0x1, 0x0, 0x0, xxx[0]);
    DisplayNum(0x1, 0x20, 0x0, yyy[0]);
    DisplayNum(0x1, 0x0, 0x8, xxx[1]);
    DisplayNum(0x1, 0x20, 0x8, yyy[1]);
    DisplayNum(0x1, 0x0, 0x10, xxx[2]);
    DisplayNum(0x1, 0x20, 0x10, yyy[2]);
    DisplayNum(0x1, 0x0, 0x18, xxx[3]);
    DisplayNum(0x1, 0x20, 0x18, yyy[3]);
    DisplayNum(0x1, 0x0, 0x28, computeLength(xxx[0],yyy[0],xxx[1],yyy[1]));
    DisplayNum(0x1, 0x0, 0x30, computeLength(xxx[1],yyy[1],xxx[2],yyy[2]));
    DisplayNum(0x1, 0x30, 0x30, computeAngle(xxx[0],yyy[0],xxx[1],yyy[1],xxx[2],yyy[2]));
    DisplayNum(0x1, 0x0, 0x38, computeLength(xxx[2],yyy[2],xxx[3],yyy[3]));
    DisplayNum(0x1, 0x30, 0x38, computeAngle(20,0,20,20,30,20));
    DisplayUpdateSync();
    while(1);
#endif
    
    startDrawing();
    current_opstate = STANDBY_OPSTATE;
    
#if (0)
  while (PLOTTING_OPSTATE) {
    DisplayErase();      

      switch(state){
          case DRAW_LINE:
              draw_line((ULONG *) &running_state,
                        computeLength(xx1, yy1, xx2, yy2)/10);
              DisplayNum(0x1, 0x0, 0x20, computeLength(xx1, yy1, xx2, yy2)/10);
              if(running_state == 4){
		          state = DRAW_ANGLE;   // CHANGE TO DRAW ANGLE
		          running_state = 100;  // STARTING STATE FOR ANGLES
              }
              break;
          case DRAW_ANGLE:
              calculate_angle((ULONG *) &running_state,
                              180-computeAngle(xx0, yy0, xx1, yy1, xx2, yy2));
              DisplayNum(0x1,0x0, 0x28, 180-computeAngle(xx0, yy0, xx1, yy1, xx2, yy2) );
              if(running_state == 108){
		          state = DRAW_LINE;    // CHANGE TO DRAW LINE
		          running_state = 0;    // STARTING STATE FOR LINES
		          counter++;
              }
              break;
      }
      
    switch (button_pressed) {
      case BUTTON_LEFT:
      DisplayString(0x0,0x0,(UBYTE *) "Button: <-");
      break;
      case BUTTON_RIGHT:
      DisplayString(0x0,0x0,(UBYTE *) "Button: ->");
      break;
      case BUTTON_EXIT:
      DisplayString(0x0,0x0,(UBYTE *) "Button: X");
      break;
      case BUTTON_ENTER:
      DisplayString(0x0,0x0,(UBYTE *) "Button: O");
      break;
      default:
      DisplayString(0x0,0x0,(UBYTE *) "Button: NONE");
      break;
    }

    DisplayString(0x0,0x8,(UBYTE *) "Motor 0:");
    DisplaySignedNum(0x1,0x30,0x8, IoToAvr.PwmValue[0]);
    DisplayString(0x0,0x10,(UBYTE *) "Motor 1:");
    DisplaySignedNum(0x1,0x30,0x10, IoToAvr.PwmValue[1]);
    DisplayString(0x0,0x18,(UBYTE *) "Motor 2:");
    DisplaySignedNum(0x1,0x30,0x18, IoToAvr.PwmValue[2]);

    DisplayUpdateSync();
      
      if (ButtonRead_Sampled() == BUTTON_EXIT) {
          current_opstate = STANDBY_OPSTATE;
          break;
      }
  }
#endif
    
    goto SELECT;

#endif
    
EXIT:
  ButtonExit();
  InputExit();
  dOutputExit();
  I2CExit();
  PITInterruptDisable();
  PITDisable();
  SoundExit();
  DisplayExit();

  return 0;
}

#if(0)

ULONG computeLength(ULONG xx0, ULONG yy0, ULONG xx1, ULONG yy1)
{
    return ((ULONG)sqrt((xx1 - xx0)*(xx1 - xx0) + (yy1 - yy0)*(yy1 - yy0)));
}

ULONG computeAngle(ULONG xx0, ULONG yy0,
                   ULONG xx1, ULONG yy1,
                   ULONG xx2, ULONG yy2)
{
    float theta;
    SLONG ax, ay, bx, by;
    SLONG inner, outer;
    
    ax = 100*(xx1 - xx0);
    ay = 100*(yy1 - yy0);
    bx = 100*(xx2 - xx1);
    by = 100*(yy2 - yy1);
    
    inner = ax*bx + ay*by;
    outer = ax*by - ay*bx;

#define PI 3.14159265
    
    theta = (float)atan2(outer, inner) * 180 / PI;
    
    if (theta < 0) theta = 360. + theta;
    
    if (theta - (ULONG)theta >= 0.5) theta = (ULONG)theta + 1;
    
    return ((ULONG)theta);
}
#endif

