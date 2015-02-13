#include "helpfunc.h"

void draw_line(ULONG *state, ULONG centimetre){
	static volatile ULONG x = 0;
	if(dTimerRead() > 1000 && *state == 0){    
      	drop_stylo();
      	x = dTimerRead();
      	(*state)++;
    }
    else if(*state == 1){
      	if(dTimerRead() - x > 1000) (*state)++;
    }
	else if(*state == 2){
		go_backwards(centimetre);
		x = dTimerRead();
		(*state)++;
    }
    else if(*state == 3){
    	if(dTimerRead() - x > 6000) (*state)++; 
    }
}

void calculate_angle(ULONG *state, ULONG degree){
	static volatile ULONG x = 0;
	if(*state == 100){
      	raise_stylo();
      	x = dTimerRead();
      	(*state)++;
    }
    else if(*state == 101){
      	if(dTimerRead() - x > 1000) (*state)++;
    }
    else if(*state == 102){
		go_forwards(20);
		x = dTimerRead();
		(*state)++;
    }
    else if(*state == 103){
      	if(dTimerRead() - x > 6000) (*state)++;
    }
    else if(*state == 104){
      	if(degree < 180) rotate_right(degree);
      	else{
//			degree-=180;
            degree = 360 - degree;
			rotate_left(degree);
      	}
     	x = dTimerRead();
      	(*state)++;
    }
    else if(*state == 105){
      if(dTimerRead() - x > 6000) (*state)++;
    }
    else if(*state == 106){
    	if(degree < 180) go_backwards(19);
    	else go_backwards(19);
		x = dTimerRead();
		(*state)++;
    }
    else if(*state == 107){
      	if(dTimerRead() - x > 6000) (*state)++;
    }
}

void drop_stylo(){
	dOutputResetTachoLimit(MOTOR_A);
	dOutputSetSpeed(MOTOR_A, MOTOR_RUN_STATE_RUNNING, 55, 0);
	dOutputSetTachoLimit(MOTOR_A, 100);
	return;
}

void raise_stylo(){
	dOutputResetTachoLimit(MOTOR_A);
	dOutputSetSpeed(MOTOR_A, MOTOR_RUN_STATE_RUNNING, -60, 0);
	dOutputSetTachoLimit(MOTOR_A, 100);
}

void go_backwards(ULONG centimetre){
	dOutputEnableRegulation(MOTOR_B, REGSTATE_SYNCHRONE);
	dOutputEnableRegulation(MOTOR_C, REGSTATE_SYNCHRONE);
	dOutputRegulateMotor(MOTOR_B);
	dOutputResetTachoLimit(MOTOR_C);
	dOutputResetTachoLimit(MOTOR_B);
	dOutputSetSpeed(MOTOR_C, MOTOR_RUN_STATE_RUNNING, -60, 0);
	dOutputSetSpeed(MOTOR_B, MOTOR_RUN_STATE_RUNNING, -60, 0);
	dOutputSetTachoLimit(MOTOR_C, centimetre*20);
	dOutputSetTachoLimit(MOTOR_B, centimetre*20);
	dOutputDisableRegulation(MOTOR_B);
	dOutputDisableRegulation(MOTOR_C);
}

void go_forwards(ULONG centimetre){
	dOutputEnableRegulation(MOTOR_B, REGSTATE_SYNCHRONE);
	dOutputEnableRegulation(MOTOR_C, REGSTATE_SYNCHRONE);
	dOutputRegulateMotor(MOTOR_B);
	dOutputResetTachoLimit(MOTOR_C);
	dOutputResetTachoLimit(MOTOR_B);
	dOutputSetSpeed(MOTOR_C, MOTOR_RUN_STATE_RUNNING, 60, 0);
	dOutputSetSpeed(MOTOR_B, MOTOR_RUN_STATE_RUNNING, 60, 0);
	dOutputSetTachoLimit(MOTOR_C, centimetre*20);
	dOutputSetTachoLimit(MOTOR_B, centimetre*20);
	dOutputDisableRegulation(MOTOR_B);
	dOutputDisableRegulation(MOTOR_C);
}

void rotate_left(ULONG degree){
	dOutputResetTachoLimit(MOTOR_B);
	dOutputResetTachoLimit(MOTOR_C);
	dOutputSetSpeed(MOTOR_B, MOTOR_RUN_STATE_RUNNING, 68, 0);
	dOutputSetSpeed(MOTOR_C, MOTOR_RUN_STATE_RUNNING, -68, 0);
//	dOutputSetTachoLimit(MOTOR_B, (180 - degree)*2.1);
//	dOutputSetTachoLimit(MOTOR_C, (180 - degree)*2.1);
	dOutputSetTachoLimit(MOTOR_B, (degree)*2.1);
	dOutputSetTachoLimit(MOTOR_C, (degree)*2.1);
	dOutputDisableRegulation(MOTOR_C);
	dOutputDisableRegulation(MOTOR_B);
}

void rotate_right(ULONG degree){
	dOutputResetTachoLimit(MOTOR_C);
	dOutputResetTachoLimit(MOTOR_B);
	dOutputSetSpeed(MOTOR_C, MOTOR_RUN_STATE_RUNNING, 69, 0);
	dOutputSetSpeed(MOTOR_B, MOTOR_RUN_STATE_RUNNING, -68, 0);
	dOutputSetTachoLimit(MOTOR_C, degree*2.1);
	dOutputSetTachoLimit(MOTOR_B, degree*2.1);
	dOutputDisableRegulation(MOTOR_B);
	dOutputDisableRegulation(MOTOR_C);
}

ULONG dTimerRead(){
  return timer;
}
