#define PTB0_Pin 0
#define PTB1_Pin 1
#define ClockFreq 375000
#define SIZE 7

typedef enum {C, D, E, F, G, A, B} notes;
typedef enum {minim, crotchet, quaver, semiquaver} lengths;

void play_note(notes note, lengths length);
void connected_tune(void);
void background_tune(void);
void ending_tune(void);
void initPWM(void);
