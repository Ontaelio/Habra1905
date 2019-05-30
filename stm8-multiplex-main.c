/* MAIN.C file
 * 
 * Copyright (c) 2002-2005 STMicroelectronics
 */
//#include "stm8s.h"
#include <stdint.h>
#include <stdlib.h>
//#include "main.h"

//clk
#define CLK_CKDIVR *(volatile uint8_t *)0x0050C6

//io
#define PA_DDR     *(volatile uint8_t *)0x005002
#define PA_CR1     *(volatile uint8_t *)0x005003
#define PA_CR2     *(volatile uint8_t *)0x005004
#define PD_DDR     *(volatile uint8_t *)0x005011
#define PD_CR1     *(volatile uint8_t *)0x005012
#define PD_CR2     *(volatile uint8_t *)0x005013
#define PD_ODR     *(volatile uint8_t *)0x00500F
#define PC_DDR     *(volatile uint8_t *)0x00500C
#define PC_CR1     *(volatile uint8_t *)0x00500D
#define PC_CR2     *(volatile uint8_t *)0x00500E
#define PC_ODR     *(volatile uint8_t *)0x00500A
#define PB_DDR     *(volatile uint8_t *)0x005007
#define PB_CR1     *(volatile uint8_t *)0x005008
#define PB_CR2     *(volatile uint8_t *)0x005009
#define PB_ODR     *(volatile uint8_t *)0x005005

//tim2
#define TIM2_CR1   *(volatile uint8_t *)0x005300
#define TIM2_CCMR1 *(volatile uint8_t *)0x005307
#define TIM2_CCMR2 *(volatile uint8_t *)0x005308
#define TIM2_CCMR3 *(volatile uint8_t *)0x005309
#define TIM2_CCER1 *(volatile uint8_t *)0x00530A
#define TIM2_CCER2 *(volatile uint8_t *)0x00530B
#define TIM2_ARRH  *(volatile uint8_t *)0x00530F
#define TIM2_ARRL  *(volatile uint8_t *)0x005310
#define TIM2_CCR1H *(volatile uint8_t *)0x005311
#define TIM2_CCR1L *(volatile uint8_t *)0x005312
#define TIM2_CCR2H *(volatile uint8_t *)0x005313
#define TIM2_CCR2L *(volatile uint8_t *)0x005314
#define TIM2_CCR3H *(volatile uint8_t *)0x005315
#define TIM2_CCR3L *(volatile uint8_t *)0x005316
#define TIM2_PSCR  *(volatile uint8_t *)0x00530E
#define TIM2_IER   *(volatile uint8_t *)0x005303
#define TIM2_SR1   *(volatile uint8_t *)0x005304

//defines for TIMx_CCMRx
#define PWM_MODE2  0x70 //PWM mode 2, 0b01110000
#define PWM_MODE1  0x60 //PWM mode 1, 0b01110000
#define OCxPE			 0x08 //preload enable

//defines for TIMx_CCER1 & 2
#define CC1P  (1<<1) // CCER1
#define CC1E  (1<<0) // CCER1
#define CC2P  (1<<5) // CCER1
#define CC2E  (1<<4) // CCER1
#define CC3P  (1<<1) // CCER2
#define CC3E  (1<<0) // CCER2

uint16_t tcount = 0;
uint8_t red, green, blue;

uint8_t colors[8][3] = {
{254, 0, 0},
{200, 54, 0},
{100, 154, 0},
{0, 254, 0},
{0, 127, 127},
{0, 0, 254},
{100, 0, 154},
{200, 0, 54}};

uint8_t cnt;
uint8_t ccc, ccc1, aaa;

void simpleDelay(uint16_t baba);
void setRGBled(uint8_t r, uint8_t g, uint8_t b);

@far @interrupt void TIM2_Overflow (void)
{
	PD_ODR &= ~(1<<5); // вырубаем демультиплексор
	PC_ODR = (cnt<<3); // записываем в демультиплексор новое значение
	PD_ODR |= (1<<5); // включаем демультиплексор
	
	TIM2_SR1 = 0; // сбрасываем флаг Update Interrupt Pending
	
	//TIM2_CR1 |= 1; // включаем таймер
		
	cnt++; 
	cnt &= 7; // двигаем счетчик LED
	
	TIM2_CCR1L = ~colors[cnt][0]; // передаем в буфер значения
	TIM2_CCR2L = ~colors[cnt][1]; // для следующего цикла ШИМ
	TIM2_CCR3L = ~colors[cnt][2];
	
  
    return;
}


main()

{
	
	CLK_CKDIVR &= ~(0x18);
	
	PA_DDR = (1<<3); //output
	PA_CR1 |= (1<<3); //push-pull
	PA_CR2 |= (1<<3); //fast
	PD_DDR = ((1<<3) | (1<<4) | (1<<5)); //output
	PD_CR1 |= ((1<<3) | (1<<4)| (1<<5)); //push-pull
	PD_CR2 |= ((1<<3) | (1<<4)| (1<<5)); //fast
	PC_DDR = ((1<<3) | (1<<4) | (1<<5)); //output
	PC_CR1 |= ((1<<3) | (1<<4) | (1<<5)); //push-pull
	PC_CR2 |= ((1<<3) | (1<<4) | (1<<5)); //fast
	
	PB_DDR |= (1<<5);//output
	PB_ODR |= (1<<5);
	
	PD_ODR |= (1<<5);
	PC_ODR = (1<<4);
	//PB_CR1 |= (5<<1); //open drain
	//PB_CR2 |= (5<<1); //fast
	
	//ARR preload
	//TIM2_CR1 = (1<<7);
	
	//set ARR
	TIM2_ARRH = 0;
	TIM2_ARRL = 254;
	
	//set prescaler
	TIM2_PSCR = 5;
	
	//set PWM mode2, preload enable
	TIM2_CCMR1 = (PWM_MODE2 | OCxPE);
	TIM2_CCMR2 = (PWM_MODE2 | OCxPE);
	TIM2_CCMR3 = (PWM_MODE2 | OCxPE);
	
	//set polarity 
	TIM2_CCER1 |= (CC1P | CC2P);
	TIM2_CCER2 |= CC3P;
  //TIM1_CCER1 |= (CC1P | CC2P);
	//TIM1_CCER2 |= CC3P;

	 //enable outputs
	TIM2_CCER1 |= (CC1E | CC2E);
	TIM2_CCER2 |= CC3E;
	
	//enable interrupt
	TIM2_IER = 1;
	
	//enable counter stop on UEV
	//TIM2_CR1 |= (1<<3);
	
  //start timer
	TIM2_CR1 |= 1;
	
	//enable interrupts
	_asm("rim");
	
	
		while (1)
	{
		//int count = 0;
		/*tcount = 0;
		
		do
		{
			
				if (tcount < 256) {red = 255 - tcount; green = tcount; blue = 0;}
				else if (tcount < 512) {red = 0; green = 511 - tcount; blue = tcount-256;}
				else {red = tcount - 512; green = 0; blue = 765 - tcount;}
				//setRGBled(red, green, blue);
			
		tcount++; */
		do {ccc = (uint8_t)abs(rand() & 7);
	      ccc1 = (uint8_t)abs(rand() & 7);}
		while (ccc == ccc1);
		//ccc = (tcount & 7);
		//ccc1 = ((tcount + 1) & 7);
		_asm("sim");
		aaa = colors[ccc][0];
		colors[ccc][0] = colors[ccc1][0];
		colors[ccc1][0] = aaa;
		aaa = colors[ccc][1];
		colors[ccc][1] = colors[ccc1][1];
		colors[ccc1][1] = aaa;
		aaa = colors[ccc][2];
		colors[ccc][2] = colors[ccc1][2];
		colors[ccc1][2] = aaa;
		_asm("rim");
		simpleDelay(1500);
		//} while (tcount<765);
	
	}
}	

void simpleDelay(uint16_t baba)
{
	uint16_t i, j;
	
	for(i = 0; i < baba; i ++)
	{
		for(j=0; j < baba; j ++)
		{
		}
	}
}

void setRGBled(uint8_t r, uint8_t g, uint8_t b)
{		
	TIM2_CCR1L = r;
	TIM2_CCR2L = g;
	TIM2_CCR3L = b;
	
	
}