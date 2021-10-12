#include "MKL25Z4.h"                    // Device header
#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define SW_POS 6 // PortD Pin 6
#define MASK(x) (1 << (x))

typedef enum {RED, GREEN, BLUE}colour_t;

volatile colour_t current_colour = 0;

void Delay(unsigned long duration)
{
	while (duration > 0)	{
		duration--;
	}
}

void PORTD_IRQHandler()
{
	//Clear Pending IRQ
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	
	// Updating some variable/flag
	if (current_colour < 2)	
		current_colour++;
	else 
		current_colour = 0;
	
	//Clear INT flag
	PORTD_ISFR |= MASK(SW_POS);
}

void initLED(void)
{
	// Enable Clock to PORTB and PORTD
	SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));
	
	// Configure MUX settings to make all 3 pins GPIO
	PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1);
	PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1);
	PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);
	
	// Set Data Direction Registers for PortB and PortD
	PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED));
	PTD->PDDR |= MASK(BLUE_LED);
}

void initSwitch()
{
	// Enable Clock for PORTD
	SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK);
	
	/* Select GPIO and enable pull-up resistors and interrupts on
	falling edges of pin connected to switch*/
	PORTD->PCR[SW_POS] |= (PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_IRQC(0x0a));
	
	// Set PORT D Switch bit to input
	PTD->PDDR &= ~(MASK(SW_POS));
	
	//Enable Interrupts
	NVIC_SetPriority(PORTD_IRQn, 2);
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	NVIC_EnableIRQ(PORTD_IRQn);
}

void led_control(colour_t colour)
{
	switch(colour)	
	{
		case (RED):
			PTD->PDOR = MASK(BLUE_LED);
			PTB->PDOR = ~(MASK(RED_LED));
			break;
		case (GREEN):
			PTD->PDOR = MASK(BLUE_LED);
			PTB->PDOR = ~(MASK(GREEN_LED));
			break;
		case (BLUE):
			PTB->PDOR = MASK(GREEN_LED) | MASK(RED_LED);
			PTD->PDOR = ~(MASK(BLUE_LED));
			break;
	}
}

int main(void)
{
	initSwitch();
	initLED();
	
	while(1)
	{
		led_control(current_colour);
		Delay(5000000);
	}
}