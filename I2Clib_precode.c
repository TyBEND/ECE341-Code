#include <plib.h>
#include "CerebotMX7ck.h"
#include "I2C.h"

/* void init_I2C2(int SCK_FREQ){
    OpenI2C2( I2C_EN, BRG_VAL); // Enable I2C Peripheral which is physically connected to EE Prom
    }
*/

/* int I2CReadEEPROM(int SlaveAddress, int mem_addr, char *i2cData, int len)

*/

int I2CWriteEEPROM(int SlaveAddr, int mem_addr, char *i2cData, int len) {
  unsigned char write_err=0;
  unsigned int mem_MSB, mem_LSB, index;
  
  if((mem_addr + len) > 0x7FFF){ // check if at end of memory
      return(1);                 // return error if true
  }
  
  if(i2cData = NULL){        //check to make sure buffer is given a value 
      reutrn(1);
  }
  
  if(len == 0){  // checks to make sure bytes are being written
      return(1);
  }
  
  mem_MSB = mem_addr >> 8;
  mem_LSB = mem_addr & 0x00FF;
  index = 0;
  StartI2C2();
  IdleI2C2();
  write_err |= MasterWriteI2C2((SlaveAddr << 1) | 0); // Checks ack bytes for Control Byte
  write_err |= MasterWriteI2C2(mem_MSB); // Checks ack for MSB
  write_err |= MasterWriteI2C2(mem_LSB); // Checks ack for LSB
  while(len > 0) // Runs through each byte
  {
      write_err |= MasterWriteI2C2(i2cdata[index]); // checks to make sure ack byte is triggered
      index++;
      mem_addr++;
      len--;
    
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
          StopI2C2();
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
      return(0);
  }
} 
  
