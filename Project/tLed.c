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
	PTC->PDOR &= ~MASK(RED_LED);
	PTC->PDOR &= ~(MASK(GREEN_LED) | MASK(GREEN_LED + 1) | MASK(GREEN_LED + 2) 
										| MASK(GREEN_LED + 3) | MASK(GREEN_LED + 4) | MASK(GREEN_LED + 5) 
										| MASK(GREEN_LED + 6) | MASK(GREEN_LED + 7));
}

void led_red_control(mode_t mode) { 
	switch(mode) { 
		case (RUN): 
			// 500ms on, 500ms off
			PTC->PDOR |= MASK(RED_LED); 
			delay(0x80000); 
			PTC->PDOR &= ~MASK(RED_LED); 
			delay(0x80000); 	
			break; 
		case (STOP): 
			// 250ms on, 250ms off 
			PTC->PDOR |= MASK(RED_LED); 
			delay(0x80000); 
			PTC->PDOR &= ~MASK(RED_LED); 
			delay(0x80000); 	
			break; 
	}
}
			
void led_green_control(mode_t mode) { 
	switch(mode) { 
		case (RUN): 
			// running mode 
			for (int i = 0; i < NUM_GREEN_LED; i++) { 
				PTC->PDOR |= MASK(GREEN_LED + i); // set current LED to ON
				delay(0x80000); 
				PTC->PDOR &= ~MASK(GREEN_LED + i);  
			} 
			break; 
		case (STOP): 
			// light up all 
			for (int i = 0; i < NUM_GREEN_LED; i++) { 
				PTC->PDOR |= MASK(GREEN_LED + i); // set current LED to ON
				delay(0x1000); 
				PTC->PDOR &= ~MASK(GREEN_LED + i); 
			} 		
			break; 
	} 
} 	

//Front LED flash green twice when device is connected
void connected_LED(void)
{
	for (int i = 0; i < REPEAT; i++) {
		PTC->PDOR |= (MASK(GREEN_LED) | MASK(GREEN_LED + 1) | MASK(GREEN_LED + 2) 
										| MASK(GREEN_LED + 3) | MASK(GREEN_LED + 4) | MASK(GREEN_LED + 5) 
										| MASK(GREEN_LED + 6) | MASK(GREEN_LED + 7));
		delay(0x80000);
		PTC->PDOR &= ~(MASK(GREEN_LED) | MASK(GREEN_LED + 1) | MASK(GREEN_LED + 2) 
										| MASK(GREEN_LED + 3) | MASK(GREEN_LED + 4) | MASK(GREEN_LED + 5) 
										| MASK(GREEN_LED + 6) | MASK(GREEN_LED + 7));	
		delay(0x80000);
	}
}

void initLED(void)
{
	// Enable Clock to PORTC
	SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK);
	
	// Configure MUX settings to make all 3 pins GPIO
	PORTC->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[RED_LED] |= PORT_PCR_MUX(1);
	for (int i = 0; i < NUM_GREEN_LED; i++) { 		
		PORTC->PCR[GREEN_LED + i] &= ~PORT_PCR_MUX_MASK;
		PORTC->PCR[GREEN_LED + i] |= PORT_PCR_MUX(1);
	} 
	
	// Set Data Direction Registers for PortC
	PTC->PDDR |= MASK(RED_LED) | MASK(GREEN_LED) | MASK(GREEN_LED + 1) | MASK(GREEN_LED + 2) 
										| MASK(GREEN_LED + 3) | MASK(GREEN_LED + 4) | MASK(GREEN_LED + 5) 
										| MASK(GREEN_LED + 6) | MASK(GREEN_LED + 7);
}

/*
void led_control(colour_t colour)
{
	switch(colour)	
	{
		case (RED):
			PTC->PDOR = MASK(GREEN_LED);
			PTC->PDOR &= ~(MASK(RED_LED));
			break;
		case (GREEN):
			PTC->PDOR = MASK(RED_LED);
			PTB->PDOR &= ~(MASK(GREEN_LED));
			break;
		case (OFF):
			offRGB();
			break;
	}
}
*/
