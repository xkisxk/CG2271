#include "MKL25Z4.h"                    // Device header
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "tLed.h"

uint8_t greenLeds[8] = {LED_G3, LED_G4, LED_G5, LED_G6, LED_G7, LED_G8, LED_G9, LED_G10};

void offRed(void) 
{
	PTA->PDOR &= ~MASK(LED_R);
}

void greenFlash() {
	int i;
	for(i = 0; i<2; i++) {
		PTC->PDOR |= (MASK(LED_G3) | MASK(LED_G4) | MASK(LED_G5) | MASK(LED_G6) | MASK(LED_G7) | MASK(LED_G8) | MASK(LED_G9) | MASK(LED_G10));
		osDelay(GREEN_FLASH);
		PTC->PDOR &= (~MASK(LED_G3) & ~MASK(LED_G4) & ~MASK(LED_G5) & ~MASK(LED_G6) & ~MASK(LED_G7) & ~MASK(LED_G8) & ~MASK(LED_G9) & ~MASK(LED_G10));
		osDelay(GREEN_FLASH);
	}
}

void runningModeRed(void)
{
	//RED
	PTA -> PDOR |= MASK(LED_R);
	osDelay(RED_STOP);
	PTA -> PDOR &= ~MASK(LED_R);
	osDelay(RED_STOP);
}

void runningModeGreen(int ledchoice)
{
	PTD -> PDOR |= MASK(greenLeds[ledchoice]);
	osDelay(GREEN_MOVE);
	PTD -> PDOR &= ~MASK(greenLeds[ledchoice]);
}

void stationaryModeRed(void)
{
	PTA -> PDOR |= MASK(LED_R);
	osDelay(RED_MOVE);
	PTA -> PDOR &= ~MASK(LED_R);
	osDelay(RED_MOVE);
}

void stationaryModeGreen(void)
{
		PTC->PDOR |= (MASK(LED_G3) | MASK(LED_G4) | MASK(LED_G5) | MASK(LED_G6) | MASK(LED_G7) | MASK(LED_G8) | MASK(LED_G9) | MASK(LED_G10));
}

void initLED(void) {
	// Enable Clock to PORTC
	SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK);
	
	// Enable Clock for PORTA
	SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK);
	
 // Configure MUX settings for rear led
 PORTA->PCR[LED_R] &= ~PORT_PCR_MUX_MASK;
 PORTA->PCR[LED_R] |= PORT_PCR_MUX(1);
 
 // Configure MUX settings for front led
 PORTC->PCR[LED_G3] &= ~PORT_PCR_MUX_MASK;
 PORTC->PCR[LED_G3] |= PORT_PCR_MUX(1);	
 PORTC->PCR[LED_G4] &= ~PORT_PCR_MUX_MASK;
 PORTC->PCR[LED_G4] |= PORT_PCR_MUX(1);
 PORTC->PCR[LED_G5] &= ~PORT_PCR_MUX_MASK;
 PORTC->PCR[LED_G5] |= PORT_PCR_MUX(1);
 PORTC->PCR[LED_G6] &= ~PORT_PCR_MUX_MASK;
 PORTC->PCR[LED_G6] |= PORT_PCR_MUX(1);
 PORTC->PCR[LED_G7] &= ~PORT_PCR_MUX_MASK;
 PORTC->PCR[LED_G7] |= PORT_PCR_MUX(1);
 PORTC->PCR[LED_G8] &= ~PORT_PCR_MUX_MASK;
 PORTC->PCR[LED_G8] |= PORT_PCR_MUX(1);
 PORTC->PCR[LED_G9] &= ~PORT_PCR_MUX_MASK;
 PORTC->PCR[LED_G9] |= PORT_PCR_MUX(1);
 PORTC->PCR[LED_G10] &= ~PORT_PCR_MUX_MASK;
 PORTC->PCR[LED_G10] |= PORT_PCR_MUX(1);

 // Set Data Direction Registers for PortA and PortC
 PTA->PDDR |= MASK(LED_R);
 PTC->PDDR |= (MASK(LED_G3) | MASK(LED_G4) | MASK(LED_G5) | MASK(LED_G6) | MASK(LED_G7) | MASK(LED_G8) | MASK(LED_G9) | MASK(LED_G10));
}