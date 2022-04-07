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
#include <string.h>
#include "project9.h"
#include "pwm.h"

main(void)
{
    system_init();
    pwm_init(dutyCycle, cycleFrequency);
    while(1)
    {
    }
    return 1;
}
void system_init(void)
{
    Cerebot_mx7cK_setup(); // Initialize processor board, BTNs and LEDs
    PORTSetPinsDigitalIn(IOPORT_G, BTN1 | BTN2);
    LATBCLR = SM_LEDS;
    
    // Initialize timer2 to generate interrupt every ms with GP lvl 2 and SGP lvl 0
    OpenTimer2(T2_ON | T2_PS_1_1, (T2_TICK - 1));
    mT1SetIntPriority(2);
    mT1SetIntSubPriority(0);
    
    // Initialize Change Notice
    mCNOpen(CN_ON, (CN8_ENABLE | CN9_ENABLE), 0);  // Enable CN for BTN1 and BTN2
    mCNSetIntPriority(1);
    mCNSetIntSubPriority(0);
    mCNClearIntFlag();
    mT2ClearIntFlag();
    
    // Enable multi vectored interrupts
    INTEnableSystemMultiVectoredInt(); 
    INTEnableInterrupts();
    mT2IntEnable(1);
    mCNIntEnable(1);
}

