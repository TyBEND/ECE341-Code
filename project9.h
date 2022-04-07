/************* Project 9 ***************
 *
 * File name:	Project9.h
 * Author:		Tyler Bendele 
 * Rev Date:	April 7, 2022
 *
****************************************/

#define T2_PRESCALE 1
#define TOGGLES_PER_SEC 1000
#define T2_TICK (FPB/T2_PRESCALE/TOGGLES_PER_SEC)


/* Function prototypes */
void system_init(void);
