#include <stdio.h>

#define _RCC_(mem_offset) (*(volatile uint32_t *)(0x40021000 + (mem_offset)))

#define _CR 0x00
#define _CFGR 0x04
#define _CIR 0x08
#define _APB2RSTR 0x0C
#define _APB1RSTR 0x10
#define _AHBENR 0x14
#define _APB2ENR 0x18
#define _APB1ENR 0x1C
#define _BDCR 0x20
#define _CSR 0x24

#define IOPAEN 0x0004
#define IOPBEN 0x0008
#define IOPCEN 0x0010
#define AFIOEN 0x0001
#define SPI1EN 0x1000
#define SPI2EN 0x4000
#define SPI3EN 0x8000

#define _PORTB_(mem_offset) (*(volatile uint32_t *)(0x40010C00 + (mem_offset)))

#define _BRR  0x14
#define _BSRR 0x10
#define _CRL  0x00
#define _CRH  0x04
#define _IDR  0x08
#define _ODR  0x0C

#define _SPI2_(mem_offset) (*(volatile uint32_t *)(0x40003800 + (mem_offset)))

#define _SPI_CR1 0x00
#define _SPI_CR2 0x04
#define _SPI_SR  0x08
#define _SPI_DR  0x0C

#define CPHA        0x0001
#define CPOL        0x0002
#define MSTR        0x0004
#define BR_0        0x0008
#define BR_1        0x0010
#define BR_2        0x0020
#define SPE         0x0040
#define LSB_FIRST   0x0080
#define SSI         0x0100
#define SSM         0x0200
#define RX_ONLY     0x0400
#define DFF         0x0800
#define CRC_NEXT    0x1000
#define CRC_EN      0x2000
#define BIDIOE      0x4000
#define BIDIMODE    0x8000

#define BSY         0x0080

#define TXE         0x0002

#define CNF0_0 0x00000004
#define CNF0_1 0x00000008
#define CNF1_0 0x00000040
#define CNF1_1 0x00000080
#define CNF2_0 0x00000400
#define CNF2_1 0x00000800
#define CNF3_0 0x00004000
#define CNF3_1 0x00008000
#define CNF4_0 0x00040000
#define CNF4_1 0x00080000
#define CNF5_0 0x00400000
#define CNF5_1 0x00800000
#define CNF6_0 0x04000000
#define CNF6_1 0x08000000
#define CNF7_0 0x40000000
#define CNF7_1 0x80000000
#define CNF8_0 0x00000004
#define CNF8_1 0x00000008
#define CNF9_0 0x00000040
#define CNF9_1 0x00000080
#define CNF10_0 0x00000400
#define CNF10_1 0x00000800
#define CNF11_0 0x00004000
#define CNF11_1 0x00008000
#define CNF12_0 0x00040000
#define CNF12_1 0x00080000
#define CNF13_0 0x00400000
#define CNF13_1 0x00800000
#define CNF14_0 0x04000000
#define CNF14_1 0x08000000
#define CNF15_0 0x40000000
#define CNF15_1 0x80000000

#define MODE0_0 0x00000001
#define MODE0_1 0x00000002
#define MODE1_0 0x00000010
#define MODE1_1 0x00000020
#define MODE2_0 0x00000100
#define MODE2_1 0x00000200
#define MODE3_0 0x00001000
#define MODE3_1 0x00002000
#define MODE4_0 0x00010000
#define MODE4_1 0x00020000
#define MODE5_0 0x00100000
#define MODE5_1 0x00200000
#define MODE6_0 0x01000000
#define MODE6_1 0x02000000
#define MODE7_0 0x10000000
#define MODE7_1 0x20000000
#define MODE8_0 0x00000001
#define MODE8_1 0x00000002
#define MODE9_0 0x00000010
#define MODE9_1 0x00000020
#define MODE10_0 0x00000100
#define MODE10_1 0x00000200
#define MODE11_0 0x00001000
#define MODE11_1 0x00002000
#define MODE12_0 0x00010000
#define MODE12_1 0x00020000
#define MODE13_0 0x00100000
#define MODE13_1 0x00200000
#define MODE14_0 0x01000000
#define MODE14_1 0x02000000
#define MODE15_0 0x10000000
#define MODE15_1 0x20000000

/*** LAT pulse - high, then low */
#define LAT_pulse() _PORTB_(_BSRR) = (1<<14); _PORTB_(_BRR) = (1<<14)

#define LAT_low() _PORTB_(_BRR) = (1<<14)

uint16_t leds[16];

void dm_shift16(uint16_t value)
{
    _SPI2_(_SPI_DR) = value; //send 2 bytes
    while (!(_SPI2_(_SPI_SR) & TXE)); //wait until it's sent
}

void sendLEDdata()
{
	LAT_low();
	uint8_t k = 16;
	do
	{   k--;
	    dm_shift16(leds[k]);
	} while (k);

	while (_SPI2_(_SPI_SR) & BSY); // finish transmission

	LAT_pulse();
}

void setupDM()
{
	//используем стандартный SPI2: MOSI на B15, CLK на B13
	//LAT пусть будет на неиспользуемом MISO - B14
	//включаем тактирование порта B и альт. функций
	_RCC_(_APB2ENR) |= IOPBEN | AFIOEN;

	//очищаем дефолтный бит, он нам точно не нужен
	_PORTB_ (_CRH) &= ~(CNF15_0 | CNF14_0 | CNF13_0 | CNF12_0);

	//альтернативные функции для MOSI и SCK
	_PORTB_ (_CRH) |= CNF15_1 | CNF13_1;

	//50 МГц, оба MODE = 1
	_PORTB_ (_CRH) |= MODE15_1 | MODE15_0 | MODE14_1 | MODE14_0 | MODE13_1 | MODE13_0;

	//включаем  тактирование SPI2
	_RCC_(_APB1ENR) |= SPI2EN;

	//настраиваем SPI2
	//нам надо получить: Master, Bidirectional, Transmit only, MSB first, 16-bit mode,
	//rising edge, low SCK when idle, SS SC, SS high, pclk/4
	_SPI2_ (_SPI_CR1) |= BR_0;// pclk/4
 	_SPI2_ (_SPI_CR1) &= ~CPOL; //0 = low SCK when idle
 	_SPI2_ (_SPI_CR1) &= ~CPHA; //0 = rising edge
  	_SPI2_ (_SPI_CR1) |= DFF;// &= ~DFF; // 16-bit mode
	_SPI2_ (_SPI_CR1) &= ~LSB_FIRST; //0 = MSB first
	_SPI2_ (_SPI_CR1) |= SSM | SSI; //enable software control of SS, SS high
	_SPI2_ (_SPI_CR1) |= MSTR; //SPI master
//	_SPI2_ (_SPI_CR1) |= BIDIMODE; //Bidirectional mode
//	_SPI2_ (_SPI_CR1) |= BIDIOE; //output enable, transmit only

	//когда все готово, включаем SPI
	_SPI2_ (_SPI_CR1) |= SPE;
}

void some_delay()
{
	for (volatile uint32_t bz = 0; bz < 1000000; bz++);
}

int main(void)
{
	setupDM();
	while(1)
	{
		for (uint8_t lednum = 0; lednum < 16; lednum++)
		{
			leds[lednum] = 0x0C01;
			//sendLEDdata();
			sendLEDdata();
			some_delay();
			leds[lednum] = 0;
		}
	}

}
