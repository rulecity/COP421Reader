// COP421 Reader
// By Matt Ownby
// Jan 2024

// Very quick and dirty.  This was to read a single microcontroller so I just put enough effort into it to get it working.
// Thanks to http://john.ccac.rwth-aachen.de:8000/patrick/COPSreader.htm for doing most of the work and the excellent documentation.
// Final design's CKI runs at about 250 kHz and it still works.  I could've written this in assembly language and increased CKI speed but it wasn't worth it.

// This program dumps 1k bytes + 2 CRC bytes at 115200 bps over AVR's USART serial lines.

#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include "serial.h"
#include "crc.h"

/////////////////////////////////////////////////////

#define WAIT_FALL()		while ((PINC & (1 << PC0)) != 0) { fastClockThink(); }
#define WAIT_RISE()		while ((PINC & (1 << PC0)) == 0) { fastClockThink(); }

//////////////////////////////////////

// to manage output clock that we generate
uint8_t g_u8COP421CkiIsHigh = 0;

// to hold buffer that we read from COP
uint16_t g_u16BufferCount = 0;
uint8_t *g_pBufferPtr = 0;
uint8_t g_arrCOPBuffer[1024];

inline void fastClockThink()
{
	// make fast clock tick
	if (TIFR0 & (1 << OCF0A))
	{
		g_u8COP421CkiIsHigh ^= 1;

		PORTB &= ~(1 << PB0);
		PORTB |= (g_u8COP421CkiIsHigh << PB0);

		TIFR0 |= (1 << OCF0A);	// clear CTC flag
	}
}

inline void insertByte(uint8_t u8Byte)
{
	PORTA = u8Byte;

	// wait for SK's next falling edge
	WAIT_FALL();

	// wait for SK's next rising edge
	WAIT_RISE();
}

int main (void) 
{
	uint8_t u8CyclesElapsed64k = 0;

	uint8_t u8Tmp = 0;

	serial_init();

	// Setup 16-bit timer
	TCCR1A = (1 << WGM11);	// enable CTC
	TCCR1B = (1 << CS10) | (1 << CS11);	// /64 prescaling
	OCR1B = 0xFFF0;	// ~64k worth of cycles

	// Setup 8-bit timer
	TCCR0A = (1 << WGM01);	// enable CTC
	TCCR0B = (1 << CS00);	// no-prescaling
	OCR0A = 36;	// CTC every 36 cycles (this makes outgoing clock run at about 250 kHz)

	// go into output mode for CKI, SI, and RESET
	DDRB = (1 << PB0) | (1 << PB1) | (1 << PB2);

	// set CKI, SI and RESET low
	PORTB = (0 << PB0) | (0 << PB1) | (0 << PB2);

	// go into input mode for SK
	DDRC = (0 << PC0);

	// start in input mode for L0-L7 to be safe
	DDRA = 0;

	// start tracking 16-bit timer so we know how many cycles have elapsed
	TCNT1 = 0;

	// enable interrupts now that we are initialized
	sei();

	///////////////////

	// POWER ON STAGE

	// played around with this value until I got a ~130ms setup time
	while (u8CyclesElapsed64k < 37)
	{
		fastClockThink();

		// if 64k cycles have elapsed
		if (TIFR1 & (1 << OCF1A))
		{
			TIFR1 |= (1 << OCF1A);	// clear CTC flag
			u8CyclesElapsed64k++;
		}
	}

	// wait for SK's next rising edge
	WAIT_RISE();

	// raise SI and RESET'
	PORTB |= (1 << PB1)|(1 << PB2);

	// wait for SK's next falling edge
	WAIT_FALL();

	// wait for SK's next rising edge
	WAIT_RISE();

	// at this point, the COP is ready for us to insert instructions

	DDRA = 0xFF;	// go into output mode

	// This sends the initial program needed to put the COP in "ROM dump" mode
	insertByte(0);	// CLRA
	insertByte(0x63);
	insertByte(0xFC);
	insertByte(0xBF);
	insertByte(0x44);
	insertByte(0x33);
	insertByte(0x64);

	// SPECIAL BEHAVIOR LAST INSERTED BYTE

	PORTA = 0x44;	// NOP

	// wait for SK's next falling edge
	WAIT_FALL();

	// NEED TO LOWER SI IN MIDDLE OF SK

	u8Tmp = 0;
	
	// SK period is 16 of our fast clocks, so we go at 11 at recommendation of blog post
	while (u8Tmp < 11)
	{
		// make fast clock tick
		if (TIFR0 & (1 << OCF0A))
		{
			g_u8COP421CkiIsHigh ^= 1;

			PORTB &= ~(1 << PB0);
			PORTB |= (g_u8COP421CkiIsHigh << PB0);

			TIFR0 |= (1 << OCF0A);	// clear CTC flag

			if (g_u8COP421CkiIsHigh)
			{
				u8Tmp++;
			}
		}
	}

	// PREPARE TO READ BYTES

	PORTB &= ~(1 << PB1);	// LOWER SI
	DDRA = 0;	// INPUT MODE FOR DATA BUS

	WAIT_RISE();

	// At this point, the first byte of the program will be put on the data bus as soon as SK falls

	g_pBufferPtr = g_arrCOPBuffer;
	g_u16BufferCount = 0;

	// read in the 1k program
	while (g_u16BufferCount < sizeof(g_arrCOPBuffer))
	{
		WAIT_FALL();
		
		// byte of program will be put on data bus shortly after this point; we'll wait until rising edge to guarantee that it's there

		WAIT_RISE();

		*g_pBufferPtr = PINA;

		g_pBufferPtr++;
		g_u16BufferCount++;
	}

	// go back into init/reset state for COP so it's not doing anything with the bus
	PORTB = (0 << PB0) | (0 << PB1) | (0 << PB2);

	// DUMP BUFFER OUT TO SERIAL PORT
	
	g_pBufferPtr = g_arrCOPBuffer;
	g_u16BufferCount = 0;
	crc_ccitt_init();

	while (g_u16BufferCount < sizeof(g_arrCOPBuffer))
	{
		uint8_t u8 = *g_pBufferPtr;
		tx_to_buf(u8);
		crc_ccitt_update(u8);
		g_pBufferPtr++;
		g_u16BufferCount++;
	}

	// send CRC value (lil endian)
	tx_to_buf(crc_ccitt_crc() & 0xFF);
	tx_to_buf(crc_ccitt_crc() >> 8);

	// program done!
	for (;;)
	{
	}
	
}
