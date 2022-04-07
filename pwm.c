/************* Project 9 ***************
 * File name:	Project9.c
 *
 * Author:	Tyler Bendele
 * Date:	April 7, 2022    
 * Notes: 	Pulse Width Modulation with DC Motor
 *              
****************************************/
#include "CerebotMX7cK.h"
#include "pwm.h"

int pwm_init(int dutyCycle, int cycleFrequency)
{
    int OC_configure_bits = OC_ON | OC_TIMER_MODE16 | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE;
    OpenOC3(OC_configure_bits, nOC3RS, nOC3R);
    
}

int pwm_set(int dutyCycle)
{
    
}

void __ISR( _TIMER_2_VECTOR, IPL2) Timer2_ISR(void)
{
    LATBINV = LEDA;
    mT2ClearIntFlag();
}