// os.c
// Runs on LM4F120/TM4C123/MSP432
// Lab 2 starter file.
// Daniel Valvano
// February 20, 2016

#include <stdint.h>
#include "os.h"
#include "../inc/CortexM.h"
#include "../inc/BSP.h"

// function definitions in osasm.s
void StartOS(void);


tcbType tcbs[NUMTHREADS];
tcbType *RunPt;
int32_t Stacks[NUMTHREADS][STACKSIZE];
uint32_t Task1measure = 0;  // Measurement  for 1st task
uint32_t Task2measure = 0;  // Measurement  for 2nd task

void (*Thread1implementation)(void) = 0; // thread 1
void (*Thread2implementation)(void) = 0; // thread 2

int32_t Main_Mail;
uint32_t Main_Mail_Input = 0;
uint64_t Measurement  = 0;

// ******** OS_Init ************
// Initialize operating system, disable interrupts
// Initialize OS controlled I/O: systick, bus clock as fast as possible
// Initialize OS global variables
// Inputs:  none
// Outputs: none
void OS_Init(void){
  DisableInterrupts();
  BSP_Clock_InitFastest();// set processor clock to fastest speed
  // initialize any global variables as needed
  //***YOU IMPLEMENT THIS FUNCTION*****

    
}

void StackSettings(int i){
  //***YOU IMPLEMENT THIS FUNCTION*****
	
	tcbs[i].sp = &Stacks[i][STACKSIZE-16];    // thread stack pointer
    Stacks[i][STACKSIZE-1] = 0x01000000;    // Enable thumb bit in PSR
                                            // Stack[i][STACKSIZE-2] is for PC
    Stacks[i][STACKSIZE-3] = 0x14141414;    // R14 (LR)
    Stacks[i][STACKSIZE-4] = 0x12121212;    // R12
    Stacks[i][STACKSIZE-5] = 0x03030303;    // R3
    Stacks[i][STACKSIZE-6] = 0x02020202;    // R2
    Stacks[i][STACKSIZE-7] = 0x01010101;    // R1
    Stacks[i][STACKSIZE-8] = 0x00000000;    // R0
    Stacks[i][STACKSIZE-9] = 0x11111111;    // R11
    Stacks[i][STACKSIZE-10] = 0x10101010;   // R10
    Stacks[i][STACKSIZE-11] = 0x09090909;   // R9
    Stacks[i][STACKSIZE-12] = 0x08080808;   // R8
    Stacks[i][STACKSIZE-13] = 0x07070707;   // R7
    Stacks[i][STACKSIZE-14] = 0x06060606;   // R6
    Stacks[i][STACKSIZE-15] = 0x05050505;   // R5
    Stacks[i][STACKSIZE-16] = 0x04040404;   // R4
}

//******** OS_AddThreads ***************
// Add four main threads to the scheduler
// Inputs: function pointers to four void/void main threads
// Outputs: 1 if successful, 0 if this thread can not be added
// This function will only be called once, after OS_Init and before OS_Launch
int OS_AddThreads(void(*thread0)(void),
                  void(*thread1)(void),
                  void(*thread2)(void),
                  void(*thread3)(void)){
// initialize TCB circular list
// initialize RunPt
// initialize four stacks, including initial PC
  //***YOU IMPLEMENT THIS FUNCTION*****
		
		
    // initializing TCB circular list
		tcbs[0].forward = &tcbs[1];	
		tcbs[1].forward = &tcbs[2];
		if (thread3) {
			tcbs[2].forward = &tcbs[3];
			tcbs[3].forward = &tcbs[0];
		} else {
			tcbs[2].forward = &tcbs[0];
		}
	
    // initializing RunPt, 
		// thread0 will run First
    RunPt = &tcbs[0];

    // initializing Four stacks, including initial PC
    StackSettings(0);
		StackSettings(1);
		StackSettings(2);
		StackSettings(3);
		
    Stacks[0][STACKSIZE-2] = (int32_t)(thread0);     
    Stacks[1][STACKSIZE-2] = (int32_t)(thread1);
    Stacks[2][STACKSIZE-2] = (int32_t)(thread2);
    Stacks[3][STACKSIZE-2] = (int32_t)(thread3);
		
    return 1;     // successful
}

//******** OS_AddThreads3 ***************
// add three foregound threads to the scheduler
// This is needed during debugging and not part of final solution
// Inputs: three pointers to a void/void foreground tasks
// Outputs: 1 if successful, 0 if this thread can not be added
int OS_AddThreads3(void(*task0)(void),
                 void(*task1)(void),
                 void(*task2)(void)){ 
// initialize TCB circular list (same as RTOS project)
// initialize RunPt
// initialize four stacks, including initial PC
  //***YOU IMPLEMENT THIS FUNCTION*****
    return OS_AddThreads(task0, task1, task2, 0); // Here returns 1 if the process is succesful
}
                 
//******** OS_AddPeriodicEventThreads ***************
// Add two background periodic event threads
// Typically this function receives the highest priority
// Inputs: pointers to a void/void event thread function2
//         periods given in units of OS_Launch (Lab 2 this will be msec)
// Outputs: 1 if successful, 0 if this thread cannot be added
// It is assumed that the event threads will run to completion and return
// It is assumed the time to run these event threads is short compared to 1 msec
// These threads cannot spin, block, loop, sleep, or kill
// These threads can call OS_Signal
int OS_AddPeriodicEventThreads(void(*thread1)(void), uint32_t period1,
  void(*thread2)(void), uint32_t period2){
  //***YOU IMPLEMENT THIS FUNCTION*****

		Thread1implementation = thread1; // It sets the first periodic task to thread 1
		Task1measure = period1;          // sets the period for Task 1
		Thread2implementation = thread2; // sets 2nd periodic task to thread 2
		Task2measure = period2;          // sets the period for task 2
		return 1; // succesful
}

//******** OS_Launch ***************
// Start the scheduler, enable interrupts
// Inputs: number of clock cycles for each time slice
// Outputs: none (does not return)
// Errors: theTimeSlice must be less than 16,777,216
void OS_Launch(uint32_t theTimeSlice){
  STCTRL = 0;                  // disable SysTick during setup
  STCURRENT = 0;               // any write to current clears it
  SYSPRI3 =(SYSPRI3&0x00FFFFFF)|0xE0000000; // priority 7
  STRELOAD = theTimeSlice - 1; // reload value
  STCTRL = 0x00000007;         // enable, core clock and interrupt arm
  StartOS();                   // start on the first task
}
// runs every ms
void Scheduler(void){ // every time slice
  // run any periodic event threads if needed
  // implement round robin scheduler, update RunPt
  //***YOU IMPLEMENT THIS FUNCTION*****
	
	Measurement ++;
	
	if (Measurement  % Task1measure == 0) 
		Thread1implementation();

	if (Measurement  % Task2measure == 0) 
		Thread2implementation();
	
	RunPt = RunPt->forward;  // it forwards the process in STACK
}

// ******** OS_InitSemaphore ************
// Initialize counting semaphore
// Inputs:  pointer to a semaphore
//          initial value of semaphore
// Outputs: none
void OS_InitSemaphore(int32_t *semaPt, int32_t value){
  //***YOU IMPLEMENT THIS FUNCTION*****
	long counting_semaphore = StartCritical();
	*semaPt = value;
	EndCritical(counting_semaphore);
}

// ******** OS_Wait ************
// Decrement semaphore
// Lab2 spinlock (does not suspend while spinning)
// Lab3 block if less than zero
// Inputs:  pointer to a counting semaphore
// Outputs: none
void OS_Wait(int32_t *semaPt){
 DisableInterrupts();
  while(*semaPt == 0){
    EnableInterrupts();    // interrupts can occur here
    DisableInterrupts();
  }
  *semaPt = *semaPt - 1;
  EnableInterrupts();
}

// ******** OS_Signal ************
// Increment semaphore
// Lab2 spinlock
// Lab3 wakeup blocked thread if appropriate
// Inputs:  pointer to a counting semaphore
// Outputs: none
void OS_Signal(int32_t *semaPt){
//***YOU IMPLEMENT THIS FUNCTION*****
	DisableInterrupts();
  *semaPt = *semaPt + 1;
  EnableInterrupts();
}


	

// ******** OS_MailBox_Init ************
// Initialize communication channel
// Producer is an event thread, consumer is a main thread
// Inputs:  none
// Outputs: none
void OS_MailBox_Init(void){
  // include data field and semaphore
  //***YOU IMPLEMENT THIS FUNCTION*****
	OS_InitSemaphore(&Main_Mail, 0);
}

// ******** OS_MailBox_Send ************
// Enter data into the MailBox, do not spin/block if full
// Use semaphore to synchronize with OS_MailBox_Recv
// Inputs:  data to be sent
// Outputs: none
// Errors: data lost if MailBox already has data
void OS_MailBox_Send(uint32_t data){
  //***YOU IMPLEMENT THIS FUNCTION*****
	long counting_semaphore = StartCritical();
	Main_Mail_Input = data;    //  copy data to Main_Mail's data field
	EndCritical(counting_semaphore);
  OS_Signal(&Main_Mail);     //  signal OS
  
}

// ******** OS_MailBox_Recv ************
// retreive Main_Mail from the MailBox
// Use semaphore to synchronize with OS_MailBox_Send
// Lab 2 spin on semaphore if mailbox empty
// Lab 3 block on semaphore if mailbox empty
// Inputs:  none
// Outputs: data retreived
// Errors:  none
uint32_t OS_MailBox_Recv(void){ uint32_t data;
  //***YOU IMPLEMENT THIS FUNCTION*****
	
  OS_Wait(&Main_Mail);    // signal OS
	long counting_semaphore = StartCritical();
  data = Main_Mail_Input; // read Main_Mail
  EndCritical(counting_semaphore);
  return data;            // return Main_Mail data
}


