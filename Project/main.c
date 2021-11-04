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

#define WIFI_STATUS 0x36

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

osMessageQueueId_t tAudioMsg, tMotorMsg, tGreenMsg, tRedMsg, tBrainMsg;

//variable to store data received from UART
uint8_t uartData;

//TODO: add UART IRQ handler??

void tAudio() {
	//TODO: add audio code
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

void tGreen() {
	uint8_t command = NODATA;
	int ledchoice = 0;
	
	for(;;) {
		osMessageQueueGet(tGreenMsg, &command, NULL, 0);

		if (command == STOP || !(command == MOVE_FORWARD || command == MOVE_BACKWARD || command == MOVE_FORWARD_LEFT
			|| command == MOVE_FORWARD_RIGHT || command == MOVE_BACKWARD_LEFT || command == MOVE_BACKWARD_RIGHT
			|| command == TURN_LEFT || command == TURN_RIGHT)) {     // If robot is stationary
			stationaryModeGreen();
			//red flash on and off with period 0.5sec
		} else if(command == WIFI_STATUS) {     //On connection with bluetooth.
			//green flash twice
			greenFlash();
			command = STOP;		
			osMessageQueuePut(tGreenMsg, &command, NULL, 0);
		} else if(command != NODATA) { // If the robot is moving.
			ledchoice = (ledchoice + 1) % 8;
			runningModeGreen(ledchoice);
			//green one at a time
			//red flash on and off with period 1.0sec
		}
	}
}

void tRed() {
	uint8_t command = NODATA;
	
	for(;;) {
		osMessageQueueGet(tRedMsg, &command, NULL, 0);

		if (command == STOP || !(command == MOVE_FORWARD || command == MOVE_BACKWARD || command == MOVE_FORWARD_LEFT
			|| command == MOVE_FORWARD_RIGHT || command == MOVE_BACKWARD_LEFT || command == MOVE_BACKWARD_RIGHT
			|| command == TURN_LEFT || command == TURN_RIGHT)) {     // If robot is stationary
			stationaryModeRed();
			//red flash on and off with period 0.5sec
		} else if(command != NODATA) { // If the robot is moving.
			runningModeRed();
			//red flash on and off with period 1.0sec
		}
	}
}

void tBrain() {
	uint8_t data = NODATA;
	for(;;) {
		//get message from UART IRQ and put it in uartData for every other thread to access
		osMessageQueueGet(tBrainMsg, &uartData, NULL, osWaitForever);
		//send uartData to corresponding thread
		osMessageQueuePut(tMotorMsg, &uartData, NULL, 0);
    osMessageQueuePut(tAudioMsg, &uartData, NULL, 0);
    osMessageQueuePut(tGreenMsg, &uartData, NULL, 0);
		osMessageQueuePut(tRedMsg, &uartData, NULL, 0);
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
  osThreadNew(tGreen, NULL, NULL);
	osThreadNew(tRed, NULL, NULL);   	
	
	tBrainMsg = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
  tMotorMsg = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
  tAudioMsg = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
  tGreenMsg = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
	tRedMsg = osMessageQueueNew(MSG_COUNT, sizeof(uint8_t), NULL);
		
	osKernelStart();                     
	for (;;) {}
}
