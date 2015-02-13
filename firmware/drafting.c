//
//  drafting.c
//
//

#include <stdlib.h>
#include <math.h>

#include "drafting.h"

#include "display.h"
#include "input.h"
#include "helpfunc.h"
#include "drawing.h"
#include "button.h"
#include "i2c.h"


#define PI 3.14159265

#define MAX_POLYLINE_EDGES      100

#define TOLERANCE 5

static UBYTE PolyLine[MAX_POLYLINE_EDGES+1][2] = {{-1}};

static SBYTE last = -1;

static SBYTE polyLineEdges = -1;
static UBYTE Cursor[2] = {50, 32};

static volatile UBYTE newCursor[2] = {50, 32};

static UBYTE PrevTouchSensorStatus = 0;
static UBYTE polyLineActive = FALSE;

enum PolyLine_t {
    SINGLE_POINT,
    OPEN_POLYLINE,
    CLOSED_POLYGON,
    NO_POLYLINE
};

static enum PolyLine_t type = NO_POLYLINE;

ULONG polyEdgeNormSquared(UBYTE);

UBYTE isDraftReady(void) {
    return ((polyLineEdges > 0) ? TRUE : FALSE);
}

UBYTE CheckDraftSetInactive(void) {
    polyLineActive = FALSE;
    return isDraftReady();
}

UBYTE draft_polyline(draft_t ThisDraft)
{
    UBYTE ii;
    static ULONG counter = 0;
    
    //I2CTransfer();
    button_pressed = BUTTON_NONE;
    
    DisplayErase();
    
    if (ThisDraft == DRAFT_ERASE) {
        for (ii = 0; ii <= MAX_POLYLINE_EDGES; ii++)
            PolyLine[ii][0] = PolyLine[ii][1] = -1;
        Cursor[0] = newCursor[0] = 50;
        Cursor[1] = newCursor[1] = 32;
        polyLineEdges = -1;
        last = -1;
        return TRUE;
    }

    if (ThisDraft == DRAFT_RESTORE) polyLineActive = FALSE;

    DisplayString(0x0, 0x8, (UBYTE *) (polyLineActive ? "Active" : "Inactive"));
    DisplayString(0x0, 0x10, (UBYTE *) "EDGES: ");
    DisplayNum(0x1, 0x3C, 0x10, polyLineEdges);
    DisplayString(0x40, 0x0, (UBYTE *) "N=");
    if (polyLineActive) DisplayNum(0x1, 0x4C, 0x0, polyEdgeNormSquared(1));
    else DisplayString(0x4C, 0x0, (UBYTE *) "xxx");
    DisplayNum(0x1, 0x40, 0x8, IoFromAvr.Buttons);
    
//    if (ButtonPosEdge(BUTTON_RIGHT, EXAMINE_BUTTON_STATE))
//        newCursor[0] = (newCursor[0] + 1) % 100;
//    else if (ButtonPosEdge(BUTTON_LEFT, EXAMINE_BUTTON_STATE) )
//        newCursor[1] = (newCursor[1] + 1) % 64;

    button_pressed = ButtonRead_Sampled();
    if (ButtonRead_Sampled() == BUTTON_RIGHT)//    if (button_pressed == BUTTON_RIGHT) // && (++counter / 3000 % 2))
        newCursor[0] = (newCursor[0] + 1) % 100;
    else if (ButtonRead_Sampled() == BUTTON_LEFT)
    //else if (button_pressed == BUTTON_LEFT) // && (++counter / 3000 % 2))
        newCursor[1] = (newCursor[1] + 1) % 64;

    if (ThisDraft == DRAFT_CREATE && inputIsTouchSensorPressed_Sampled()) {
//        if (ThisDraft == DRAFT_CREATE && inputIsPosEdgeTouchSensor()) {
        
        if(!polyLineActive) polyLineActive = TRUE;

        if (polyLineEdges < MAX_POLYLINE_EDGES) {
            
            if (polyLineEdges > -1 &&
                PolyLine[polyLineEdges][0] == Cursor[0] &&
                PolyLine[polyLineEdges][1] == Cursor[1])
                polyLineActive = FALSE;
            
            if (polyLineActive) {
                ++polyLineEdges;
                PolyLine[polyLineEdges][0] = Cursor[0];
                PolyLine[polyLineEdges][1] = Cursor[1];
                    
                if (polyLineEdges > 0 &&
                    polyEdgeNormSquared(0) < TOLERANCE * TOLERANCE) {
                    PolyLine[polyLineEdges][0] = PolyLine[0][0];
                    PolyLine[polyLineEdges][1] = PolyLine[0][1];
                    polyLineActive = FALSE;
                    type = CLOSED_POLYGON;
                }
                
                switch (polyLineEdges) {
                    case 0:
                        type = SINGLE_POINT;
                        break;
                    case 1:
                        type = OPEN_POLYLINE;
#if(0)
                        DisplayErase();
                        DisplayNum(0x1, 0x30, 0x30, computeLength(PolyLine[0][0],
                                                                  PolyLine[0][1],
                                                                  PolyLine[1][0],
                                                                  PolyLine[1][1]));
                        DisplayNum(0x1, 0x30, 0x38, INVALID_ANGLE);
#endif
                        updateLineAngle(computeLength(PolyLine[0][0],
                                                      PolyLine[0][1],
                                                      PolyLine[1][0],
                                                      PolyLine[1][1]),
                                        INVALID_ANGLE);
                        break;
                    default:
                        if (type != CLOSED_POLYGON) type = OPEN_POLYLINE;
#if(0)
                        DisplayErase();
                        DisplayNum(0x1, 0x30, 0x30, computeLength(PolyLine[polyLineEdges - 1][0],
                                                                  PolyLine[polyLineEdges - 1][1],
                                                                  PolyLine[polyLineEdges][0],
                                                                  PolyLine[polyLineEdges][1]));
                        DisplayNum(0x1, 0x30, 0x38, computeAngle(PolyLine[polyLineEdges - 2][0],
                                                                 PolyLine[polyLineEdges - 2][1],
                                                                 PolyLine[polyLineEdges - 1][0],
                                                                 PolyLine[polyLineEdges - 1][1],
                                                                 PolyLine[polyLineEdges][0],
                                                                 PolyLine[polyLineEdges][1]));
#endif
                        updateLineAngle(computeLength(PolyLine[polyLineEdges - 1][0],
                                                      PolyLine[polyLineEdges - 1][1],
                                                      PolyLine[polyLineEdges][0],
                                                      PolyLine[polyLineEdges][1]),
                                        computeAngle(PolyLine[polyLineEdges - 2][0],
                                                     PolyLine[polyLineEdges - 2][1],
                                                     PolyLine[polyLineEdges - 1][0],
                                                     PolyLine[polyLineEdges - 1][1],
                                                     PolyLine[polyLineEdges][0],
                                                     PolyLine[polyLineEdges][1]));
                        break;
                }
#if(0)
                if (!polyLineActive && polyLineEdges > 0) {
                    if (type == OPEN_POLYLINE) {
#if(0)
                       DisplayErase();
                        DisplayNum(0x1, 0x30, 0x30, INVALID_LENGTH);
                        DisplayNum(0x1, 0x30, 0x38, 0);
#endif
                        updateLineAngle(INVALID_LENGTH, 0);
                    } else {
#if(0)
                        DisplayErase();
                        DisplayNum(0x1, 0x30, 0x30, INVALID_LENGTH);
                        DisplayNum(0x1, 0x30, 0x38,computeAngle(PolyLine[polyLineEdges - 1][0],
                                                                PolyLine[polyLineEdges - 1][1],
                                                                PolyLine[0][0],
                                                                PolyLine[0][1],
                                                                PolyLine[1][0],
                                                                PolyLine[1][1]));
#endif
                        updateLineAngle(INVALID_LENGTH,
                                        computeAngle(PolyLine[polyLineEdges - 1][0],
                                                     PolyLine[polyLineEdges - 1][1],
                                                     PolyLine[0][0],
                                                     PolyLine[0][1],
                                                     PolyLine[1][0],
                                                     PolyLine[1][1]));
                    }
                }
#endif
            }
    
        } else polyLineActive = FALSE;
            
    } //else PrevTouchSensorStatus = 0;

    if (ThisDraft == DRAFT_CREATE || ThisDraft == DRAFT_RESTORE) {
        
        if (polyLineEdges >= 0) {
            for (ii = 0; ii < polyLineEdges; ii++)
                DisplayLineXY(PolyLine[ii][0], PolyLine[ii][1],
                              PolyLine[ii+1][0], PolyLine[ii+1][1]);

            if (polyLineActive)
                DisplayLineXY(PolyLine[ii][0], PolyLine[ii][1],
                              Cursor[0], Cursor[1]);
        }
        
        if (ThisDraft != DRAFT_RESTORE) DisplayCross(Cursor[0], Cursor[1]);
        
        DisplayString(0x0, 0x30, (UBYTE *) "X: ");
        DisplayString(0x0, 0x38, (UBYTE *) "Y: ");
        DisplayNum(0x1, 0xC, 0x30, Cursor[0]);
        DisplayNum(0x1, 0xC, 0x38, Cursor[1]);
        
        Cursor[0] = newCursor[0];
        Cursor[1] = newCursor[1];
    }
    
    DisplayUpdateSync();
    
    return TRUE;
}

//

ULONG computeLength(ULONG xx0, ULONG yy0, ULONG xx1, ULONG yy1) {
    return ((ULONG)sqrt((xx1 - xx0)*(xx1 - xx0) + (yy1 - yy0)*(yy1 - yy0)));
}

ULONG computeAngle(ULONG xx0, ULONG yy0, ULONG xx1, ULONG yy1, ULONG xx2, ULONG yy2)
{
    float theta;
    ULONG ax, ay, bx, by;
    SLONG inner, outer;
    ULONG phi;
    
    ax = 100*(xx1 - xx0);
    ay = 100*(yy1 - yy0);
    bx = 100*(xx2 - xx1);
    by = 100*(yy2 - yy1);
    
    inner = ax*bx + ay*by;
    outer = ax*by - ay*bx;
    
    theta = (atan2f(outer, inner) * 180.) / (float) M_PI;
 
    if (theta < 0.0) theta = 360. + theta;
    
    if (theta - (unsigned int)theta >= 0.5) phi = (unsigned int)theta + 1;
    else phi = (unsigned int)theta;
    
    return phi;
}

void updateLineAngle(ULONG length, ULONG angle)
{
    static ULONG l_buf = INVALID_LENGTH;
    static ULONG a_buf = INVALID_ANGLE;
    
    UBYTE l_pending = 0;
    UBYTE a_pending = 0;
    UBYTE readyLengthAngle = FALSE;

    if(angle == -1){
        setLineAngle(l_buf, -1);
        return;
    }
    
    if (l_buf == INVALID_LENGTH) l_buf = length;
    else l_pending = (length == INVALID_LENGTH) ? 0 : 1;
    
    if (a_buf == INVALID_ANGLE) a_buf = angle;
    else a_pending = (angle == INVALID_ANGLE) ? 0 : 1;
   
    if (l_buf != INVALID_LENGTH && a_buf != INVALID_ANGLE)
        readyLengthAngle = TRUE;
    //else if (l_buf == INVALID_LENGTH && a_pending) {
    //    l_buf = 0;
    //    readyLengthAngle = TRUE;
    //} else if (a_buf == INVALID_ANGLE && l_pending) {
    //    a_buf = 0;
    //    readyLengthAngle = TRUE;
    //}

    if (readyLengthAngle) {
        setLineAngle(l_buf, a_buf);
        l_buf = (l_pending ? length : INVALID_LENGTH);
        a_buf = (a_pending ? angle : INVALID_ANGLE);
    }

    return;
}

ULONG polyEdgeNormSquared(UBYTE thisCursor)
{
    return ( thisCursor ?
            (Cursor[0] - PolyLine[0][0]) *
            (Cursor[0] - PolyLine[0][0]) +
            (Cursor[1] - PolyLine[0][1]) *
            (Cursor[1] - PolyLine[0][1]) :
            (PolyLine[polyLineEdges][0] - PolyLine[0][0]) *
            (PolyLine[polyLineEdges][0] - PolyLine[0][0]) +
            (PolyLine[polyLineEdges][1] - PolyLine[0][1]) *
            (PolyLine[polyLineEdges][1] - PolyLine[0][1]));
}
