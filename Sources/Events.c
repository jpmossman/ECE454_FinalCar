/* ###################################################################
**     Filename    : Events.c
**     Project     : Car
**     Processor   : MKL25Z128VLK4
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2023-04-14, 15:30, # CodeGen: 9
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Contents    :
**         Cap1_OnCapture       - void Cap1_OnCapture(void);
**         Cap1_OnOverflow      - void Cap1_OnOverflow(void);
**         Clk_OnEnd            - void Clk_OnEnd(void);
**         SI_Timer_OnInterrupt - void SI_Timer_OnInterrupt(void);
**         AO_OnEnd             - void AO_OnEnd(void);
**         AO_OnCalibrationEnd  - void AO_OnCalibrationEnd(void);
**         Cpu_OnNMIINT         - void Cpu_OnNMIINT(void);
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */
#include "Motors.h"

/* ---------------------------------------- Global variables and constants ----------------------------------------- */
// Configurable constants and coefficients
const double Kps = 30;		// P constant for steering
const double Kds = 2.5;		// D constant for steering
const double Kpv = 1;		// P constant for velocity
const double Ka = 1;		// Attenuation constant for when we veer off path
const double Bv = 0.1;		// Infinite impulse response ratio for velocity smoothing
const double Bs = 0.1;		// IIR ratio for steering smoothing
double velocity_max = 36.0; // inches per second, max speed for straight paths
const uint8_t num_magnets = 4; // Number of magnets

// Line camera variables
static volatile uint16_t count = 0;		//The index of the pixels from the line camera.
volatile char pixel[150] = {0};			//The pixel values of the line camera.
#define Pixel_Count 130					//The number of pixels we are going to read before resetting the camera.
const char desired_center = 64;			//The target center index of the black line is half of 128.

const uint16_t Servo_Center = 20000 - 750;			//The servo center command in us.
const uint16_t Servo_Left	= 20000 - 450;			//The servo max left command in us.
const uint16_t Servo_Right  = 20000 - 1050;			//The servo max right command in us.

// Velocity sensing stuff
double velocity = 0.0; // inches per second
double velocity_desired = 0.0; // inches per second, just a starting val
int ovf = 0; // overflow control CNT
static uint16_t interrupts_since_velocity_update = 0;

// Motor control stuff

// Config switches
#define USE_LINE_WEIGHTED_CENTER false // TODO: actually setup config enable/disable
#define USE_SERVO_PD false // TODO: actually setup config enable/disable
#define USE_VELOCITY_IIR false // TODO: actually setup config enable/disable




/*
** ===================================================================
**     Event       :  Cap1_OnCapture (module Events)
**
**     Component   :  Cap1 [Capture]
**     Description :
**         This event is called on capturing of Timer/Counter actual
**         value (only when the component is enabled - <Enable> and the
**         events are enabled - <EnableEvent>.This event is available
**         only if a <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void Cap1_OnCapture(void)
{
	interrupts_since_velocity_update = 0;
	// Constants and statics
	static uint16_t old_time = 0;
	// TODO: Figure out where this clk_freq comes from???
	const double clk_freq = 2621440; // Hz
	const double _radius = 1.25; // inches
	const double _angle = 6.28 / num_magnets; // radians, tau / num = radial spacing

	// Read in the newest time in clock cycles
	uint16_t new_time = 0;
	Cap1_GetCaptureValue(&new_time);

	// Calculate current speed
	double time = new_time - old_time;	//Get the tick difference.
	time = time / clk_freq;	//convert back to seconds.
	// TODO: Figure out where 0.025 comes from???
	time = time + (ovf * 0.025); // correction for overflow event CNT
	old_time = new_time; //Store the current time into the old time for next use.
	velocity = _radius * _angle / time; //Calculate the velocity in inches / seconds.
	ovf = 0;

	// Calculate velocity using new method
	double yn = _radius * _angle / time;
	static double yn_prev = 0;
	velocity = (1-Bv) * yn + Bv * yn_prev;
	yn_prev = yn;

	// Update speed
	static uint16_t PWM = 0; // Starting speed
	uint16_t delta_PWM = Kpv * (velocity_desired - velocity);
	PWM = PWM + delta_PWM;
//	motors_set(MotorDir_Forward, PWM);
//	motors_set(MotorDir_Forward, 0);
}

/*
** ===================================================================
**     Event       :  Cap1_OnOverflow (module Events)
**
**     Component   :  Cap1 [Capture]
**     Description :
**         This event is called if counter overflows (only when the
**         component is enabled - <Enable> and the events are enabled -
**         <EnableEvent>.This event is available only if a <interrupt
**         service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void Cap1_OnOverflow(void)
{
  /* Write your code here ... */
	ovf++;
}

/*
** ===================================================================
**     Event       :  Clk_OnEnd (module Events)
**
**     Component   :  Clk [PWM]
**     Description :
**         This event is called when the specified number of cycles has
**         been generated. (Only when the component is enabled -
**         <Enable> and the events are enabled - <EnableEvent>). The
**         event is available only when the <Interrupt service/event>
**         property is enabled and selected peripheral supports
**         appropriate interrupt.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void Clk_OnEnd(void)
{
	// Velocity variables
	static uint16_t PWM = 20000 - 750; // Start going straight
	static uint16_t delta_PWM = 0; // Assume no turn at the start
	static char error_prev = 0;
	static double un_prev = 0;
	const char error_max = 64;

	// Line camera stuff. I'm 95% confident that John wrote this??
	if(count > Pixel_Count)	//Sets up the SI Pulse for a new measurement.
	{
		SI_Timer_Enable();
		count = 0; //This is to do a minor offset to correct for the incrementation of count.
		AS1_SendChar('*');
		return;
	}
	else if (count == Pixel_Count) //All pixels have been read, now we can evaluate them.
	{
		// Normal way to figure out the center
		char start = -1;
		char end   = -1;
		// My way to figure out the center by averaging the indices of the black pixel
		uint16_t center_indices_sum = 0;
		uint16_t center_indices_count = 0;
		for (int i = 0; i < 127; ++i) {
			// My method
			if (pixel[i] == '0') { // Black pixel
				center_indices_sum += i + 1;
				center_indices_count += 1;
			}
			// Normal method
			if (pixel[i] == '1' && pixel[i+1] == '0' && end == -1) {
				start = i + 1;
				continue;
			}
			if (pixel[i] == '0' && pixel[i+1] == '1' && start != -1) {
				end = i + 1;
				break;
			}
		}

		if (start == -1 || end == -1)	//If we failed to locate the line then just jump away.
		{
			count++;
			return;
		}
		unsigned char actual_center = start + ((end - start) / 2);
//		if (center_indices_count == 0) {
//			count++;
//			return;
//		}
//		 unsigned char actual_center = center_indices_sum / center_indices_count;


		//Now we can calculate the error and do the PID control for the servo.
		char error = desired_center - actual_center;

		char dError = error - error_prev;		 //
		const double dT = Pixel_Count * 0.001; //Seconds.
		uint16_t Servo_Command = Servo_Center + (Kps * error) ;//+ (Kds * dError/dT);

		//These may have to be flipped.
		if (Servo_Command > Servo_Left) {
			Servo_Command = Servo_Left;
		}

		else if (Servo_Command < Servo_Right) {
			Servo_Command = Servo_Right;
		}


		Servo_SetDutyUS(Servo_Command);

		// Use our method not John's

//		const unsigned char delta_T = 20; // 20 ms???

//		double un = Kps * error + Kds * (error - error_prev) / delta_T;
//		delta_PWM = (1-Bs) * un + Bs * un_prev;
//		PWM = Servo_Center + delta_PWM;
////		Servo_SetDutyUS(PWM);

		error_prev = error;
//		un_prev = un;

	}
	else if (count < 129) //Read each pixel for count = 0 to 127.
	{
		AO_Measure(0);
	}
	count++;

	// Velocity detecting stuff
	if (interrupts_since_velocity_update > 50) { // 50 * 20ms = 1s
		// If it has been too long since an update to velocity, it's stopped
		velocity = 0;
//		motors_set(MotorDir_Forward, 0xFFFF/8);
	}
	interrupts_since_velocity_update++;
	// Update desired velocity
	char error_prev_abs = error_prev < 0 ? -error_prev : error_prev;
	velocity_desired = (1 - error_prev_abs / ((double) error_max)) * velocity_max;
//	velocity_desired = velocity_max - Ka * delta_PWM;
}

/*
** ===================================================================
**     Event       :  SI_Timer_OnInterrupt (module Events)
**
**     Component   :  SI_Timer [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the component is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void SI_Timer_OnInterrupt(void)
{
	static bool SI_Flag = 0;
	if (SI_Flag) {
		SI_ClrVal();
		SI_Flag = 0;
		SI_Timer_Disable();
	}
	else {
		SI_SetVal();
		SI_Flag = 1;
	}
}

/*
** ===================================================================
**     Event       :  AO_OnEnd (module Events)
**
**     Component   :  AO [ADC]
**     Description :
**         This event is called after the measurement (which consists
**         of <1 or more conversions>) is/are finished.
**         The event is available only when the <Interrupt
**         service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AO_OnEnd(void)
{
	count--;
	const uint16_t Threshold = (2.5 / 3.3) * (65535);	//ADC Result equivalent.
	uint16_t ADC_Value = 0;
	AO_GetValue16(&ADC_Value);

	if (ADC_Value >= Threshold) {
		pixel[count] = '1';
	}
	else {
		pixel[count] = '0';
	}

	count++;
	AS1_SendChar(pixel[count]);
}


/*
** ===================================================================
**     Event       :  AO_OnCalibrationEnd (module Events)
**
**     Component   :  AO [ADC]
**     Description :
**         This event is called when the calibration has been finished.
**         User should check if the calibration pass or fail by
**         Calibration status method./nThis event is enabled only if
**         the <Interrupt service/event> property is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void AO_OnCalibrationEnd(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  Cpu_OnNMIINT (module Events)
**
**     Component   :  Cpu [MKL25Z128LK4]
*/
/*!
**     @brief
**         This event is called when the Non maskable interrupt had
**         occurred. This event is automatically enabled when the [NMI
**         interrupt] property is set to 'Enabled'.
*/
/* ===================================================================*/
void Cpu_OnNMIINT(void)
{
  /* Write your code here ... */
}

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
