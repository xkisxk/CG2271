#include "stdint.h"
#define TRIG_PIN 2 // PTD2
#define ECHO_PIN 3 //PTD3
#define MASK(x) (1 << (x))

void InitUltrasonicGPIO(void);
void TPM0_IRQHandler();
void initUltrasonicPWM(void);
void delay(volatile uint32_t nof);
void generateTRIG();
int executeUltrasonic(void);