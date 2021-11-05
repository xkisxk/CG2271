#include "MKL25Z4.h"                    // Device header
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "tLed.h"

uint8_t greenLeds[8] = {LED_G3, LED_G4, LED_G5, LED_G6, LED_G7, LED_G8, LED_G9, LED_G10};

static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

void greenFlash(void) {
	int i;
	for(i = 0; i<2; i++) {
		PTC->PDOR |= (MASK(LED_G3) | MASK(LED_G4) | MASK(LED_G5) | MASK(LED_G6) | MASK(LED_G7) | MASK(LED_G8) | MASK(LED_G9) | MASK(LED_G10));
		osDelay(GREEN_FLASH);
		//delay(0x80000);
		PTC->PDOR &= (~MASK(LED_G3) & ~MASK(LED_G4) & ~MASK(LED_G5) & ~MASK(LED_G6) & ~MASK(LED_G7) & ~MASK(LED_G8) & ~MASK(LED_G9) & ~MASK(LED_G10));
		osDelay(GREEN_FLASH);
		//delay(0x80000);
	}
}

void runningModeRed(void)
{
	//RED
	PTA -> PDOR |= MASK(LED_R);
	osDelay(RED_MOVE);
	//delay(0x80000);
	PTA -> PDOR &= ~MASK(LED_R);
	osDelay(RED_MOVE);
	//delay(0x80000);
}

void runningModeGreen(int ledChoice)
{
	PTC->PDOR &= (~MASK(LED_G3) & ~MASK(LED_G4) & ~MASK(LED_G5) & ~MASK(LED_G6) & ~MASK(LED_G7) & ~MASK(LED_G8) & ~MASK(LED_G9) & ~MASK(LED_G10));
	PTC -> PDOR |= MASK(greenLeds[ledChoice]);
	osDelay(GREEN_MOVE);
	//delay(0x1000);
	PTC -> PDOR &= ~MASK(greenLeds[ledChoice]);
	osDelay(GREEN_MOVE);
}

void stationaryModeRed(void)
{
	PTA -> PDOR |= MASK(LED_R);
	osDelay(RED_STOP);
	//delay(0x40000);
	PTA -> PDOR &= ~MASK(LED_R);
	osDelay(RED_STOP);
	//delay(0x40000);
}

void stationaryModeGreen(void)
{
	PTC->PDOR |= (MASK(LED_G3) | MASK(LED_G4) | MASK(LED_G5) | MASK(LED_G6) | MASK(LED_G7) | MASK(LED_G8) | MASK(LED_G9) | MASK(LED_G10));
}

void initLED(void) {
 // Enable Clock to PORTC
 SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK) | (SIM_SCGC5_PORTA_MASK);
	
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