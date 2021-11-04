#include "MKL25Z4.h"                    
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "Serial_ISR.h"
#include "tAudio.h"
#include "tLed.h"
#include "tMotorControl.h"

//null command
#define NODATA 0x00

//led commands
#define RED_LED_OFF 0x30
#define RED_LED_ON 0x31
#define GREEN_LED_OFF 0x32
#define GREEN_LED_ON 0x33
#define BLUE_LED_OFF 0x34
#define BLUE_LED_ON 0x35

#define WIFI_STATUS 0x36
#define END_CHALLENGE 0x37

//motor commands
#define MOVE_FORWARD 0x40
#define TURN_LEFT 0x41
#define TURN_RIGHT 0x42
#define MOVE_BACKWARD 0x43
#define MOVE_FORWARD_LEFT 0x44
#define MOVE_FORWARD_RIGHT 0x45
#define MOVE_BACKWARD_LEFT 0x46
#define MOVE_BACKWARD_RIGHT 0x47
#define STOP 0x48
#define SELF_DRIVE 0x49

#define MSG_COUNT 1

osMessageQueueId_t tAudioMsg, tMotorMsg, tLEDMsg, tBrainMsg;

//variable to store data received from UART
uint8_t uartData;

//TODO: add UART IRQ handler??
//UART2 Interrupt Request Handler
void UART2_IRQHandler(void) 
{
	if (UART2->S1 & UART_S1_RDRF_MASK) {
		rx_data = UART2->D;
		osMessageQueuePut(tBrainMsg, &rx_data, NULL, 0);
	}
}

void tAudio() {
	//TODO: add audio code
	uint8_t command = NODATA;
	for(;;) {
		//receive mesage and put it into command
		osMessageQueueGet(tAudioMsg, &command, NULL, osWaitForever);

		if(command == WIFI_STATUS) {
			connected_tune();
			command = NODATA;
		} else if (command == END_CHALLENGE) {
			ending_tune();
			command = NODATA;
		} else {
			background_tune();
		}
	}
}

void tMotor() {
	uint8_t command = NODATA;
	for(;;) {
		//receive mesage and put it into command
		osMessageQueueGet(tMotorMsg, &command, NULL, 0);
		
		if(command == MOVE_FORWARD) {
			forward();
		} else if (command == TURN_LEFT) {
			left();
		} else if (command == TURN_RIGHT) {
			right();
		} else if (command == MOVE_BACKWARD) {
			reverse();
		} else if (command == MOVE_FORWARD_LEFT) {
			leftforward();
		} else if (command == MOVE_FORWARD_RIGHT) {
			rightforward();
		} else if (command == MOVE_BACKWARD_LEFT) {
			leftreverse();
		} else if (command == MOVE_BACKWARD_RIGHT) {
			rightreverse();
		} else {  //STOP
			stopMotors();
		}
	}
}

void tLED() {
	uint8_t command = NODATA;
	
	for(;;) {
		osMessageQueueGet(tLEDMsg, &command, NULL, 0);
		//TODO: add LED code
	}
}

void tBrain() {
	for(;;) {
		//get message from UART IRQ and put it in uartData for every other thread to access
		osMessageQueueGet(tBrainMsg, &uartData, NULL, osWaitForever);
		//send uartData to corresponding thread
		osMessageQueuePut(tMotorMsg, &uartData, NULL, 0);
    osMessageQueuePut(tAudioMsg, &uartData, NULL, 0);
    osMessageQueuePut(tLEDMsg, &uartData, NULL, 0);
	}
}

int main (void) {
  // System Initialization
  SystemCoreClockUpdate();
	
	//Enable board LED
	initLED();
	//Enable Audio
	initAudio();
	//Enable UART
	initUART2(BAUD_RATE);
	//Enable motor
	initMotors();
 
	osKernelInitialize();            

	osThreadNew(tBrain, NULL, NULL);    
  osThreadNew(tMotor, NULL, NULL);   
  osThreadNew(tAudio, NULL, NULL);    
  osThreadNew(tLED, NULL, NULL);    
	
	tBrainMsg = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
  tMotorMsg = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
  tAudioMsg = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
  tLEDMsg = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
		
	osKernelStart();                     
	for (;;) {}
}
