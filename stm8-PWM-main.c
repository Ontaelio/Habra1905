#include <stdint.h>

//clk
#define CLK_CKDIVR *(volatile uint8_t *)0x0050C6

//io
#define PA_DDR     *(volatile uint8_t *)0x005002
#define PA_CR1     *(volatile uint8_t *)0x005003
#define PA_CR2     *(volatile uint8_t *)0x005004
#define PD_DDR     *(volatile uint8_t *)0x005011
#define PD_CR1     *(volatile uint8_t *)0x005012
#define PD_CR2     *(volatile uint8_t *)0x005013
#define PC_DDR     *(volatile uint8_t *)0x00500C
#define PC_CR1     *(volatile uint8_t *)0x00500D
#define PC_CR2     *(volatile uint8_t *)0x00500E
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
#define TIM2_CCR1L *(volatile uint8_t *)0x005312
#define TIM2_CCR2L *(volatile uint8_t *)0x005314
#define TIM2_CCR3L *(volatile uint8_t *)0x005316
#define TIM2_PSCR  *(volatile uint8_t *)0x00530E
#define TIM2_IER   *(volatile uint8_t *)0x005303
#define TIM2_SR1   *(volatile uint8_t *)0x005304

//tim1
#define TIM1_CR1   *(volatile uint8_t *)0x005250
#define TIM1_CCMR1 *(volatile uint8_t *)0x005258
#define TIM1_CCMR2 *(volatile uint8_t *)0x005259
#define TIM1_CCMR3 *(volatile uint8_t *)0x00525A
#define TIM1_CCER1 *(volatile uint8_t *)0x00525C
#define TIM1_CCER2 *(volatile uint8_t *)0x00525D
#define TIM1_ARRH  *(volatile uint8_t *)0x005262
#define TIM1_ARRL  *(volatile uint8_t *)0x005263
#define TIM1_CCR1L *(volatile uint8_t *)0x005266
#define TIM1_CCR2L *(volatile uint8_t *)0x005268
#define TIM1_CCR3L *(volatile uint8_t *)0x00526A
#define TIM1_PSCRH *(volatile uint8_t *)0x005260
#define TIM1_PSCRL *(volatile uint8_t *)0x005261
#define TIM1_BKR   *(volatile uint8_t *)0x00526D
#define TIM1_IER   *(volatile uint8_t *)0x005254
#define TIM1_SR1   *(volatile uint8_t *)0x005255

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


void simpleDelay(uint16_t baba);
void setRGBled(uint8_t r, uint8_t g, uint8_t b);



uint16_t tcount = 0;
uint8_t red, green, blue;
		
main()

{
	
	CLK_CKDIVR &= ~(0x18);
	
	
	PA_CR1 |= (1<<3); //push-pull
	PA_CR2 |= (1<<3); //fast
	
	PD_CR1 |= ((1<<3) | (1<<4)); //push-pull
	PD_CR2 |= ((1<<3) | (1<<4)); //fast
	
	PC_CR1 |= ((1<<3) | (1<<6) | (1<<7)); //push-pull
	PC_CR2 |= ((1<<3) | (1<<6) | (1<<7)); //fast
	
	PB_DDR |= (1<<5);//output
	PB_ODR |= (1<<5);
	
	//ARR preload
	TIM2_CR1 = (1<<7);
	TIM1_CR1 = (1<<7);
	
	//set ARR
	TIM2_ARRH = 0;
	TIM2_ARRL = 255;
	TIM1_ARRH = 0;
	TIM1_ARRL = 255;
	
	//set prescaler
	TIM2_PSCR = 5;
	
	//set PWM mode2, preload enable
	TIM2_CCMR1 = (PWM_MODE2 | OCxPE);
	TIM2_CCMR2 = (PWM_MODE2 | OCxPE);
	TIM2_CCMR3 = (PWM_MODE2 | OCxPE);
	TIM1_CCMR1 = (PWM_MODE2 | OCxPE);
	TIM1_CCMR2 = (PWM_MODE2 | OCxPE);
	TIM1_CCMR3 = (PWM_MODE2 | OCxPE);
	
	//set polarity 
	//TIM2_CCER1 |= (CC1P | CC2P);
	//TIM2_CCER2 |= CC3P;
  //TIM1_CCER1 |= (CC1P | CC2P);
	//TIM1_CCER2 |= CC3P;

	 //enable outputs
	TIM2_CCER1 = (CC1E | CC2E);
	TIM2_CCER2 = CC3E;
	TIM1_CCER1 = (CC1E | CC2E);
	TIM1_CCER2 = CC3E;
	
	//timer 1 MOE enable
  TIM1_BKR = (1<<7);// | (1<<6);
	
	//enable interrupt
	TIM2_IER = 1;
	TIM1_IER = 0;
	
  //start timer
	TIM2_CR1 |= 1;
	TIM1_CR1 |= 1;
	
	//_asm("rim");
	
	
		while (1)
	{
		//int count = 0;
		tcount = 0;
		do
		{
			
				if (tcount < 256) {red = 255 - tcount; green = tcount; blue = 0;}
				else if (tcount < 512) {red = 0; green = 511 - tcount; blue = tcount-256;}
				else {red = tcount - 512; green = 0; blue = 765 - tcount;}
				setRGBled(red, green, blue);
			
		tcount++;
		simpleDelay(150);
		} while (tcount<765);
	
	}
}	

void simpleDelay(uint16_t baba)
{
	uint16_t i, j;
	
	for(i = 0; i < baba; i ++)
	{
		for(j=0; j < 40; j ++)
		{
		}
	}
}

void setRGBled(uint8_t r, uint8_t g, uint8_t b)
{
	TIM2_CCR1L = r;
	TIM2_CCR2L = g;
	TIM2_CCR3L = b;
	TIM1_CCR1L = g;
	TIM1_CCR2L = r;
	TIM1_CCR3L = b;
}