//---------------------------------------------------------------
// Descrição dos pinos do PortA usados na placa MicroTB_UTFPR
// Amauri Amorin Assef
// 09/06/22
//---------------------------------------------------------------
//PortA: 0bxxxx_xxxx
//          ||| ||||-> U0Rx      
//          ||| |||--> U0Tx
//          ||| ||---> RS do LCD
//          ||| |----> E do LCD
//          |||------> Acionamento dos LEDs
//          ||-------> Acionamento do display de 7 segmentos 1
//          |--------> Acionamento do display de 7 segmentos 2
//---------------------------------------------------------------

#include "tm4c123gh6pm.h"

void PortA_Init(void){
  volatile uint32_t delay;
	SYSCTL_RCGC2_R |= 0x00000001; 		// activate clock for port A
	SYSCTL_RCGCGPIO_R |= 0x01;        // activate port A
  delay=SYSCTL_RCGC2_R;           	// delay
  GPIO_PORTA_AMSEL_R &= ~0x7C;     	// disable analog functionality on PA
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xF00000FF);// configure PA2-PA6 as GPIO
  GPIO_PORTA_AFSEL_R &= ~0x7C; 			// disable alt funct on PA2-PA6
  GPIO_PORTA_DIR_R |= 0x7C;    			// make P2-PA6 out
  GPIO_PORTA_DEN_R |= 0x7C;  	   		// enable digital I/O on PA2-PA6
	GPIO_PORTA_DATA_R &= ~0x7C;  	   	// PA2:A6 = 0
}