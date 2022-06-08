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
// Autor(a)/Equipe: Matheus Fortunato D�rio
// Descri��o:
// Projto de dipensador de alcool gel.
//==========================================================================
// Vers�o: 1.1
//==========================================================================
// Pinagem:
// PF1 - LED Vermelho
// PF2 - LED Azul
// PF3 - LED Verde
// PF0 - Chave SW2 - chave para ligar o sistema
// PF4 - Chave SW1 - chave para sele��o de vaz�o(raz�o c�clica)
// PORTB - Caracteres para os displays de 7 segmentos
// PA5 - Acionamento de display 1
// PA6 - Acionamento de display 2
// PC5 - sa�da de PWM
// PE4 (AIN9) - Entrada anal�gica para simula��o de sensor no Keil 
// PE3 (AIN0) - Entrada anal�gica para simula��o de sensor na placa
//==========================================================================
// Sugest�es:
// - Para tempo limite usar o Timer 2
// - PWM na placa PC5 (sa�da): portc.h portc.c
// 'i' ->	if(tempo_limite<tempo_m�ximo)
//					tempo limite=tempo_limite+tempo_200m
// 't' -> else if(tempo_limite>)
//==========================================================================
#include <stdio.h>
#include <stdint.h> // C99 variable types
#include "ADCSWTrigger.h"
#include "porta.h"
#include "portb.h"
#include "portc.h"
#include "portf.h"
#include "UART.h"
#include "Timer0.h"
#include "Timer1.h"
#include "tm4c123gh6pm.h"
#include "SysTick.h"

void Output_Init(void);

#define PERIOD 160000							// 100 Hz @ 16 MHz PLL
#define PERIOD_75 160000*0.75			// raz�o de trabalho 75%
#define tempo_maximo 16000000*1.4 // 1,4s
#define tempo_minimo 16000000*0.6 // 0,6s
#define tempo_padrao 16000000*1 	// 1,0s
#define tempo_100ms 16000000*0.1 	// 0,1s
#define valor_15cm 2047						//

const long tabela_display[10] = {
	0x3F,	// 0
	0x06,	// 1
	0x5B,	// 2
	0x4F,	// 3
	0x66,	// 4
	0x6D,	// 5
	0x7D,	// 6
	0x07,	// 7
	0x7F,	// 8
	0x67	// 9
}; 

uint32_t i = 0,tempo_limite = tempo_padrao;
uint8_t liga_pwm=0,display = 0,unidade=1,dezena=0;

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode

void Timer0A_Handler(void){
	TIMER0_ICR_R = TIMER_ICR_TATOCINT;			// acknowledge TIMER0A timeout
	// Testar qual display deve ser acionado
	if(display == 0){
		display = 1;
		GPIO_PORTA_DATA_R |= 0x20;	// habilitar o pino PA5  (0b00100000) 
		GPIO_PORTA_DATA_R &= 0x40;	// desabilitar o pino PA6(0b01000000)
		// enviar o valor da dezena do estado para o display
		GPIO_PORTB_DATA_R = tabela_display[dezena];
	}
	else{
		display = 0;
		GPIO_PORTA_DATA_R &= 0x20;	// desabilitar o pino PA5  (0b00100000) 
		GPIO_PORTA_DATA_R |= 0x40;	// habilitar o pino PA6(0b01000000)
		// enviar o valor da unidade do estado para o display
		GPIO_PORTB_DATA_R = tabela_display[unidade];
	}
	// Trocar para o pino PC5
	if(liga_pwm==1){
		GPIO_PORTC_DATA_R |= 0x20;		//0b_0010_0000
		//GPIO_PORTF_DATA_R |= 0x20;	//0b_0010_0000
	}
	else{
		GPIO_PORTC_DATA_R &= 0x20;		//0b_0010_0000
	}
	//GPIO_PORTC_DATA_R |= 0x05;
	TIMER1_CTL_R = 0x00000001;    					//  enable TIMER1A
//  GPIO_PORTB_DATA_R = TABLE_SEN[i]/981;	  // 8-bit -> 250000/255 
}

void Timer1A_Handler(void){
	TIMER1_ICR_R = TIMER_ICR_TATOCINT;		// acknowledge TIMER0A timeout
	// Trocar para o pino PC5
	//GPIO_PORTC_DATA_R &= ~0x05;
	TIMER1_CTL_R = 0x00000000;    				// desable TIMER1A
	// Testar a chave para atualizar a raz�o de trabalho (vaz�o)
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
	ADC0_InitSWTriggerSeq3_Ch9();		// Sequencer 3, canal 9 (PE4). - Simula��o 	
	ADC0_InitSWTriggerSeq3(0);			// Sequencer 3, canal 0 (PE3). - Placa
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
				liga_pwm = 0;
				unidade = 2;
				dezena = 0;
				Estado = 2;
				break;
			case 2:
				GPIO_PORTF_DATA_R = 0x02;			// acende o LED vermelho
				SysTick_Wait10ms(10);					// espera 100 ms
				GPIO_PORTF_DATA_R = 0x00;			// apaga os LEDs
				SysTick_Wait10ms(10);					// espera 100 ms
				unidade = 3;
				dezena = 0;
				Estado = 3;
				break;
			case 3:
				In = GPIO_PORTF_DATA_R & 0X01;	// Leitura da chave
				if(In == 0x00){								// Teste da chave fechada
						unidade = 4;
						dezena = 0;
						Estado = 4;
				}
				else{
						unidade = 2;
						dezena = 0;
						Estado = 2;
				}
				break;
			case 4:
				printf("\nSistema em operacao");
				unidade = 5;
				dezena = 0;
				Estado = 5;
				break;
			case 5:
				GPIO_PORTF_DATA_R = 0x04;			// acende o LED Azul
				SysTick_Wait10ms(10);					// espera 100 ms
				GPIO_PORTF_DATA_R = 0x00;			// apaga os LEDs
				SysTick_Wait10ms(10);					// espera 100 ms
				
				data = ADC0_InSeq3();
				UART_data = UART_InChar1();
				if (UART_data == 'a'){			// 'a': aumentar o tempo limite em 100 ms
					if(tempo_limite<tempo_maximo){	
						tempo_limite = tempo_limite + tempo_100ms;
					}
				}
				else if(UART_data == 'd'){	// 'd': diminuir o tempo limite em 100 ms
					if(tempo_limite>tempo_minimo){
						tempo_limite = tempo_limite + tempo_100ms;
					}
				}
				else if(UART_data == 'r'){	// 'r': resetar o tempo limite;
					tempo_limite = tempo_padrao;
				}
				else{
					printf("???");							// estado indefinido
				}
				unidade = 6;
				dezena = 0;
				Estado = 6;
				break;
			case 6:
				if(1){
					unidade = 7;
					dezena = 0;
					Estado = 7;
				}
				else{
					unidade = 5;
					dezena = 0;
					Estado = 5;
				}
				break;
			case 7:
				unidade = 8;
				dezena = 0;
				Estado = 8;
				break;
			case 8:
				unidade = 9;
				dezena = 0;
				Estado = 9;
				break;
			case 9:
				if(1){
					unidade = 2;
					dezena = 1;
					Estado = 12;
					if(1){
						unidade = 0;
						dezena = 1;
						Estado = 10;
					}
					else{
						unidade = 9;
						dezena = 0;
						Estado = 9;
					}
				}
				break;
			case 10:
				unidade = 1;
				dezena = 1;
				Estado = 11;
				break;
			case 11:
				if(1){
					unidade = 2;
					dezena = 1;
					Estado = 12;
				}
				else{
					unidade = 1;
					dezena = 1;
					Estado = 11;
				}
				break;
			case 12:
				unidade = 4;
				dezena = 0;
				Estado = 4;
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
