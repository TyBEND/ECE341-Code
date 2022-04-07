#include <plib.h>
#include "CerebotMX7ck.h"
#include "I2Clib.h"

void init_I2C2(){
    OpenI2C2( I2C_EN, BRG_VAL); // Enable I2C Peripheral which is physically connected to EEPROM
}

int I2CReadEEPROM(int SlaveAddr, int mem_addr, char *i2cData, int len)
{
    unsigned char read_err=0;
    unsigned int mem_MSB, mem_LSB, index;
    index = 0;
 
    mem_MSB = mem_addr >> 8;
    mem_LSB = mem_addr & 0x00FF;
    StartI2C2();
    IdleI2C2();
    read_err |= MasterWriteI2C2((SlaveAddr << 1) | 0); // Checks Control Byte
    read_err |= MasterWriteI2C2(mem_MSB); // Checks MSB
    read_err |= MasterWriteI2C2(mem_LSB); // Checks LSB
    RestartI2C2(); // Reverse I2C Bus Direction
    IdleI2C2();
    MasterWriteI2C2( (SlaveAddr << 1) | 1); // pastes a 1 into rightmost bit
    while(len--) // Runs through each byte
    {
        *i2cData = MasterReadI2C2();
        *i2cData++;
        mem_addr++;
        index++;
    
        if(len == 0)      // Checks if there are no more bytes
        {
            NotAckI2C2(); // End read with a NACK
            IdleI2C2();
            StopI2C2();
            IdleI2C2();
        } else {
            AckI2C2();
            IdleI2C2();
        }
        if(read_err == 1)   // Checks to see if error was detected
        {
            return(1);          
        }
    }
    return(0);
}

int I2CWriteEEPROM(int SlaveAddr, int mem_addr, char *i2cData, int len)
{
    unsigned char write_err=0;
    unsigned int mem_MSB, mem_LSB, index;
    index = 0;

    mem_MSB = mem_addr >> 8;
    mem_LSB = mem_addr & 0x00FF;
    
    StartI2C2();
    IdleI2C2();
    write_err |= MasterWriteI2C2((SlaveAddr << 1) | 0); // Checks Control Byte
    write_err |= MasterWriteI2C2(mem_MSB); // Checks MSB
    write_err |= MasterWriteI2C2(mem_LSB); // Checks LSB
    while(len--) // Runs through each byte
    {
        write_err |= MasterWriteI2C2(i2cData[index]); // checks to make sure ack byte is triggered
        index++;
        mem_addr++;
    
        if(len == 0)      // Checks if there are no more bytes
        {
          StopI2C2();
          IdleI2C2();
          wait_i2c(SlaveAddr);
          StopI2C2();
          IdleI2C2();
          return(0);
        }
        if(mem_addr %64 == 0) //Checks to see if there is no remainder left from the address
        {                     // If there is no remainder, it went on to the next page
            StopI2C2();
            IdleI2C2();
            wait_i2c(SlaveAddr);
            StartI2C2();
            IdleI2C2();
            mem_MSB = mem_addr >> 8;
            mem_LSB = mem_addr & 0x00FF;
            write_err |= MasterWriteI2C2((SlaveAddr << 1) | 0); // Checks ack bytes for Control Byte
            write_err |= MasterWriteI2C2(mem_MSB); // Checks ack for MSB
            write_err |= MasterWriteI2C2(mem_LSB); // Checks ack for LSB
        }
        if(write_err == 1)   // Checks to see if error was detected
        {
            return(1);          
        }
    }
    return(0);
}

// Poll EEPROM
int wait_i2c(int SlaveAddr){
    StartI2C2();
    IdleI2C2();
    while (MasterWriteI2C2((SlaveAddr << 1) | 0)){
        RestartI2C2(); // try restart instead of stop if no ack
        IdleI2C2();
    }
    StopI2C2(); // Got an ACK. Write complete.
    IdleI2C2();
}
// Originally in read
  /*
    if((mem_addr + len) > 0x7FFF){ // check if at end of memory
        return(1);                 // return error if true
    }
  
    if(len == 0){  // checks to make sure bytes are being written
        return(1);
    }
 */  
// Originally in write
 /*   
    if((mem_addr + len) > 0x7FFF){ // check if at end of memory
        return(1);                 // return error if true
    }
  
  
    if(len == 0){  // checks to make sure bytes are being written
        return(1);
    }
  */
