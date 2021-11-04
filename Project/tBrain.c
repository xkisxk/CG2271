#include "cmsis_os2.h"

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
