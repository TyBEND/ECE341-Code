/************* Project 6 ***************
 *
 * File name:	LCDlib.h
 * Author:		Tyler Bendele 
 * Rev Date:	March 3, 2022
 *
****************************************/

/* Defined Variables */
#define COUNTS_PER_MS 8889
/* Function prototypes */
void system_init(void);
void sw_delay (unsigned int mS);
void LCD_putc(char c);
void writeLCD(int addr, char c);
char readLCD(int addr);
void LCD_puts(char *char_string);
int busyLCD(void);

/* End of LCDlib.h */