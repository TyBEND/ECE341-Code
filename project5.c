/************* Project 5 ***************
 * File name:	Project5.c
 *
 * Author:	Tyler Bendele
 * Date:	February 24, 2022    - Tested with MPLab X
 * Notes: 	This project implements process speed control using interrupts.
 *              
****************************************/

#include <plib.h>
#include "CerebotMX7cK.h"
#include "project5.h"

unsigned int step_dir, step_mode, step_delay, motor_code;
int step;

int main()
{
    step = 0;
    system_init();
    step_dir = CW;
    step_mode = HS;
    step_delay = 20;
    while(1)
    {
    }
    return 1;
}

void system_init(void)
{
    Cerebot_mx7cK_setup(); // Initialize processor board
    PORTSetPinsDigitalIn(IOPORT_G, BTN1 | BTN2);
    PORTSetPinsDigitalOut(IOPORT_B, SM_LEDS | SM_COILS);
    LATBCLR = SM_LEDS;
    LATBCLR = SM_COILS;
    
    // Initialize timer1
    OpenTimer1(T1_ON | T1_PS_1_1, (T1_TICK - 1));
    mT1SetIntPriority(2);
    mT1SetIntSubPriority(0);
   
    
    // Initialize Change Notice
    mCNOpen(CN_ON, (CN8_ENABLE | CN9_ENABLE), 0);  // Enable CN for BTN1 and BTN2
    mCNSetIntPriority(1);
    mCNSetIntSubPriority(0);
    mCNClearIntFlag();
    mT1ClearIntFlag();
    
    // Enable multi vectored interrupts
    INTEnableSystemMultiVectoredInt(); 
    INTEnableInterrupts();
    mT1IntEnable(1);
    mCNIntEnable(1);
}

void __ISR(_CHANGE_NOTICE_VECTOR, IPL1) change_notice_ISR(void)
{
    /* User ISR code inserted here */
    LATBSET = LEDC;
    debounce(20);
    int button_status;
    button_status = read_buttons();
    decode_buttons(button_status);

    /* Required to clear the interrupt flag in the ISR */
    mCNClearIntFlag(); // Macro function
    LATBCLR = LEDC;
}

unsigned int read_buttons(void)
{
    // Student supplied code inserted here
    int btn_value = 0;
    btn_value = PORTReadBits(IOPORT_G, BTN1 | BTN2);
    return btn_value;
}

void decode_buttons(unsigned int buttons)
{
    /* Student supplied code inserted here */
    switch(buttons)
        {
        case 0:                //neither BTN1 or BTN2 are pressed
            step_dir = CW;
            step_mode = HS;
            step_delay = 20;
            break;
        case 64:               //Just BTN1 is pressed
            step_dir = CW;
            step_mode = FS;
            step_delay = 40;
            break;
        case 128:              //Just BTN2 is pressed
            step_dir = CCW;
            step_mode = HS;
            step_delay = 30;
            break;
        case 192:              //Both BTNs are pressed
            step_dir = CCW;
            step_mode = FS;
            step_delay = 24;
            break;
    }
}
void __ISR(_TIMER_1_VECTOR, IPL2) Timer1_ISR(void)
{
    /* User generated code to service the interrupt is inserted here */
    // set a 1 ms interval
    LATBINV = LEDA;
    step = step - 1;
    if(step <= 0){
            motor_code = stepper_state_machine(step_dir, step_mode);
            output_to_stepper_motor(motor_code);
            step = step_delay;
        }
    mT1ClearIntFlag(); // Macro function to clear the interrupt flag
}

unsigned int stepper_state_machine(unsigned int step_dir, unsigned int step_mode)
{
    LATBINV = LEDB;
    enum {S0=0, S0_5, S1, S1_5, S2, S2_5, S3, S3_5};
    static unsigned int pstate;
    const unsigned int motor_code[] = {0x0A, 0x08, 0x09, 0x01, 0x05, 0x04, 0x06, 0x02};
    
    switch (pstate)
    {
        case S0:
            if (step_dir == CW)
            {
                if (step_mode == HS) pstate = S0_5;
                else
                    pstate = S1; 
            }
            else {
                if (step_mode == HS) pstate = S3_5;
                else {
                    pstate = S3;
                }
            }
            break;
        case S0_5:
            if (step_dir == CW)
            {
                if (step_mode == HS) pstate = S1;
                else
                    pstate = S1_5; 
            }
            else {
                if (step_mode == HS) pstate = S0;
                else {
                    pstate = S3_5;
                }
            }
            break;
        case S1:
            if (step_dir == CW)
            {
                if (step_mode == HS) pstate = S1_5;
                else
                    pstate = S2; 
            }
            else {
                if (step_mode == HS) pstate = S0_5;
                else {
                    pstate = S0;
                }
            }
            break;
        case S1_5:
            if (step_dir == CW)
            {
                if (step_mode == HS) pstate = S2;
                else
                    pstate = S2_5; 
            }
            else {
                if (step_mode == HS) pstate = S1;
                else {
                    pstate = S0_5;
                }
            }
            break;
        case S2:
            if (step_dir == CW)
            {
                if (step_mode == HS) pstate = S2_5;
                else
                    pstate = S3; 
            }
            else {
                if (step_mode == HS) pstate = S1_5;
                else {
                    pstate = S1;
                }
            }
            break;
        case S2_5:
            if (step_dir == CW)
            {
                if (step_mode == HS) pstate = S3;
                else
                    pstate = S3_5; 
            }
            else {
                if (step_mode == HS) pstate = S2;
                else {
                    pstate = S1_5;
                }
            }
            break;
        case S3:
            if (step_dir == CW)
            {
                if (step_mode == HS) pstate = S3_5;
                else
                    pstate = S0; 
            }
            else {
                if (step_mode == HS) pstate = S2_5;
                else {
                    pstate = S2;
                }
            }
            break;
        case S3_5:
            if (step_dir == CW)
            {
                if (step_mode == HS) pstate = S0;
                else
                    pstate = S0_5; 
            }
            else {
                if (step_mode == HS) pstate = S3;
                else {
                    pstate = S2_5;
                }
            }
            break;
    }
    return motor_code[pstate];
}

void output_to_stepper_motor(unsigned int motor_code)
{
    LATBCLR = SM_COILS;
    LATBSET = motor_code << 7;
}

void debounce(unsigned int mS)
{
/* Use code from Listing 4 */
unsigned int tWait, tStart;
    tStart=ReadCoreTimer(); // Read core timer count - SW Start breakpoint
    tWait= (CORE_MS_TICK_RATE * mS); // Set time to wait 
    while((ReadCoreTimer() - tStart) < tWait); // Wait for the time to pass
}         



/*
void Timer1_delay(int delay, unsigned int *button_delay, unsigned int *step)
{
    while(delay--)
    {    
        while(!mT1GetIntFlag());
        mT1ClearIntFlag();
        LATBINV = LEDA;
        *button_delay = *button_delay - 1;
        *step = *step - 1;
    }
}
*/


/* End of Project5.c */
