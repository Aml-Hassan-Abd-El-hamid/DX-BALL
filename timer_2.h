void Timer2_Init(unsigned long period);
void disableTimer();
void enableTimer(void);
void Delay100ms(unsigned long count);
static unsigned long TimerCount;
static unsigned long Semaphore;
