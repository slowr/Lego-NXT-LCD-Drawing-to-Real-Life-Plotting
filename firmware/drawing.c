#include "drawing.h"

static volatile ULONG running_state = 0;
static volatile draw_state state = DRAW_LINE;
static volatile UBYTE counter = 0;
static volatile UBYTE position = 0;

void initializeWindow(){
	UBYTE i;
	for(i=0; i<PROJECT_SIZE; i++){
		lineWindow[i] = -1;
		angleWindow[i] = -1;
	}
}

UBYTE startDrawing(){
#if(0)
    DisplayErase();
    DisplayNum(0x1, 0x0, 0x0, lineWindow[0]);
    DisplayNum(0x1, 0x20, 0x0, angleWindow[0]);
    DisplayNum(0x1, 0x0, 0x8, lineWindow[1]);
    DisplayNum(0x1, 0x20, 0x8, angleWindow[1]);
    DisplayNum(0x1, 0x0, 0x10, lineWindow[2]);
    DisplayNum(0x1, 0x20, 0x10, angleWindow[2]);
    DisplayNum(0x1, 0x0, 0x18, lineWindow[3]);
    DisplayNum(0x1, 0x20, 0x18, angleWindow[3]);
    DisplayNum(0x1, 0x0, 0x20, lineWindow[4]);
    DisplayNum(0x1, 0x20, 0x20, angleWindow[4]);
    DisplayNum(0x1, 0x20, 0x28, position);
    DisplayUpdateSync();

	while (1==1);
#endif
    
    while((lineWindow[counter] != -1) && (counter < PROJECT_SIZE)){
//        DisplayErase();
//        DisplayNum(0x1, 0x0, 0x0, counter);
//        DisplayNum(0x1, 0x0, 0x8, lineWindow[counter]);
//        DisplayNum(0x1, 0x0, 0x10, angleWindow[counter]);
//        DisplayUpdateSync();
		switch(state){
	      	case DRAW_LINE:
		        draw_line((ULONG *) &running_state, lineWindow[counter]);
		        if(running_state == 4){
		          state = DRAW_ANGLE;   // CHANGE TO DRAW ANGLE
		          running_state = 100;  // STARTING STATE FOR ANGLES
		        }
		        break;
	      	case DRAW_ANGLE:
                if(angleWindow[counter] != -1){
		        calculate_angle((ULONG *) &running_state, angleWindow[counter]);
		        if(running_state == 108){
		          state = DRAW_LINE;    // CHANGE TO DRAW LINE
		          running_state = 0;    // STARTING STATE FOR LINES
		          counter++;
		        }
                } else {
                    counter++;
                    raise_stylo();
                    return TRUE;
                }
	       		break;
    	}
	}
	return TRUE;
}

UBYTE setLineAngle(ULONG centimetre,ULONG degree){
	if(position < PROJECT_SIZE){
		lineWindow[position] = centimetre;
		angleWindow[position] = degree;
		position++;
		return TRUE;
	}
	return FALSE;
}
