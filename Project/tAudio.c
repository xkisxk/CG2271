#include "MKL25Z4.h"                    // Device header
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "tAudio.h"

#define REPEAT 2

int len[4] = {1000, 500, 250, 125};
int freq[SIZE] = {262, 294, 330, 349, 392, 440, 494}; 

void play_note(notes note, lengths length)
{
	TPM1->MOD = ClockFreq / freq[note];
	TPM1_C0V = TPM1->MOD * 0.8;
	osDelay(len[length]);
}

void connected_tune(void)
{
	for (int i = 0; i < REPEAT; i++) {
		play_note(D, semiquaver);
		play_note(A, semiquaver);
		osDelay(len[quaver]);
	}
}

void background_tune(void)
{
}

void ending_tune(void)
{
}

void initPWM(void)
{
	// Enable clock for PortB
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	// TPM - Timer/PWM Module
	// Clear MUX Mask, then setting it to TPM1_CH0
	// Port B Pin 0
	PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3);
	
	// Clear MUX Mask, then setting it to TPM1_CH1
	// Port B Pin 1
	PORTB->PCR[PTB1_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB1_Pin] |= PORT_PCR_MUX(3);
	
	// Enable clock for TPM1
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	
	// Clear TPMSRC, then setting it to MCGFLLCLK or MCGFLLCLK/2
	//Multipurpose Clock Generator Fixed Frequency Loop Clock
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	//48MHz / 128 = 375kHz
	//375kHz / 50Hz = 7500
	TPM1->MOD = 7500;
	
	// 80% Duty Cycle
	// MOD Count * 0.8 = 6000
	TPM1_C0V = 6000;
		
	// Clear CMOD and PS, then setting CMOD to increment counting
	// and PS to divide by 128
	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	
	// Clear Centre Aligned PWM which sets LPTPM to up counting mode
	// LPTPM - Low Power TPM
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
	
	// Clearing TPM1_C0SC, then setting MSB:MSA and ELSB:ELSA to 10
	// which is Edge-Aligned PWM with High true pulses (clear output on match, set output on reload)
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}
