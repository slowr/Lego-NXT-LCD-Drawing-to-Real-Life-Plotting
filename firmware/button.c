#include "button.h"
#include "arm2avr.h"
#include "pit.h"

enum button_t button_pressed;

static UBYTE ButtonLeftActivated = FALSE;
static UBYTE ButtonRightActivated = FALSE;
static UBYTE ButtonEnterActivated = FALSE;
static UBYTE ButtonExitActivated = FALSE;

void   ButtonInit(void)
{
  button_pressed = BUTTON_NONE;
}

void   ButtonExit(void)
{ ; }

enum button_t ButtonRead(void)
{
    switch (IoFromAvr.Buttons) {
        //case 0x7E:
        case 0x7F:
        //case 0x80:
          button_pressed = BUTTON_LEFT;
          break;
        //case 0x195:
        case 0x196:
        //case 0x197:
          button_pressed = BUTTON_RIGHT;
          break;
        //case 0x3FE:
        case 0x3FF:
        //case 0x400:
          button_pressed = BUTTON_EXIT;
          break;
        case 0x7FF:
        case 0x87E:
        case 0x995:
        case 0xBFE:
          button_pressed = BUTTON_ENTER;
          break;
        default:
          button_pressed = BUTTON_NONE;
          break;
    }
    
    //ButtonPosEdge(button_pressed, UPDATE_BUTTON_STATE);
    return button_pressed;
}

enum button_t ButtonRead_Sampled()
{
#define LEFTRIGHT_SMPL_INTRVL   5
#define EXIT_SMPL_INTRVL        10
#define ENTER_SMPL_INTRVL       10
    
    enum button_t btn;
    
    btn = ButtonRead();

    switch (btn) {
        case BUTTON_LEFT:
        case BUTTON_RIGHT:
            if (!(timer % LEFTRIGHT_SMPL_INTRVL))
                return btn;
            break;
        case BUTTON_ENTER:
            if (!(timer % ENTER_SMPL_INTRVL))
                return btn;
            break;
        case BUTTON_EXIT:
            if (!(timer % EXIT_SMPL_INTRVL))
                return btn;
            break;
        default:            // BUTTON_NONE;
            break;
    }

    return BUTTON_NONE;
}

UBYTE ButtonPosEdge(enum button_t Button) {
    
#define BUTTON_DELAY 1000
    
    static UBYTE ButtonOn[4] = {0};
    static UWORD ButtonCounter[4] = {0};
    
    static UBYTE ButtonLeftPressed = 0;
    static UBYTE ButtonRightPressed = 0;
    static UBYTE ButtonEnterPressed = 0;
    static UBYTE ButtonExitPressed = 0;
    
    UBYTE ret = 0;

    button_pressed = ButtonRead();
    
    switch (button_pressed) {
        case BUTTON_LEFT:
            if (Button == BUTTON_LEFT) {
                ret = (ButtonCounter[0] < BUTTON_DELAY - 1) ? 0 : 1;
                ButtonLeftPressed =
                (ButtonCounter[0] < BUTTON_DELAY - 1) ? 1 : 0;
                ButtonCounter[0] = (ButtonCounter[0] + 1) % BUTTON_DELAY;
            } else {
                ret = 0;
                ButtonCounter[0] = 0;
            }
            ButtonRightPressed = ButtonEnterPressed = ButtonExitPressed = 0;
            ButtonCounter[1] = ButtonCounter[2] = ButtonCounter[3] = 0;
            return ret;
        case BUTTON_RIGHT:
            if (Button == BUTTON_RIGHT) {
                ret = (ButtonCounter[1] < BUTTON_DELAY - 1) ? 0 : 1;
                ButtonCounter[1] = (ButtonCounter[1] + 1) % BUTTON_DELAY;
            } else {
                ret = 0;
                ButtonCounter[1] = 0;
            }
            ButtonRightPressed = ButtonEnterPressed = ButtonExitPressed = 0;
            ButtonCounter[1] = ButtonCounter[2] = ButtonCounter[3] = 0;
            return ret;
        case BUTTON_ENTER:
            if (Button == BUTTON_ENTER) ret = 1 - ButtonEnterPressed;
            ButtonLeftPressed = 0;
            ButtonRightPressed = 0;
            ButtonEnterPressed = 1;
            ButtonExitPressed = 0;
            return ret;
        case BUTTON_EXIT:
            if (Button == BUTTON_EXIT) ret = 1 - ButtonExitPressed;
            ButtonLeftPressed = 0;
            ButtonRightPressed = 0;
            ButtonEnterPressed = 0;
            ButtonExitPressed = 1;
            return ret;
        default:       // BUTTON_NONE
            ButtonLeftPressed = 0;
            ButtonRightPressed = 0;
            ButtonEnterPressed = 0;
            ButtonExitPressed = 0;
            return 0;
    }

    return 0;
}

UBYTE ButtonActivatedState(enum button_t Button)
{
    UBYTE posedge = ButtonPosEdge(Button);
    
    switch (Button) {
        case BUTTON_LEFT:
            if (posedge)
                ButtonLeftActivated = ButtonLeftActivated ? FALSE : TRUE;
            return ButtonLeftActivated;
        case BUTTON_RIGHT:
            if (posedge)
                ButtonRightActivated = ButtonRightActivated ? FALSE : TRUE;
            return ButtonRightActivated;
        case BUTTON_ENTER:
            if (posedge)
                ButtonEnterActivated = ButtonEnterActivated ? FALSE : TRUE;
            return ButtonEnterActivated;
        case BUTTON_EXIT:
            if (posedge)
                ButtonExitActivated = ButtonExitActivated ? FALSE : TRUE;
            return ButtonEnterActivated;
        default:
            return FALSE;
    }
 
    return FALSE;
}
