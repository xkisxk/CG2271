#include "MKL25Z4.h"                    // Device header
unsigned int counter = 0;

#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define TRIG_PIN 2 // PTD2
#define ECHO_PIN 3 //PTD3
#define MASK(x) (1 << (x))
volatile static int startEchoSignal = 0;
volatile static int finishEchoSignal = 0;
volatile static int echoClkCycle = 0;

void InitGPIO(void)
{
//System Clock Gate Control Register	
// Enable Clock to PORTB and PORTD
SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));
// Configure MUX settings to make all 3 pins GPIO
//PORT_PCR_MUX_MASK = 0x700u. Need to clear it first	
PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK; //clear bits 8,9,10 to reset whatever config was there
PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1); //set to gpio
PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1);
PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);
	
PORTD->PCR[TRIG_PIN] &= ~PORT_PCR_MUX_MASK;
PORTD->PCR[TRIG_PIN] |= PORT_PCR_MUX(1);
	
// Set Data Direction Registers for PortB and PortD
PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED));
PTD->PDDR |= MASK(BLUE_LED) | MASK(TRIG_PIN);
}

void TPM0_IRQHandler()
{
// Clear Pending IRQ
	//Nested Vectored Interrupt Controller
	NVIC_ClearPendingIRQ(TPM0_IRQn);
	// Updating some variable / flag
	if(startEchoSignal == 0) { //rising edge
			startEchoSignal = 1;
			finishEchoSignal = 0;
			echoClkCycle = TPM0_C3V;
	} else { //falling edge
		startEchoSignal = 0;
		finishEchoSignal = 1;
		echoClkCycle = TPM0_C3V - echoClkCycle;
	}
	
//Clear Interrupt Flag
	//clear event flag
	TPM0_C3SC |= TPM_CnSC_CHF_MASK;
	//TPM0->SC |= TPM_SC_TOF_MASK;
	PORTD->ISFR |= MASK(ECHO_PIN);
}

void initPWM(void)
{
	//Enable clock gating for PORTD
	//SIM_SCGC5 |= (SIM_SCGC5_PORTD_MASK);
	
	//Configure mux settings to make pins GPIO
	
	PORTD->PCR[ECHO_PIN] &= ~PORT_PCR_MUX_MASK;
	//Alternative 4 TPM0 pg.163 for PTB0
	PORTD->PCR[ECHO_PIN] |= (PORT_PCR_MUX(4) | PORT_PCR_IRQC(0x0b)); //interrupt on either edge
	
	//Enable clock gating for Timer0
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	
	//Select clock for TPM module
	// TPMSRC TPM Source pg.196
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK; //Multipurpose Clock Generator Frequency Locked Loop
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); //set 01 for MCGFLLCLK (48MHz)or MCGFLLCLK/2
	
	//Set modulo value 48000000 / 128 = 375000 / 7500 = 50Hz //128 is PS; 
	//when count reach 7500, signal goes back to 0 and counts back up again
	TPM1->MOD = 7500; //period of PWM waveform
	
	/*Edge-Aligned PWM*/
	//pg.552
	//Update SnC register: CMOD = 01, PS = 111 (128) //TPM is Timer PWM, LPTPM is Low Power
	TPM0->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM0->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM0->SC &= ~(TPM_SC_CPWMS_MASK); //Center-Aligned PWM Select by clearing CPWMS bit
	
	//Enable PWM on TPM0 CHannel 3 -> PTD3 ; setting CnSC to Capture rising and falling edge, enabling channel interrupts
	TPM0_C3SC &= ~( TPM_CnSC_CHIE_MASK | (TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK)); //all this for setting bits
	TPM0_C3SC |= ( TPM_CnSC_CHIE(1) | TPM_CnSC_ELSB(1) | TPM_CnSC_ELSA(1)); //setting elsb, elsa to 1 and everything else to 0 //see pg555
	//TPM0_C0V = TPM_CnV_VAL(0x05);  
	NVIC_SetPriority(TPM0_IRQn,2);
	NVIC_ClearPendingIRQ(TPM0_IRQn);
	NVIC_EnableIRQ(TPM0_IRQn);
}

 /* Delay Function */
 static void delay(volatile uint32_t nof) {
	 while(nof!=0) {
		 __ASM("NOP");
		 nof--;
	 }
 }	
 
 void generateTRIG() {
	  PTD->PDOR &= ~MASK(TRIG_PIN);
		delay(24);
		PTD->PDOR |= MASK(TRIG_PIN);
		delay(120);
		PTD->PDOR &= ~MASK(TRIG_PIN);
 }
 
void offRGB() {
	/* Active low, so set bits to 1 to switch off */
	PTD->PDOR |= MASK(BLUE_LED);
	PTB->PDOR |= MASK(RED_LED);
	PTB->PDOR |= MASK(GREEN_LED);
}

int main(void){
	SystemCoreClockUpdate();
	//initialises GPIO
	InitGPIO();
	initPWM();
	//turns off all LED to prevent white light at the start
	offRGB();
	
	while(1){
		generateTRIG();
		TPM0_CNT = 0;
		//delay(300000);
		if(finishEchoSignal == 0) {
			echoClkCycle = 10000000;
		}
		double duration = (((double) echoClkCycle) / (50 * 7500));
		double distance = 3.4 * duration / 2;
		if(distance > 10){
			PTB->PDOR &= ~MASK(GREEN_LED);
		} else {
			PTB->PDOR |= MASK(GREEN_LED);
		}
		startEchoSignal = 0;
		finishEchoSignal = 1;
		
	}
}