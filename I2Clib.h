#define Fsck 400000
#define BRG_VAL ((FPB/2/Fsck)-2)
// #define TEST_VAL 0x67  //1 Byte that will be writing

void init_I2C2();
int I2CReadEEPROM(int SlaveAddr, int mem_addr, char *i2cData, int len);
int I2CWriteEEPROM(int SlaveAddr, int mem_addr, char *i2cData, int len);
int wait_i2c(int SlaveAddr);