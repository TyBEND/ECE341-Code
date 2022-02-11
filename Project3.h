/************* Project 3 ***************
 *
 * File name:	Project3.h
 * Author:		Tyler Bendele 
 * Rev Date:	February 10, 2022
 *
****************************************/

#define CW = 0;
#define CCW = 1;
#define FS = 1;
#define HS = 0;
/* Function prototypes */
void system_init(void);
unsigned int read_buttons(void);
void decode_buttons(unsigned int buttons,unsigned int *step_delay,
        unsigned int *step_dir, unsigned int *step_mode);
unsigned int stepper_state_machine(unsigned int step_dir, unsigned int step_mode);
void output_to_stepper_motor();
void sw_delay (unsigned int mS);

/* End of Project3.h */