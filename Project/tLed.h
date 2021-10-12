#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define SW_POS 6 // PortD Pin 6
#define MASK(x) (1 << (x))

typedef enum {RED, GREEN, BLUE, OFF}colour_t;

void offRGB(void);
void led_control(colour_t colour);
void initLED(void);
void running_mode_LED(void);
void stationary_mode_LED(void);
void connected_LED(void);
