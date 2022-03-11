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
//#include "config_bits.h"
#include "comm.h"
#include "LCDlib.h"
#include <string.h>
#include "project7.h"
// #include "motorcode.c"
// #include "motorcode.h"
// #include "buttonlib.c"
// #include "buttonlib.h"

unsigned int step_dir, step_mode, step_delay, motor_code = 20;
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
    
    putsU1("CW FS 5");
    
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
/*Button Library*/
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

void debounce(unsigned int mS)
{
/* Use code from Listing 4 */
unsigned int tWait, tStart;
    tStart=ReadCoreTimer(); // Read core timer count - SW Start breakpoint
    tWait= (CORE_MS_TICK_RATE * mS); // Set time to wait 
    while((ReadCoreTimer() - tStart) < tWait); // Wait for the time to pass
}         




/*Stepper Library*/
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


