#include "MKL25Z4.h"                    // Device header
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "tLed.h"

#define DELAY 500
#define REPEAT 2

static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

void offRGB(void) 
{
	PTB->PDOR = MASK(GREEN_LED) | MASK(RED_LED);
	PTD->PDOR = MASK(BLUE_LED);
}

void led_control(colour_t colour)
{
	switch(colour)	
	{
		case (RED):
			PTD->PDOR = MASK(BLUE_LED);
			PTB->PDOR &= ~(MASK(RED_LED));
			break;
		case (GREEN):
			PTD->PDOR = MASK(BLUE_LED);
			PTB->PDOR &= ~(MASK(GREEN_LED));
			break;
		case (BLUE):
			PTB->PDOR = MASK(GREEN_LED) | MASK(RED_LED);
			PTD->PDOR &= ~(MASK(BLUE_LED));
			break;
		case (OFF):
			offRGB();
			break;
	}
}

//Front and Rear LEDs
void running_mode_LED(void)
{
}

//Front and Rear LEDs
void stationary_mode_LED(void)
{
}

//Front LED flash green when device is connected
void connected_LED(void)
{
	for (int i = 0; i < REPEAT; i++) {
		led_control(GREEN);
		delay(0x80000);
		led_control(OFF);
		delay(0x80000);
	}
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
