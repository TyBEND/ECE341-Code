/************* Project 10 ***************
 * File name:	input_capture.c
 *
 * Author:	Tyler Bendele
 * Date:	April 21, 2022    
 * Notes: 	Input Capture with DC motor
 *              
****************************************/
#include <stdio.h>
#include <plib.h>
#include "CerebotMX7cK.h"
#include "LCDlib.h"
#include "project10.h"
#include "pwm.h"
#include "input_capture.h"

#define fPB 10000000               // Peripheral Bus clock at 10MHz
#define tps 256                    // Prescale value for timer 3
float RPS;


void input_capture_init(void)
{
    PORTSetPinsDigitalIn(IOPORT_D, (BIT_3 | BIT_12)); //Initialize MTR_SA and MTR_SB
    LATDCLR = (BIT_1 | BIT_3 | BIT_12);
    mIC5ClearIntFlag(); // Clear pending Input Capture Interrupt
    
    OpenCapture5(IC_ON | IC_CAP_16BIT | IC_IDLE_STOP | IC_FEDGE_FALL | IC_TIMER3_SRC
                        | IC_INT_1CAPTURE | IC_EVERY_FALL_EDGE);
    
    ConfigIntCapture5(IC_INT_ON | IC_INT_PRIOR_3 | IC_INT_SUB_PRIOR_0);
}

void __ISR(_INPUT_CAPTURE_5_VECTOR, IPL3) Capture5(void)
{
    static unsigned int con_buf[4];      //Declare an input capture buffer
    // Three Time capture variables:
    static unsigned short int t_new;     // Most recent captured time
    static unsigned short int t_old = 0; // Previous time capture
    static unsigned short int time_diff; // Time between captures

    LATBINV = LEDD;                      // Toggle LEDD on each input capture interrupt
    ReadCapture5(con_buf);               // Read captures into buffer
    t_new = con_buf[0];                  // Save time of event
    time_diff = t_new - t_old;           // Compute elapsed time in timer ?ticks?
    t_old = t_new;                       // Replace previous time capture with new
    // Compute motor speed in RPS (revolutions per second) and save as global variable
    float speed = 0;  
    static unsigned short speed_array[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int i = 0;
    // Shift each value in the array
    for (i = 15; i > 0; i--)
    {
        speed_array[i] = speed_array[i - 1];
    }
    
    speed_array[0] = time_diff;
    
 	for (i = 0; i < 16; i++)
 	{
 		speed += speed_array[i];        // Find the sum of the elements in the array
 	}
 	speed = (speed/16.0);         // Find average by dividing by the total    
 	speed = (fPB/(speed * tps));       // Use average speed value to determine rps value
 	RPS = speed;     // Save rps value as global variable 
    mIC5ClearIntFlag(); // Clears interrupt flag

}


void __ISR( _TIMER_3_VECTOR, IPL2) Timer3_ISR(void)
{
    LATBINV = LEDC;   // Use LEDC to measure Timer 3 timing
    mT3ClearIntFlag();
}