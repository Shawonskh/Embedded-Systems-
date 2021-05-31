#include "ADC.h"
#include "UART.h"
#include "Init.h"
#include "tm4c123gh6pm.h"

#define FALSE 0
#define TRUE  1

#define BUS_FREQ 80      		  // [MHz] bus clock

#define SUPPLY_VOLTAGE 3300   // [mV]
//find maximum value of ADC
#define ADC_MAX  4095         // 12-bit number
//find maximum volatage in mV
#define MAX_VOLT   3300  		  // [mV]  maximum expected voltage: 100%
//find minimum voltage in mV
#define MIN_VOLT   0.805  		// [mV]  minimum expected voltage: 0%


void EnableInterrupts(void);      // Enable interrupts

unsigned long ADCdata = 0;        // 12-bit 0 to 4095 sample
unsigned long Flag = FALSE;       // The ADCdata is ready
double resolution = 0.805;        // floating point number, resolution


//********Convert2Voltage****************
// Convert a 12-bit binary ADC sample into a 32-bit unsigned
// fixed-point voltage (resolution 1 mV).  
// Maximum and minimum values are calculated 
// and are dependant on resitor value.
// Overflow and dropout should be considered 
// Input: sample  12-bit ADC sample
// Output: 32-bit voltage (resolution 1 mV)
unsigned long Convert2Voltage(unsigned long sample)
{
	long voltage=0;
//your code should be here
//converts Digital Data to Voltage. 
	sample = (unsigned long)(sample*resolution);
	voltage = sample + MIN_VOLT;
	return voltage;
}

//********Convert2Percent****************
// Convert voltage reading into fixed-point precision
// luminosity percentage (resolution 0.1%). 
// Assume linear dependency.
// Input: voltage (mV)
// Output: fixed-point percent ((resolution 0.1%)
//unsigned long Convert2Percent(unsigned long voltage)
//{
	//long percent=0;
	//your code should be here
	//Conditions to check to reduce the number of computation times
	//if(voltage <= MIN_VOLT) return percent = 0;
	//if(voltage >= MAX_VOLT) return percent = 100;
	
	//percent = voltage * 1000 / MAX_VOLT; // Here we multiply by 1000, to get 1 Decimal place
	//return percent;
//}

// Initialize SysTick with interrupts 
void SysTick_Init()
{
	NVIC_ST_CTRL_R = 0;              // disable SysTick during setup
	NVIC_ST_RELOAD_R = 1999999;   // reload value for max
	NVIC_ST_CURRENT_R = 0;           // any write to current clears it
	NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R&0x00FFFFFF; // priority 0
	NVIC_ST_CTRL_R = 0x00000007;     // enable SysTick with core clock with interrupts
}

// Fires whenever reload value reaches 0.
// read from ADC with 40 Hz 
void SysTick_Handler(void)
{	
	ADCdata = ADC0_In(); // Call ADC function
	Flag = TRUE;         // The ADCdata is not ready
	
}

// Delay
// get the systick interrupt every 0.025 ms
//void Delay (void){	
//	unsigned long volatile i = 1999999;
//	while(i>0){i--;}
//}

// a simple main program allowing you to debug the ADC interface
int main(void)
	{ 
	unsigned long voltage = 0;
	unsigned long PercentageofVoltage = 0;
	
	PLL_Init();
	ADC0_Init();// initialize ADC0 for PE2, channel 1, sequencer 3
	UART_Init();
	//write initialization for SysTick 
	SysTick_Init();								
	EnableInterrupts();
  
	// Infinity loop
	while(TRUE) {     
		while(!Flag){   // while adcdata is ready
			//voltage = Convert2Voltage(ADCdata);		          // Take raw ADCdata and convert it to voltage
			//PercentageofVoltage = Convert2Percent(voltage); // Make calculations percentage of voltage
// Now here the output is being converted to string to show on USB interface via UART 
		//	UART_OutRaw(ADCdata);
			UART_OutVoltage(Convert2Voltage(ADCdata));
			//UART_OutPercent(PercentageofVoltage);
			// Delay for 40Hz
			//Delay();
			// flipping flag
			//Flag ^= Flag; 
		}
		
	}
	
}

