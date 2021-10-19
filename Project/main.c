#include "Serial_ISR.h"
#include "tAudio.h"
#include "tLed.h"

#define RED_LED_OFF 0x30
#define RED_LED_ON 0x31
#define GREEN_LED_OFF 0x32
#define GREEN_LED_ON 0x33
#define BLUE_LED_OFF 0x34
#define BLUE_LED_ON 0x35
#define WIFI_STATUS 0x36

int main (void) {
  // System Initialization
  SystemCoreClockUpdate();
	
	//Enable board LED
	initLED();
	//Enable Audio
	initPWM();
	//Enable UART
	initUART2(BAUD_RATE);
  while (1) 
	{
		switch (rx_data) {
			case (RED_LED_ON):
				led_control(RED);
				break;
			case (RED_LED_OFF):
				led_control(OFF);
				break;
			case (GREEN_LED_ON):
				led_control(GREEN);
				break;
			case (GREEN_LED_OFF):
				led_control(OFF);
				break;
			case (BLUE_LED_ON):
				led_control(BLUE);
				break;
			case (BLUE_LED_OFF):
				led_control(OFF);
				break;
			case (WIFI_STATUS):
				connected_tune();
				connected_LED();
				rx_data = 0x00;
				break;
			default:
				led_control(OFF);
				off_audio();
		}
	}
}
