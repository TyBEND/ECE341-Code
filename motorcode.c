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
#include "motorcode.h"

extern unsigned int step_dir, step_mode, step_delay, motor_code;
extern int step;

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

/* End of Project5.c */
