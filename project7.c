/************* Project 7 ***************
 * File name:	Project7.c
 *
 * Author:	Tyler Bendele
 * Date:	March 10, 2022    - Tested with MPLab X
 * Notes: 	This lab combines labs 5 and 6.
 *              
****************************************/

#include <stdio.h>
#include <plib.h>
#include "CerebotMX7cK.h"
#include "config_bits.h"
#include "comm.h"
#include "LCDlib.h"
#include <string.h>
// #include "motorcode.c"
#include "motorcode.h"
// #include "buttonlib.c"
#include "buttonlib.h"

unsigned int step_dir, step_mode, step_delay, motor_code;
int step;

void system_init(void);

char str_buf[32];
//char rdy_msg[] = "\r\n\r\nUART open and ready for business!!\r\n\r\n";

unsigned int count = 0;


main(void)
{
    char mode_txt[5];
    char dir_txt[5];
    unsigned int rpm;
    
    system_init();
    
    //putsU1(rdy_msg);
    
    while(1)
    {
        // demo code
        /*
        while(!getstrU1(str_buf, sizeof(str_buf)));
        
        putsU1("\r\n");
        putsU1(str_buf);
        putsU1("\r\n");
        
        sprintf(str_buf, "Message number %d", count = count+1);
        putsU1(str_buf);
        putsU1("\r\n");
        */
        // while loop tasks
        while(!getstrU1(str_buf, sizeof(str_buf)));   // wait for line of text
        mCNIntEnable(FALSE);                       // Disable CN Interrupts
        writeLCD(0,1);                             // Clear LCD
        
        putsU1("\r\n");                           // echo to LCD
        putsU1(str_buf);
        putsU1("\r\n");
        LCD_puts(str_buf);
        
        sscanf(str_buf,"%s %s %d", dir_txt, mode_txt, &rpm); // Parse the string into the three variables
        
        /* Set the Stepper motor control global variables*/
        if (!(strcmp(dir_txt, "CW"))){
            step_dir = CW;
        } else if (!(strcmp(dir_txt, "CCW"))){
            step_dir = CCW;
        } else {};
        
        if (!(strcmp(mode_txt, "FS"))){
            step_mode = FS;
            step_delay = 60000/(rpm*100*1);
        } else if (!(strcmp(mode_txt, "HS"))){
            step_mode = HS;
            step_delay = 60000/(rpm*100*2);
        } else {};
        mCNIntEnable(TRUE); // Enable CN Interrupt
    } // end while(1)
    return(1);    
} // End of main

void system_init(void)
{
    Cerebot_mx7cK_setup(); // Initialize processor board
    PORTSetPinsDigitalIn(IOPORT_G, BTN1 | BTN2);
    PORTSetPinsDigitalOut(IOPORT_B, SM_LEDS | SM_COILS);
    LATBCLR = SM_LEDS;
    LATBCLR = SM_COILS;
    
    LCD_init();
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
    
    // Initialize UART
    initialize_uart1(19200, ODD_PARITY);
    
    // Enable Interrupts
    INTEnableSystemMultiVectoredInt(); 
    INTEnableInterrupts();
    mT1IntEnable(1);
    mCNIntEnable(1);
}



