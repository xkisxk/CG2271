#include "tMotorControl.h"
#include "tUltrasonic.h"
#include "cmsis_os2.h" 
#include "MKL25Z4.h"

static void delay(volatile uint32_t nof) {
  while(nof!=0) {
    __asm("NOP");
    nof--;
  }
}

void initMotors(void)
{
	//Enable clock gating for PORTB
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	//Configure mux settings to make pins GPIO
	//Configure mode 3 for PWM pin operation
	PORTB->PCR[LEFT_FW] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[LEFT_FW] |= PORT_PCR_MUX(3);
	PORTB->PCR[LEFT_BK] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[LEFT_BK] |= PORT_PCR_MUX(3);
	
	PORTB->PCR[RIGHT_FW] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RIGHT_FW] |= PORT_PCR_MUX(3);
	PORTB->PCR[RIGHT_BK] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RIGHT_BK] |= PORT_PCR_MUX(3);
	
	//Enable clock gating for Timer1
	SIM->SCGC6 |= (SIM_SCGC6_TPM1_MASK | SIM_SCGC6_TPM2_MASK);
	
	//Select clock for TPM module
	// TPMSRC TPM Source pg.196
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK; //Multipurpose Clock Generator Frequency Locked Loop
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); //set 01 for MCGFLLCLK (48MHz)or MCGFLLCLK/2
	
	//Set modulo value 48000000 / 128 = 375000 / 7500 = 50Hz //128 is PS; 
	//when count reach 7500, signal goes back to 0 and counts back up again
	TPM1->MOD = MOD_VAL; //period of PWM waveform
	TPM2->MOD = MOD_VAL;
	
	/*Edge-Aligned PWM*/
	//pg.552
	//Update SnC register: CMOD = 01, PS = 111 (128) //TPM is Timer PWM, LPTPM is Low Power
	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK); //Center-Aligned PWM Select by clearing CPWMS bit
	
	TPM2->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM2->SC &= ~(TPM_SC_CPWMS_MASK);
	
	//Enable PWM on TPM1 CHannel 0 -> PTB0
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK)); //all this for setting bits
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1)); //setting elsb, mab to 1 and everything else to 0 //see pg555
	
	//Enable PWM on TPM1 Channel 1 -> PTB1
	TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	//Enable PWM on TPM2 channel 0 -> PTB2
	TPM2_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM2_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
	//Enable PWM on TPM2 channel 1 -> PTB3
	TPM2_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM2_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
	
}

void stopMotors() {
	TPM1->MOD = 0;
	TPM1_C0V = 0; //stop left fw
	TPM1_C1V = 0; //stop left bk
	
	TPM2->MOD = 0;
	TPM2_C0V = 0; //stop right fw
	TPM2_C1V = 0; //stop right bk
}	

void forward() {
  TPM1->MOD = MOD_VAL;
	TPM1_C0V = (int) (MOD_VAL * DUTY_CYCLE);
	TPM1_C1V = 0;
	
	TPM2->MOD = MOD_VAL;
	TPM2_C0V = (int) (MOD_VAL * DUTY_CYCLE);
	TPM2_C1V = 0;
}

void selfdriveforward() {
  TPM1->MOD = MOD_VAL;
	TPM1_C0V = (int) (MOD_VAL * SDDC);
	TPM1_C1V = 0;
	
	TPM2->MOD = MOD_VAL;
	TPM2_C0V = (int) (MOD_VAL * SDDC);
	TPM2_C1V = 0;
}

void reverse() {
  TPM1->MOD = MOD_VAL;
	TPM1_C0V = 0;
	TPM1_C1V = (int) (MOD_VAL * DUTY_CYCLE);
	
	TPM2->MOD = MOD_VAL;
	TPM2_C0V = 0;
	TPM2_C1V = (int) (MOD_VAL * DUTY_CYCLE);
}

void left() {
	//left reverse
	TPM1->MOD = MOD_VAL;
	TPM1_C0V = 0;
	TPM1_C1V = (int) (MOD_VAL * DUTY_CYCLE);
	
	//right forward
	TPM2->MOD = MOD_VAL;
	TPM2_C0V = (int) (MOD_VAL * DUTY_CYCLE);
	TPM2_C1V = 0;
}

void right() {
	//left forward
	TPM1->MOD = MOD_VAL;
	TPM1_C0V = (int) (MOD_VAL * DUTY_CYCLE);
	TPM1_C1V = 0;
	
	//right reverse
	TPM2->MOD = MOD_VAL;
	TPM2_C0V = 0;
	TPM2_C1V = (int) (MOD_VAL * DUTY_CYCLE);
}

void leftforward() {
	//left forward but slower
	TPM1->MOD = MOD_VAL;
	TPM1_C0V = (int) (MOD_VAL * SLOWER);
	TPM1_C1V = 0;
	
	//right forward
	TPM2->MOD = MOD_VAL;
	TPM2_C0V = (int) (MOD_VAL * DUTY_CYCLE);
	TPM2_C1V = 0;
}

void rightforward() {
	//left forward but slower
	TPM1->MOD = MOD_VAL;
	TPM1_C0V = (int) (MOD_VAL * DUTY_CYCLE);
	TPM1_C1V = 0;
	
	//right forward
	TPM2->MOD = MOD_VAL;
	TPM2_C0V = (int) (MOD_VAL * SLOWER);
	TPM2_C1V = 0;
}

void leftreverse() {
	//left reverse but slower
	TPM1->MOD = MOD_VAL;
	TPM1_C0V = 0;
	TPM1_C1V = (int) (MOD_VAL * SLOWER);
	
	//right reverse
	TPM2->MOD = MOD_VAL;
	TPM2_C0V = 0;
	TPM2_C1V = (int) (MOD_VAL * DUTY_CYCLE);
}

void rightreverse() {
	//left reverse
	TPM1->MOD = MOD_VAL;
	TPM1_C0V = 0;
	TPM1_C1V = (int) (MOD_VAL * DUTY_CYCLE);
	
	//right reverse but slower
	TPM2->MOD = MOD_VAL;
	TPM2_C0V = 0;
	TPM2_C1V = (int) (MOD_VAL * SLOWER);
}

void selfDrive() {
	initUltrasonic();
	int flag = 0;
	selfdriveforward();

	while(!flag) {
			flag = executeUltrasonic();
	}
	flag = 0;
	
	//turn 90 degress left
	left();
	osDelay(330);
	//move forward
	forward();
	osDelay(300);
	//turn 120 degress right
	right();
	osDelay(440);
	//move forward
	forward();
	osDelay(600);
	//turn 120 degress right
	right();
	osDelay(440);
  //move forward
	forward();
	osDelay(600);
	//turn 120 degrees right
	right();
	osDelay(460);
	//move forward
	forward();
	osDelay(300);
	//turn 90 degress left
	left();
	osDelay(330);
	
	selfdriveforward();
	while(!flag) {
			flag = executeUltrasonic();
	}
	flag = 0;
	
	stopMotors();
	
}
