/************* Project 6 ***************
 * File name:	LCDlib.c
 *
 * Author:	Tyler Bendele
 * Date:	March 3, 2022    - Tested with MPLab X
 * Notes: 	This project introduces the LCD and PMP control
 *              
****************************************/

#include <plib.h>
#include "CerebotMX7cK.h"
#include "LCDlib.h"

int main()
{
    system_init();
    char string1[] = "Does Dr J prefer PIC32 or FPGA??";
    char string2[] = "Answer: \116\145\151\164\150\145\162\041";
    while(1)
    {
        LCD_puts(string1);
        sw_delay(5000);
 
        LCD_puts(string2);
        sw_delay(5000);
    }
    return 0;
}
void system_init(void)
{
    Cerebot_mx7cK_setup(); // Initialize processor board
    /* PMP Initialization */
    int cfg1 = PMP_ON|PMP_READ_WRITE_EN|PMP_READ_POL_HI|PMP_WRITE_POL_HI;
    int cfg2 = PMP_DATA_BUS_8 | PMP_MODE_MASTER1 |
               PMP_WAIT_BEG_1 | PMP_WAIT_MID_2 | PMP_WAIT_END_1;
    int cfg3 = PMP_PEN_0; // only PMA0 enabled
    int cfg4 = PMP_INT_OFF; // no interrupts used
    mPMPOpen(cfg1, cfg2, cfg3, cfg4);
    
    sw_delay(20);
    
    /* Set Function to 8 bit data, 2 line display, 5X8 dot font */
    PMPMasterWrite(0x38);
    sw_delay(37);
    
    /* Set Display to Display on, Cursor on, Blink Cursor on*/
    PMPMasterWrite(0x0f);
    sw_delay(37);
    
    /* Clear Display */
    PMPMasterWrite(0x01);
    sw_delay(1.52);
}
void LCD_putc(char c)
{
    char x;
    while(busyLCD());
    x = readLCD(c);
    switch (c)
    {
        case '\r':
            if ( x >= 0x00 && x <= 0x0F) //checks if '\r' is detected on first line
            {
                writeLCD(0, (0x00 | BIT_7)); // reset to start of first line
            }
            else {
                writeLCD(0, (0x40 | BIT_7)); // reset to start of second line
            }
            break;
        case '\n':
            if (x >= 0x00 && x <= 0x0F) // check if on first line when '\n' is detected
            {
                writeLCD(0, (0x40 | BIT_7)); //reposition to second line
            }
            else {
                writeLCD(0, (0x00 | BIT_7)); //reposition to first line
            }
            break;
        default:
            x = readLCD(0) & 0x7F;
            if(x > 0x0F && x < 0x40)
            {
                writeLCD(0, (0x40 | BIT_7));
            }
            if(x > 0x4F)
            {
                writeLCD(0, (0x00 | BIT_7));
            }
            writeLCD(1,c);
            break;
    }        
}

void writeLCD(int addr, char c)
{
 while(busyLCD()); // Wait for LCD to be ready
 PMPSetAddress(addr); // Set LCD RS control
 PMPMasterWrite(c); // initiate write sequence
} // End of writeLCD

char readLCD(int addr)
{
 PMPSetAddress(addr); // Set LCD RS control
 mPMPMasterReadByte(); // initiate dummy read sequence
 return mPMPMasterReadByte();// read actual data
} // End of readLCD

void LCD_puts(char *char_string)
{
    while(*char_string) // Look for end of string NULL character
    {
        LCD_putc(*char_string); // Write character to LCD
        char_string = char_string + 1; // Increment string pointer
 }
} //End of LCD_puts

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
}                       // SW Stop breakpoint

int busyLCD(void)
{
    return readLCD(0) & 0x80;
}

/*
 if (x >= 0x10 && x <= 0x3F) // Check to see if character is off screen on first line
            {
                writeLCD(0, (0xC0 | BIT_7)); // reposition to start of second line
            }
 */
/* End of LCDlib.c */