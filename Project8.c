#include <plib.h>
#include "CerebotMX7cK.h"
#include "I2Clib.h"
#include "LCDlib.h"

#define LEN 100

char BusyI2C2(void) { // non-blocking version of IdleI2C2()
    return(I2C2CONbits.SEN || I2C2CONbits.PEN || I2C2CONbits.RSEN ||
            I2C2CONbits.RCEN || I2C2CONbits.ACKEN || I2C2STATbits.TRSTAT);
} // Returns non-zero value if I2C Controller is busy

int main(void){
    
    // Initialize resources
    Cerebot_mx7cK_setup();
    LCD_init();
    init_I2C2();
    if (BusyI2C2()) return(1); // I2C bus not ready... is waiting til all or'ed bits are cleared
    
    // New Variables
    unsigned int SlaveAddress = 0x50;
    unsigned int mem_address = 0x04FB;
    int len = LEN;
    int i = 0;
    
    char i2cdataWrite[LEN];                  // Initialize Write array

    for (i = 0; i < len; i++)     //Build character array for write
    {
        i2cdataWrite[i] = '0' + i %10;
    }    
    
    // Check read and write errors
    int write_err = I2CWriteEEPROM(SlaveAddress, mem_address, i2cdataWrite, len);
    
    char i2cdataRead[LEN];                   // Initialize Read array
        
    int read_err = I2CReadEEPROM(SlaveAddress, mem_address, i2cdataRead, len);
     
    for (i = 0; i < len; i++)
	{
		if (i2cdataRead[i] != i2cdataWrite[i])
		{
           		writeLCD(0,1);
            		LCD_puts("Test Failed");
			return 1;
		}
	}
    writeLCD(0,1);
    LCD_puts("Test Passed");
    return 0;
}
