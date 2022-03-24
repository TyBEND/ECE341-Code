// Single byte write/read

// MasterWriteI2C2() and MasterREadI2C2() are blocking
// so no IdleI2C2() needed, unlike sTARTi2c2(), etc.

#include <plib.h>
#include "CerebotMX7cK.h"

char BusyI2C2(void) { // non-blocking version of IdleI2C2()
    return(I2C2CONbits.SEN || I2C2CONbits.PEN || I2C2CONbits.RSEN ||
            I2C2CONbits.RCEN || I2C2CONbits.ACKEN || I2C2STATbits.TRSTAT);
} // Returns non-zero value if I2C Controller is busy

#define Fsck 400000
#define BRG_VAL ((FPB/2/Fsck)-2)
#define TEST_VAL 0x67  //1 Byte that will be writing

int main(void){
    
    unsigned char SlaveAddress, i2cbyte, write_err=0;
    char i2cdata[10] = {0,0,0,0,0,0,0,0,0,0};
    int datasz, index;

    // Initialize resources
    Cerebot_mx7cK_setup();
    OpenI2C2( I2C_EN, BRG_VAL); // Enable I2C Peripheral which is physically connected to EE Prom
    if (BusyI2C2()) return(1); // I2C bus not ready... is waiting til all or'ed  bits are cleared
    
    // Create I2C Frame
    SlaveAddress = 0x50;
    i2cdata[0] = ((SlaveAddress << 1) | 0); // Control Byte. Forces rightmost bit to be zero
    i2cdata[1] = 0x03; // Memory Address (MSB)
    i2cdata[2] = 0x41; // Memory Address (LSB)
    i2cdata[3] = TEST_VAL; // Single Data Byte

    // Send I2C Frame to EEPROM
    datasz = 4; // How many bytes are put into array
    index = 0; // Where the array starts
    StartI2C2(); // non-blocking // tells I2C Controller to generate start condition
    IdleI2C2(); // blocks until start condition occurs
    while (datasz--)
        write_err |= MasterWriteI2C2(i2cdata[index++] ); // checks to make sure all ack bytes were triggered
    StopI2C2();
    IdleI2C2(); // blocks until stop condition occurs

    // Technically, main() should never return anything, but ...
    if (write_err) return(1); // Some problem during write
    
    // Poll EEPROM for Write Completion
    StartI2C2();
    IdleI2C2();
    while (MasterWriteI2C2(i2cdata[0])){
        RestartI2C2(); // try restart instead of stop if no ack
        IdleI2C2();
    }
    StopI2C2(); // Got an ACK. Write complete.
    IdleI2C2();
    
    // Initiate a Read from EEPROM
    datasz = 3;
    index = 0;
    StartI2C2();
    IdleI2C2();
    while(datasz--) // Send Control Byte and Memory Address
        MasterWriteI2C2(i2cdata[index++]); // didn't check for ACK
    RestartI2C2(); // Reverse I2C Bus Direction
    IdleI2C2();
    MasterWriteI2C2( (SlaveAddress <<1) | 1); // pastes a 1 into rightmost bit
    i2cbyte = MasterReadI2C2();
    NotAckI2C2(); // End read with a NACK
    IdleI2C2();
    StopI2C2();
    IdleI2C2();
    
    // Check Data for match
    if(i2cbyte == TEST_VAL) PORTSetBits(IOPORT_G, LED4);
    else PORTSetBits(IOPORT_G, LED1);
    
    while(1); // wait here
    
    return (1); // Never!


}