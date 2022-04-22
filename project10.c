/************* Project 10 ***************
 * File name:	Project10.c
 *
 * Author:	Tyler Bendele
 * Date:	April 21, 2022    
 * Notes: 	Input Capture with DC Motor
 *              
****************************************/

#include <stdio.h>
#include <plib.h>
#include "CerebotMX7cK.h"
#include "LCDlib.h"
#include "project10.h"
#include "pwm.h"
#include "input_capture.h"

#define ft2 10000
#define PeriodR2 ft2 - 1

extern float RPS;

main(void)
{  
    system_init();
    pwm_init(40, 1000);
    input_capture_init();
    LCD_init();
    writeLCD(0,1);
    LCD_puts("PWM = 40%");
    while(1)
    {
        debounce(100);          //Set a delay of 100 ms to determine when line 2 of LCD is updated
        mCNIntEnable(0);        // Protect LCD by disabling CN int
        writeLCD(0, (0x40));   // Move to second line of LCD
        LCD_puts("\r                "); // Clear Second line
        char rps_array[8];
        sprintf(rps_array, "\rRPS = %5.2f", RPS);
        LCD_puts(rps_array);
        mCNIntEnable(1);        // Re-enable CN int
    }
    return 1;
}
void system_init(void)
{
    Cerebot_mx7cK_setup(); // Initialize processor board, BTNs and LEDs
    PORTSetPinsDigitalIn(IOPORT_G, BTN1 | BTN2);
    PORTSetPinsDigitalOut(IOPORT_B, SM_LEDS);
    LATBCLR = SM_LEDS;
    
    // Initialize timer2 to generate interrupt every ms with GP lvl 2 and SGP lvl 0
    OpenTimer2(T2_ON | T2_PS_1_1 | T2_SOURCE_INT, (PeriodR2));
    mT2SetIntPriority(2);
    mT2SetIntSubPriority(1);
    
    // Initialize timer3 with a pre-scale value of 256 with PR3 having a maximum interval
    // GP lvl 2 and SGP lvl 2
    OpenTimer3(T3_ON | T3_PS_1_256 | T3_SOURCE_INT, 0xFFFF);
    mT3SetIntPriority(2);
    mT3SetIntSubPriority(2);
    
    // Initialize Change Notice
    mCNOpen(CN_ON, (CN8_ENABLE | CN9_ENABLE), 0);  // Enable CN for BTN1 and BTN2
    mCNSetIntPriority(1);
    mCNSetIntSubPriority(0);
    
    // Clear Flags
    mCNClearIntFlag();
    mT2ClearIntFlag();
    mT3ClearIntFlag();
    
    // Enable multi vectored interrupts
    INTEnableSystemMultiVectoredInt(); 
    INTEnableInterrupts();
    
    mT2IntEnable(1);
    mT3IntEnable(1);
    mCNIntEnable(1);   
}

void __ISR(_CHANGE_NOTICE_VECTOR, IPL2) change_notice_ISR(void)
{
    /* User ISR code inserted here */
    LATBSET = LEDB;
    debounce(20);
    int button_status;
    button_status = read_buttons();
    decode_buttons(button_status);
    /* Required to clear the interrupt flag in the ISR */
    mCNClearIntFlag(); // Macro function
    LATBCLR = LEDB;
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
            pwm_set(40);
            writeLCD(0,1);
            LCD_puts("PWM = 40%");
            break;
        case 64:               //Just BTN1 is pressed
            pwm_set(65);
            writeLCD(0,1);
            LCD_puts("PWM = 65%");
            break;
        case 128:              //Just BTN2 is pressed
            pwm_set(80);
            writeLCD(0,1);
            LCD_puts("PWM = 80%");
            break;
        case 192:              //Both BTNs are pressed
            pwm_set(95);
            writeLCD(0,1);
            LCD_puts("PWM = 95%");
            break;
    }
}

void debounce(unsigned int mS)
{
/* Use code from Listing 4 */
unsigned int tWait, tStart;
    tStart=ReadCoreTimer(); // Read core timer count - SW Start breakpoint
    tWait= (CORE_MS_TICK_RATE * mS); // Set time to wait 
    while((ReadCoreTimer() - tStart) < tWait); // Wait for the time to pass
}         

