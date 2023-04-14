/* ###################################################################
**     Filename    : Events.h
**     Project     : Car
**     Processor   : MKL25Z128VLK4
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2023-04-14, 15:37, # CodeGen: 10
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
** @file Events.h
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         

#ifndef __Events_H
#define __Events_H
/* MODULE Events */

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "AS1.h"
#include "ASerialLdd1.h"
#include "SI.h"
#include "BitIoLdd1.h"
#include "AO.h"
#include "AdcLdd1.h"
#include "SI_Timer.h"
#include "TimerIntLdd1.h"
#include "Servo.h"
#include "PwmLdd1.h"
#include "Clk.h"
#include "PwmLdd2.h"
#include "MotorTimer.h"
#include "LineCameraPIT.h"
#include "LineCameraTimer.h"
#include "PWM_FA.h"
#include "PwmLdd3.h"
#include "PWM_BA.h"
#include "PwmLdd6.h"
#include "PWM_FB.h"
#include "PwmLdd4.h"
#include "PWM_BB.h"
#include "PwmLdd5.h"
#include "Cap1.h"
#include "CaptureLdd1.h"
#include "VelocityTimer.h"

#ifdef __cplusplus
extern "C" {
#endif 

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
void Cap1_OnCapture(void);

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
void Cap1_OnOverflow(void);

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
void Clk_OnEnd(void);

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
void SI_Timer_OnInterrupt(void);

void AO_OnEnd(void);
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

void AO_OnCalibrationEnd(void);
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
void Cpu_OnNMIINT(void);


/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

#endif 
/* ifndef __Events_H*/
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
