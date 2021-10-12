#include "MKL25Z4.h"                    // Device header

#define UART_TX_PORTE22 22
#define UART_RX_PORTE23 23
#define BAUD_RATE 9600
#define UART2_INT_PRIO 128

void initUART2(uint32_t baud_rate)
{
	uint32_t divisor, bus_clock;
	
	//Enable clock for UART2 and PORT E
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	// Set PORTE_22 to UART TX mode
	PORTE->PCR[UART_TX_PORTE22] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[UART_TX_PORTE22] |= PORT_PCR_MUX(4);
	
	// Set PORTE_23 to UART RX mode
	PORTE->PCR[UART_RX_PORTE23] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[UART_RX_PORTE23] |= PORT_PCR_MUX(4);
	
	// Set UART2 control register 2 to disable transmitter and receiver
	UART2->C2 &= ~((UART_C2_TE_MASK) | (UART_C2_RE_MASK));
	
	// Setting UART2 baud rate modulo divisor
	bus_clock = (DEFAULT_SYSTEM_CLOCK)/2;
	divisor = bus_clock / (baud_rate * 16);
	UART2->BDH = UART_BDH_SBR(divisor >> 8); // Upper 4 bits
	UART2->BDL = UART_BDL_SBR(divisor); // Lower 8 bits
	
	// Set everything on C1,S2,C3 to 0
	UART2->C1 = 0;
	UART2->S2 = 0;
	UART2->C3 = 0;
	
	// Set UART2 to enable transmitter
	UART2->C2 |= ((UART_C2_TE_MASK) | (UART_C2_RE_MASK));
}

void UART2_Transmit_Poll(uint8_t data)
{
	// Wait until TDRE is full and any of the S1 flags are triggered
	while(!(UART2->S1 & UART_S1_TDRE_MASK));
	UART2->D = data;
}

uint8_t UART2_Receive_Poll(void)
{
	// Wait until RDRF is full and any of the S1 flags are triggered
	while(!(UART2->S1 & UART_S1_RDRF_MASK));
	return (UART2->D);
}

static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

int main(void)
{
	uint8_t rx_data = 0x69;
	
	SystemCoreClockUpdate();
	initUART2(BAUD_RATE);
	
	while(1)
	{
		// RX and TX
		UART2_Transmit_Poll(0x69);
		delay(0x80000);
	}
}