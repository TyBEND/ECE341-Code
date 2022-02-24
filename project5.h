/************* Project 5 ***************
 *
 * File name:	Project5.h
 * Author:		Tyler Bendele 
 * Rev Date:	February 24, 2022
 *
****************************************/

#define CW 1
#define CCW 0
#define FS 1
#define HS 0
#define T1_PRESCALE 1
#define TOGGLES_PER_SEC 1000
#define T1_TICK (FPB/T1_PRESCALE/TOGGLES_PER_SEC)

/* Function prototypes */
void system_init(void);
unsigned int read_buttons(void);
void decode_buttons(unsigned int buttons);
unsigned int stepper_state_machine(unsigned int step_dir, unsigned int step_mode);
void output_to_stepper_motor(unsigned int motor_code);
void __ISR(_CHANGE_NOTICE_VECTOR, IPL1) change_notice_ISR(void);
void __ISR(_TIMER_1_VECTOR, IPL2) Timer1_ISR(void);
void debounce(unsigned int mS);


//void Timer1_delay(int delay, unsigned int *button_delay, unsigned int *step);



/* End of Project5.h */