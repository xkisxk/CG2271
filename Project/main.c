#include "Serial_ISR.h"
#include "tAudio.h"
#include "tLed.h"
#include "tMotorControl.h"

#define RED_LED_OFF 0x30
#define RED_LED_ON 0x31
#define GREEN_LED_OFF 0x32
#define GREEN_LED_ON 0x33
#define BLUE_LED_OFF 0x34
#define BLUE_LED_ON 0x35
#define WIFI_STATUS 0x36

#define MOVE_FORWARD 0x40
#define TURN_LEFT 0x41
#define TURN_RIGHT 0x42
#define MOVE_BACKWARD 0x43
#define SELF_DRIVE 0x44

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
  while (1) 
	{
		switch (rx_data) {
			case (RED_LED_ON):
				led_red_control(RUN);
				break;
			case (RED_LED_OFF):
				led_red_control(STOP);
				break;
			case (GREEN_LED_ON):
				led_green_control(RUN);
				background_tune();
				break;
			case (GREEN_LED_OFF):
				led_green_control(STOP);
				off_audio();
				break;
			case (WIFI_STATUS):
				connected_tune();
				connected_LED();
				rx_data = 0x00;
				break;
			default:
				offLED(); 
				off_audio();
		}
	}
}
