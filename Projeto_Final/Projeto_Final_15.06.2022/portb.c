//---------------------------------------------------------------
// Descrição dos pinos do PortB usados na placa MicroTB_UTFPR
// Amauri Amorin Assef
// 09/06/22
//---------------------------------------------------------------
//PortB: 0bxxxx_xxxx
//         |||| ||||-> LED1/segmento A dos displays
//         |||| |||--> LED2/segmento B dos displays      
//         |||| ||---> LED3/segmento C dos displays
//         |||| -----> LED4/segmento D dos displays      
//         |||-------> LED5/segmento E dos displays/D4 do LCD      
//         ||--------> LED6/segmento F dos displays/D5 do LCD            
//         |---------> LED7/segmento G dos displays/D6 do LCD            
//         |---------> LED8/segmento pt (ponto) dos displays/D7 do LCD            
//---------------------------------------------------------------

#include "tm4c123gh6pm.h"

void PortB_Init(void){
  volatile uint32_t delay;
	SYSCTL_RCGC2_R |= 0x00000002; 		// activate clock for port B
  delay=SYSCTL_RCGC2_R;           	// delay
  GPIO_PORTB_AMSEL_R = 0x00;      	// disable analog functionality on PB
  GPIO_PORTB_PCTL_R = 0x0000000; 		// configure PB as GPIO
  GPIO_PORTB_DIR_R = 0xFF;     			// make PB out
  GPIO_PORTB_AFSEL_R = 0x00;  			// disable alt funct on PB
  GPIO_PORTB_DEN_R = 0xFF;  	   		// enable digital I/O on PB
  GPIO_PORTB_DATA_R = 0x00;   			// PB = 0x00
}