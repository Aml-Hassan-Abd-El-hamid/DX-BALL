#include "intial.h"
#include "Nokia5110.h"
#include "tm4c123gh6pm.h"
#include "TExaS.h"
#include "time.h"
#include "stdlib.h"
#include "Random.h"


void start_screen (){
   	Nokia5110_Clear();
	Nokia5110_SetCursor(1, 1);
	Nokia5110_OutString(" Start Game");
	Nokia5110_SetCursor(4, 4);
	Nokia5110_OutString(" DX BALL");
	Nokia5110_SetCursor(0, 0);
}

void start_game(){
   int i;
   Bomb_init();
   bunker_y=10;
   bunker_x=8;
   for(i = bunker_x; i < bunker_x + 5; i++)
		set_screen(bunker_y, i, 2);
}

void draw(){
    int i, j;
    Nokia5110_ClearBuffer();
	Nokia5110_Clear();
	for(i = 0; i <  GRID_HIGHT ; i++){
		for(j = 0; j < GRID_WIDTH; j++){
			if(get_screen(i, j) == 1){
				Nokia5110_PrintBMP(4*j, i*4+(Ball_Hight - 1), Balls, 0);
			}
		}
	}	
	Nokia5110_PrintBMP(4*x, y*4+(Ball_Hight - 1), Bomb, 0);
	Nokia5110_PrintBMP(4*bunker_x, bunker_y*4+(BUNKERH - 1), Bunker, 0);		
	Nokia5110_Clear();
	Nokia5110_DisplayBuffer(); 
}

void win_screen(){
    Nokia5110_Clear();
	Nokia5110_SetCursor(1, 1);
	Nokia5110_OutString("You Won");
	Nokia5110_SetCursor(4, 4);
	Nokia5110_OutString("The Game");
	Nokia5110_SetCursor(0, 0);
}

void level_screen(int level){
    int i,j;
	Nokia5110_Clear();
	Nokia5110_SetCursor(1, 2);
	Nokia5110_OutString("  LEVEL  ");	
	Nokia5110_SetCursor(1, 4);
	Nokia5110_OutUDec(level);
	Nokia5110_SetCursor(0, 0);
	set_level(level);
    start_game();
 	for(i = 0; i < 3 + level; i++)
		for(j = 0; j < GRID_WIDTH ; j++){
			set_screen(i, j, 1);	
		}
		Delay100ms(30);
}
void loss_screen(){
	Nokia5110_Clear();
	Nokia5110_SetCursor(1, 1);
	Nokia5110_OutString("GAME OVER");
	Nokia5110_SetCursor(1, 3);
	Nokia5110_OutString("  Score");
	Nokia5110_SetCursor(1, 4);
	Nokia5110_OutUDec(score);
	Nokia5110_SetCursor(0, 0);

}



void Timer2_Init(unsigned long period){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TimerCount = 0;
  Semaphore = 0;
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
	// interrupts enabled in the main program after all devices initialized
	// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}
void disableTimer(){
	  TIMER2_CTL_R = 0x00000000;    //  disable timer2A
}
void enableTimer(void){
		TIMER2_CTL_R = 0x00000001;    //  enable timer2A
}

void Timer2A_Handler(void){ 
	int i,j;
	TIMER2_ICR_R = 0x00000001;   // acknowledge timer2A timeout
  	TimerCount++; 
 	Bomb_move();  
  	if(lifes>0){ // check if the player still have lifes to continue the game
		if(lifes!= life){
			start_game(); 
			life = lifes; // update the value of life
			Delay100ms(20);
    			}
	//read both switches, the right and the left.
	left  = GPIO_PORTF_DATA_R&0x10; // read PF4 into left switch
	right = GPIO_PORTF_DATA_R&0x01; // read PF0 into right switch
		
	if(!left && !right) GPIO_PORTF_DATA_R = 0x02; //if both switches are presed, led is red and the bunker can't move
	else{
		if(!left && right){  // if the left switch is pressed, move the bunker one step to the left
			GPIO_PORTF_DATA_R = 0x04; //if the left switch is pressed, led is blue
			if(bunker_x >= 1){ //check if the bunker is not at the end of the screen from the left
				set_screen(bunker_y, bunker_x+4 ,0); //update the prev position state with 0 
				set_screen(bunker_y ,bunker_x-1 ,2); //update the new position state with 2 
				bunker_x--; //update bunker's x coordinate value after the displacement	
				}
			}
		else if(!right && left){ // if the right switch is pressed, move the bunker one step to the right
			GPIO_PORTF_DATA_R = 0x08; //if right switch is pressed, led is green
			if(bunker_x <= 15) { //check if the bunker is not at the end of the screen from right
       				set_screen(bunker_y, bunker_x, 0); //update the prev position state with 0 
				set_screen(bunker_y, bunker_x+5, 2); //update the new position state with 2 
				bunker_x++; //update bunker's x coordinate value after the displacement
				}
			}
		else GPIO_PORTF_DATA_R = 0x00; //if no switches are pressed, do nothing
		}
		draw();  //call draw function to draw the screen after each change
	}	
  Semaphore = 1; // trigger
}

void Delay100ms(unsigned long count){
	unsigned long volatile time;
	disableTimer();
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
	enableTimer();
}
void PortF_Init(void){ 
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0        
}

void Bomb_init(){
    direction = 1;
	x = 10;
	y = 9;
}
void level_init(){
    score = 0;
	level = 1;
    life=1;
}
void Bomb_move(){
    int Bomb_state;
    if(direction ==0){
        Bomb_state= check_bomb_state(y-1,x-1);
        switch_state(Bomb_state);
        if(Bomb_state==0){
            y--;
            x--;
        }     
    }
    else if(direction==1){
        Bomb_state= check_bomb_state(y-1,x);
			switch_state(Bomb_state);
			if(Bomb_state == 0)
                y--; 
    }
    
    else if(direction==2){
        Bomb_state= check_bomb_state(y-1,x+1);
			switch_state(Bomb_state);
			if(Bomb_state == 0){
                y--; 
                x++;}
    }
    
    else if(direction==3){
            Bomb_state= check_bomb_state(y+1,x-1);
			switch_state(Bomb_state);
			if(Bomb_state == 0){
                y++; 
                x--;
                }
    }
    
    else if(direction==4){
            Bomb_state= check_bomb_state(y+1,x);
			switch_state(Bomb_state);
			if(Bomb_state == 0){
                y++; 
                }
    }
    
    else if(direction==5){
            Bomb_state= check_bomb_state(y+1,x+1);
			switch_state(Bomb_state);
			if(Bomb_state == 0){
                y++; 
                x++;
                }
    }
}


void Bomb_change_dir(int state){
	
	if(state == 1) // the ball hits the cell
{
	}
	
}

void set_screen(int i,int j,int state){
    state_screen[i][j]=state; 
}
int get_screen(int i,int j){return state_screen[i][j]; }

void set_level(int levelVal) {level = levelVal;}
