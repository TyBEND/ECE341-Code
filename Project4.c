/************* Project 4 ***************
 * File name:	Project4.c
 *
 * Author:	Tyler Bendele
 * Date:	February 17, 2022    - Tested with MPLab X
 * Notes: 	This project implements timer delay to project 3.
 *              
****************************************/

#include <plib.h>
#include "CerebotMX7cK.h"
#include "Project4.h"

int main()
{
    unsigned int button_status, step_dir, step_mode, step_delay, motor_code, 
            button_delay, step;
    button_delay = 0;
    step = 0;
    system_init();
    
    while(1)
    {
        /* Student supplied code inserted here */
        if(button_delay <= 0){
            button_status = read_buttons();
            decode_buttons(button_status, &step_delay, &step_dir, &step_mode);
            LATBINV = LEDB;
            button_delay = 100;
        }
        if(step <= 0){
            motor_code = stepper_state_machine(step_dir, step_mode);
            output_to_stepper_motor(motor_code);
            LATBINV = LEDC;
            step = step_delay;
        }
        Timer1_delay(1, &button_delay, &step);
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
    OpenTimer1(T1_ON | T1_PS_1_1, (T1_TICK - 1));
}

unsigned int read_buttons(void)
{
    /* Student supplied code inserted here */
    int btn_value = 0;
    btn_value = PORTReadBits(IOPORT_G, BTN1 | BTN2);
    return btn_value;
}

void decode_buttons(unsigned int buttons,
        unsigned int *step_delay, unsigned int *step_dir, unsigned int *step_mode)
{
    /* Student supplied code inserted here */
    switch(buttons)
        {
        case 0:                //neither BTN1 or BTN2 are pressed
            *step_dir = CW;
            *step_mode = HS;
            *step_delay = 20;
            break;
        case 64:               //Just BTN1 is pressed
            *step_dir = CW;
            *step_mode = FS;
            *step_delay = 40;
            break;
        case 128:              //Just BTN2 is pressed
            *step_dir = CCW;
            *step_mode = HS;
            *step_delay = 30;
            break;
        case 192:              //Both BTNs are pressed
            *step_dir = CCW;
            *step_mode = FS;
            *step_delay = 24;
            break;
    }
}
unsigned int stepper_state_machine(unsigned int step_dir, unsigned int step_mode)
{
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


/*
void sw_delay (unsigned int mS)
{
    int i;
    while(mS --) // SW Stop breakpoint
    {
        for (i = 0; i< COUNTS_PER_MS; i++) // 1 ms delay loop
        {
            // do nothing
        }
        LATBINV = LEDA; // Toggle LEDA each ms for instrumentation
    }
}
*/
/* End of Project4.c */
