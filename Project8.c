#include <plib.h>
#include "CerebotMX7cK.h"
#include "I2Clib.h"

#define LEN 4;

char BusyI2C2(void) { // non-blocking version of IdleI2C2()
    return(I2C2CONbits.SEN || I2C2CONbits.PEN || I2C2CONbits.RSEN ||
            I2C2CONbits.RCEN || I2C2CONbits.ACKEN || I2C2STATbits.TRSTAT);
} // Returns non-zero value if I2C Controller is busy

int main(void){
    
    // Initialize resources
    Cerebot_mx7cK_setup();
    
    init_I2C2();
    if (BusyI2C2()) return(1); // I2C bus not ready... is waiting til all or'ed bits are cleared
    
    // New Variables
    unsigned int SlaveAddress = 0x50;
    unsigned int mem_address = 0x04FB;
    int len = LEN;
    int index, i = 0;
    
    char i2cdataWrite[len];                  // Initialize Write array
	char i2cdataRead[len];                   // Initialize Read array
    
    for (i = 0; i < len; i++)     //Build character array for write
    {
        i2cdataWrite[i] = '0' + i %10;
    }    
    
    // Check read and write errors
    int write_err = I2CWriteEEPROM(SlaveAddress, mem_address, i2cdataWrite, len);
    int read_err = I2CReadEEPROM(SlaveAddress, mem_address, i2cdataRead, len);
    
    if (write_err != 0)
	{
		return 1;
	}
    
    if (read_err != 0)
	{
		return 1;
	}
    
    for (i = 0; i < len; i++)
	{
		if (i2cdataRead[i] != i2cdataWrite[i])
		{
			return 1;
		}
	}
	return 0;
}