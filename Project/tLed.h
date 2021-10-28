#define RED_LED 11 // PortC Pin 11
#define GREEN_LED 3 // PortC Pin 3
#define NUM_GREEN_LED 8 
#define SW_POS 6 // PortD Pin 6
#define MASK(x) (1 << (x))

typedef enum {RED, GREEN, OFF}colour_t;
typedef enum {RUN, STOP} mode_t; 

void offRGB(void);
//void led_control(colour_t colour);
void led_red_control(mode_t mode); 
void led_green_control(mode_t mode); 
void initLED(void);
void connected_LED(void);
