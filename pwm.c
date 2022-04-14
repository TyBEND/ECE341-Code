/************* Project 9 ***************
 * File name:	Project9.c
 *
 * Author:	Tyler Bendele
 * Date:	April 7, 2022    
 * Notes: 	Pulse Width Modulation with DC Motor
 *              
****************************************/
#include <stdio.h>
#include <plib.h>
#include "CerebotMX7cK.h"
#include "LCDlib.h"
#include "project9.h"
#include "pwm.h"

int pwm_init(int dutyCycle, int cycleFrequency)
{
    int nOC3RS_var = (dutyCycle*(10000)/100);
    int nOC3R_var = nOC3RS_var;
    mOC3ClearIntFlag();
    OpenOC3(OC_ON | OC_TIMER_MODE16 | OC_TIMER2_SRC | OC_PWM_FAULT_PIN_DISABLE, nOC3RS_var, nOC3R_var);
    SetDCOC3PWM(nOC3RS_var);
}

int pwm_set(int dutyCycle)
{
    SetDCOC3PWM(dutyCycle*(10000)/100);
}

void __ISR( _TIMER_2_VECTOR, IPL2) Timer2_ISR(void)
{
    LATBINV = LEDA;
    mT2ClearIntFlag();
}