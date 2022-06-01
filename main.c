// main.c
// Runs on LM4F120/TM4C123
// UART runs at 115,200 baud rate 
// Daniel Valvano
// May 23, 2014

/* This example accompanies the books
  "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
  ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014

"Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
 
 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
 
//==========================================================================
// Data: 01/06/2022
// Autor(a)/Equipe: Matheus Fortunato Dário
// Descrição:
// Projto de dipensador de alcool gel.
//==========================================================================
// Versão: 1.0
//==========================================================================
// Pinagem:
// PF1 - LED Vermelho
// PF2 - LED Azul
// PF3 - LED Verde
// PF0 - Chave SW2 - chave para ligar o sistema
// PF4 - Chave SW1 - chave para seleção de vazão(razão cíclica)
// PORTB - Caracteres para os displays de 7 segmentos
// PA5 - Acionamento de display 1
// PA6 - Acionamento de display 2
// PC5 - saída de PWM
// PE4 (AIN9) - Entrada analógica para simulação de sensor no Keil 
// PE3 (AIN0) - Entrada analógica para simulação de sensor na placa
//==========================================================================
#include <stdio.h>
#include <stdint.h> // C99 variable types
#include "ADCSWTrigger.h"
#include "portb.h"
#include "portf.h"
#include "UART.h"
#include "Timer0.h"
#include "Timer1.h"
#include "tm4c123gh6pm.h"
#include "SysTick.h"

void Output_Init(void);

#define PERIOD 160000		// 100 Hz @ 16 MHz PLL
#define PERIOD_75 160000*0.75		// razão de trabalho 75%

//uint32_t i = 0;

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

void Timer0A_Handler(void){
	TIMER0_ICR_R = TIMER_ICR_TATOCINT;			// acknowledge TIMER0A timeout
	// Trocar para o pino PC5
	//GPIO_PORTF_DATA_R |= 0x02;
	TIMER1_CTL_R = 0x00000001;    					//  enable TIMER1A
//  GPIO_PORTB_DATA_R = TABLE_SEN[i]/981;	  // 8-bit -> 250000/255 
}

void Timer1A_Handler(void){
	TIMER1_ICR_R = TIMER_ICR_TATOCINT;		// acknowledge TIMER0A timeout
	// Trocar para o pino PC5
	//GPIO_PORTF_DATA_R &= ~0x02;
	TIMER1_CTL_R = 0x00000000;    				// desable TIMER1A
	// Testar a chave para atualizar a razão de trabalho (vazão)
	TIMER1_TAILR_R = PERIOD_75;			
}

int main(void){ 
	int32_t data,In;
	int32_t lim_time= 1000;			// Tempo limite do dispensador
	uint8_t Estado = 1;					// Declara estado inicial
	char UART_data;
  Output_Init();              // initialize output device
  PortF_Init();
	PortB_Init();
	ADC0_InitSWTriggerSeq3_Ch9();		// Sequencer 3, canal 9 (PE4)
	Timer0_Init(PERIOD); 							 	// initialize timer0 (100 Hz)
	Timer1_One_Shot_Init(PERIOD_75); 		// initialize timer1 (75%)
	SysTick_Init();											// initialize SysTick Timer
	EnableInterrupts();
	
	while(1){
		switch(Estado){
			case 1:
				GPIO_PORTF_DATA_R = 0x0E;			// acende os LEDs
				SysTick_Wait10ms(10);					// espera 100 ms
				GPIO_PORTF_DATA_R = 0x00;				// apaga os LEDs
				printf("\nProjeto Final de Sistemas Microcontrolados Turma S25");
				printf("\nAluno(a) ou Equipe: Matheus Fortunato Dario");
				printf("\nSistema em espera");
				Estado = 2;
				break;
			case 2:
				GPIO_PORTF_DATA_R = 0x02;			// acende o LED vermelho
				SysTick_Wait10ms(10);					// espera 100 ms
				GPIO_PORTF_DATA_R = 0x00;			// apaga os LEDs
				SysTick_Wait10ms(10);					// espera 100 ms
				Estado = 3;
				break;
			case 3:
				In = GPIO_PORTF_DATA_R & 0X01;	// Leitura da chave
				if(In == 0x00){								// Teste da chave fechada
						Estado = 4;
				}
				else{
						Estado = 2;
				}
				break;
			case 4:
				printf("\nSistema em operacao");
				Estado = 5;
				break;
			case 5:
				GPIO_PORTF_DATA_R = 0x04;			// acende o LED Azul
				SysTick_Wait10ms(10);					// espera 100 ms
				GPIO_PORTF_DATA_R = 0x00;			// apaga os LEDs
				SysTick_Wait10ms(10);					// espera 100 ms
				Estado = 4;
				data = ADC0_InSeq3();
				UART_data = UART_InChar1();
				if (UART_data == 'a'){
					
				}
				else if(UART_data == 'd'){
					
				}
				else if(UART_data == 'r'){
					
				}
				else{
					printf("???");							// estado indefinido
				}
				break;
			case 6:
				break;
			case 7:
				break;
			case 8:
				break;
			case 9:
				break;
			case 10:
				break;
			case 11:
				break;
			case 12:
				break;
			default:
				break;
			
		}
	/*	
		data = ADC0_InSeq3();
    
		UART_data = UART_InChar1();
		if (UART_data == 'a')
			printf("\nADC data = %d ",data);
 */
	}
}
