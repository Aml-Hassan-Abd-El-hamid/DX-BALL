#include "tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"
#include "intial.h"

void DisableInterrupts(void);
void EnableInterrupts(void);
void Timer2_Init(unsigned long period);
void Delay100ms(unsigned long count);

void level_screen(int level);

int main(void){

	TExaS_Init(NoLCD_NoScope); // set system clock to 80 MHz.
	Nokia5110_Init();					 // initialize lcd nokia 5110.
	PortF_Init();							 // portF initialization.
	Timer2_Init(800000000/30); // 30 Hz. "controls the speed of the game."
	//--------------------------------------------------------------------------------------
	level_init();       
	start_game();					
	start_screen(); 
	Delay100ms(10);   // delay = 0.1 sec * 30 = 3 sec.
	level_screen(1);  
	draw();						
	life = GetLifes();
	
	while(GetLifes() > 0){
		if(GetLevel() == 1 && GetScore() > 200) { level_screen(2); }
		if(GetLevel() == 2 && GetScore() > 500) { level_screen(3); }
		if(GetLevel() == 3 && GetScore() > 800) { win_screen(); disableTimer(); }
	}
	loss_screen();
}
