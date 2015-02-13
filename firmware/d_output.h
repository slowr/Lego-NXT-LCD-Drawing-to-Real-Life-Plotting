//
// Date init       14.12.2004
//
// Revision date   $Date:: 14-11-07 12:40                                    $
//
// Filename        $Workfile:: d_output.h                                    $
//
// Version         $Revision:: 1                                             $
//
// Archive         $Archive:: /LMS2006/Sys01/Main_V02/Firmware/Source/d_outp $
//
// Platform        C
//

#ifndef   D_OUTPUT
#define   D_OUTPUT

#define   NEW_MOTOR

#ifdef    NEW_MOTOR

//Constant reffering to new motor
#define REG_CONST_DIV           32            // Constant which the PID constants value will be divided with
#define DEFAULT_P_GAIN_FACTOR		96//3                            
#define DEFAULT_I_GAIN_FACTOR		32//1                       
#define DEFAULT_D_GAIN_FACTOR		32//1
#define MIN_MOVEMENT_POWER      10
#define MAX_CAPTURE_COUNT       100

#else

//Constant reffering to Old motor
#define REG_CONST_DIV           1            // Constant which the PID constants value will be divided with
#define DEFAULT_P_GAIN_FACTOR		3                            
#define DEFAULT_I_GAIN_FACTOR		1                       
#define DEFAULT_D_GAIN_FACTOR		1
#define MIN_MOVEMENT_POWER      30
#define MAX_CAPTURE_COUNT       80            

#endif

//Constant reffering to RegMode parameter
#define REGSTATE_IDLE           0x00
#define REGSTATE_REGULATED      0x01
#define REGSTATE_SYNCHRONE      0x02

//Constant reffering to RunState parameter
#define MOTOR_RUN_STATE_IDLE      0x00
#define MOTOR_RUN_STATE_RAMPUP    0x10
#define MOTOR_RUN_STATE_RUNNING   0x20
#define MOTOR_RUN_STATE_RAMPDOWN  0x40
#define MOTOR_RUN_STATE_HOLD      0x60

typedef struct
{
  SBYTE MotorSetSpeed;                        // Motor setpoint in speed
  SBYTE MotorTargetSpeed;                     // Speed order for the movement
  SBYTE MotorActualSpeed;                     // Actual speed for motor (Calculated within the PID regulation)
  SBYTE TurnParameter;                        // Tell the turning parameter used
  UBYTE RegPParameter;                        // Current P parameter used within the regulation
  UBYTE RegIParameter;                        // Current I parameter used within the regulation
  UBYTE RegDParameter;                        // Current D parameter used within the regulation
  UBYTE RegulationTimeCount;                  // Time counter used to evaluate when the regulation should run again (100 mS)
  UBYTE MotorRunState;                        // Hold current motor state (Ramp-up, Running, Ramp-Down, Idle)
  UBYTE RegulationMode;                       // Hold current regulation mode (Position control, Synchronization mode)
  UBYTE MotorOverloaded;                      // Set if the motor speed in regulation is calculated to be above maximum
  UBYTE MotorRunForever;                      // Tell that the motor is set to run forever
  UWORD MotorRampDownCount;                   // Counter to tell if the ramp-down can reach it gaol and therefor need some additional help
  SWORD MotorRampDownIncrement;               // Tell the number of count between each speed adjustment during Ramp-Down
  UWORD MotorRampUpCount;                     // Used to speedup Ramp-Up if position regulation is not enabled
  SWORD MotorRampUpIncrement;                 // Tell the number of count between each speed adjustment during Ramp-up
  SWORD AccError;                             // Accumulated Error, used within the integrator of the PID regulation
  SWORD OldPositionError;                     // Used within position regulation
  SLONG DeltaCaptureCount;                    // Counts within last regulation time-periode
  SLONG CurrentCaptureCount;                  // Total counts since motor counts has been reset
  SLONG MotorTachoCountToRun;                 // Holds number of counts to run. 0 = Run forever
  SLONG MotorBlockTachoCount;                 // Hold CaptureCount for current movement
  SLONG MotorRampTachoCountOld;               // Used to hold old position during Ramp-Up
  SLONG MotorRampTachoCountStart;             // Used to hold position when Ramp-up started
  SLONG RotationCaptureCount;                 // Counter for additional rotation counter
} MOTORDATA;

extern MOTORDATA MotorData[3];

enum
{
  MOTOR_A,
  MOTOR_B,
  MOTOR_C
};

void      dOutputInit(void);
void      dOutputExit(void);

void      dOutputCtrl(void);
void      dOutputGetMotorParameters(UBYTE *CurrentMotorSpeed, SLONG *TachoCount, SLONG *BlockTachoCount, UBYTE *RunState, UBYTE *MotorOverloaded, SLONG *RotationCount);
void      dOutputSetMode(UBYTE Motor, UBYTE Mode);
void      dOutputSetSpeed (UBYTE MotorNr, UBYTE NewMotorRunState, SBYTE Speed, SBYTE TurnParameter);
void      dOutputEnableRegulation(UBYTE Motor, UBYTE RegulationMode);
void	  dOutputDisableRegulation(UBYTE Motor);
void      dOutputSetTachoLimit(UBYTE Motor, ULONG TachoCntToTravel);
void      dOutputResetTachoLimit(UBYTE Motor);
void      dOutputResetBlockTachoLimit(UBYTE Motor);
void      dOutputResetRotationCaptureCount(UBYTE MotorNr);
void      dOutputSetPIDParameters(UBYTE Motor, UBYTE NewRegPParameter, UBYTE NewRegIParameter, UBYTE NewRegDParameter); 

void 	  dOutputRegulateMotor(UBYTE MotorNr);
void      dOutputCalculateRampUpParameter(UBYTE MotorNr, ULONG NewTachoLimit);
void      dOutputRampDownFunction(UBYTE MotorNr);
void      dOutputRampUpFunction(UBYTE MotorNr);
void      dOutputTachoLimitControl(UBYTE MotorNr);
void      dOutputCalculateMotorPosition(UBYTE MotorNr);
void      dOutputSyncMotorPosition(UBYTE MotorOne, UBYTE MotorTwo);
void      dOutputMotorReachedTachoLimit(UBYTE MotorNr);
void      dOutputMotorIdleControl(UBYTE MotorNr);
void      dOutputSyncTachoLimitControl(UBYTE MotorNr);
void      dOutputMotorSyncStatus(UBYTE MotorNr, UBYTE *SyncMotorOne, UBYTE *SyncMotorTwo);
void      dOutputResetSyncMotors(UBYTE MotorNr);

#endif
