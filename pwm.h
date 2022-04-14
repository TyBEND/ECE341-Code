/************* Project 9 ***************
 *
 * File name:	pwm.h
 * Author:		Tyler Bendele 
 * Rev Date:	April 7, 2022
 * Notes: Pulse Width Modulation Functions
 * 
****************************************/


/* Function prototypes */
int pwm_init(int dutyCycle, int cycleFrequency);
int pwm_set(int dutyCycle);
// void __ISR( _TIMER_2_VECTOR, IPL2) Timer2_ISR(void);