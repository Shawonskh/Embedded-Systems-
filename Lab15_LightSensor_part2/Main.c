
#include "ADC.h"
#include "UART.h"
#include "Init.h"
#include "tm4c123gh6pm.h"

#define FALSE 0
#define TRUE  1

#define BUS_FREQ 80      		  // [MHz] bus clock
#define PWM_FREQ 4000      		// [Hz] bus clock

#define SUPPLY_VOLTAGE 3300   // [mV]
//find maximum value of ADC
#define ADC_MAX   4095        // 12-bit number
//find maximum volatage in mV
#define MAX_VOLT    3300 		  // [mV]  maximum expected voltage: 100%
//find minimum voltage in mV
#define MIN_VOLT   0.805   		// [mV]  minimum expected voltage: 0%

void EnableInterrupts(void);      // Enable interrupts

//unsigned long ADCdata = 0;      // 12-bit 0 to 4095 sample
unsigned long Flag = FALSE;       // The ADCdata is ready

unsigned long Signal_High = 0;    // Signal_High of PWM
unsigned long Signal_Low = 0;			// Signal_Low of PWM
unsigned long counter = 0;				// counter for duty cycle
	
// Initialize SysTick with interrupts 
void SysTick_Init(unsigned long SysTick_Reload_Value)
{
	NVIC_ST_CTRL_R = 0;              // 1) disable SysTick during setup
	NVIC_ST_RELOAD_R = SysTick_Reload_Value;       // reload value
	NVIC_ST_CURRENT_R = 0;           // 3) any write to current clears it
	NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R&0x00FFFFFF; // priority 0
	NVIC_ST_CTRL_R = 0x00000007;     // 4) enable SysTick with core clock with interrupts
}

// Fires whenever reload value reaches 0.
//read PA2, ISR for PA2
void SysTick_Handler(void)
{	
  if(GPIO_PORTA_DATA_R&0x04){   // Confirming if PA2 is High
    GPIO_PORTA_DATA_R &= ~0x04; // Set PA2 low
    NVIC_ST_RELOAD_R = Signal_High-1;     // count down to next Signal_High
  } 	
	else{ // if PA2 is Low
    GPIO_PORTA_DATA_R |= 0x04;  // Set PA2 High
    NVIC_ST_RELOAD_R = Signal_Low-1;      // count down to next Signal_Low
  }	
}

//PWM Reload Value
//*******CalculateReloadVal*************
// Calculates the PWM reload value based on the PWM period.
// Input: none
// Output: reload value in terms of ticks
unsigned long FindInitialReloadValue(void){
	unsigned long InitialReloadValue = 0;
	InitialReloadValue = (BUS_FREQ * 1000000) / PWM_FREQ; // Implementation of PWM Reload_Value
return InitialReloadValue - 1;
}

//***************Delay*******************
// Fires a delay cycle.
// Input: number of delay cycles where 1 cycle == 800 Hz
// Output: none
// Delay
// Slowing down the sweep process by creating the delay function
void Delay (unsigned long delaySweep){	
	unsigned long volatile i = (delaySweep * 1000) - 1;
	while(i>0){i--;}
}

//*******ReloadValueConverted*************
// Converts duty cycle % to portion of PWM reload value
// Input: duty cycle in percentage and PWM reload value
// Output: reload value of on-time in terms of ticks
unsigned long ReloadValueConverted(unsigned int duty_sweep, unsigned long SysTick_Reload_Value){
	unsigned long New_ReloadValue = 0;
	// The frequency of a PWM signal is 1kHz, its duty cycle is 25%. The CPU frequency is 80MHz, we can do 19999/1000 == 0.1% duty cycle because 200 == 1% of 20k ticks, 4kHz is 20k ticks
	New_ReloadValue = duty_sweep * SysTick_Reload_Value / 1000; 
return New_ReloadValue;
}


unsigned long Non_Linearity (unsigned long sweepValue)
{
	unsigned long duty_sweep = 0;
	//duty_sweep = sweepValue * sweepValue * sweepValue / 1000000;
	duty_sweep = sweepValue * sweepValue / 1000;
	if(duty_sweep == 0) return sweepValue;
	return duty_sweep;
}
// a simple main program allowing you to debug the ADC interface
int main(void){ 
	unsigned long SysTick_Reload_Value = 0;
	unsigned long New_sweepValue = 0;
	PLL_Init();
	UART_Init();
	//write initialization for PA2 for PWM
	PA2_Init();
	SysTick_Reload_Value = FindInitialReloadValue();
	//write initialization for SysTick 
	SysTick_Init(SysTick_Reload_Value);								
	EnableInterrupts();	
	
	while (1){
		Delay(10);
		counter++;		
		New_sweepValue = Non_Linearity(counter);	
		Signal_High = ReloadValueConverted(New_sweepValue, SysTick_Reload_Value); 
		Signal_Low = SysTick_Reload_Value - Signal_High;
		Delay(10);
		
		UART_OutPWMDuty(New_sweepValue);
		if (counter == 1000) counter = 0;
	}
}
