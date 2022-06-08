#include "tm4c123gh6pm.h"

void PortC_Init(void){
  volatile uint32_t delay;
	SYSCTL_RCGC2_R |= 0x00000004; 		// activate clock for port C
  SYSCTL_RCGCGPIO_R |= 0x04;
	delay=SYSCTL_RCGC2_R;           		// delay
  GPIO_PORTC_AMSEL_R = 0x00;      		// disable analog functionality on PC
  GPIO_PORTC_PCTL_R = 0x0000000; 		// configure PC as GPIO
  GPIO_PORTC_DIR_R = 0xFF;     			// make PC out
  GPIO_PORTC_AFSEL_R = 0x00;  			// disable alt funct on PC
  GPIO_PORTB_DEN_R = 0xFF;  	   		// enable digital I/O on PC
  GPIO_PORTB_DATA_R = 0x00;   			// PC = 0x00
}
