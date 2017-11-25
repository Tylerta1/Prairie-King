//Test

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "Player.h"
#include "ADC.h"
#include "Timer1.h"

int ADCStatus1 = 0;
int ADCStatus2 = 0;
int Move[4] = {0,0,0,0};

uint8_t player1shoot;
uint8_t player1shootcount;

void Joy_In(void){
	ADC_In(Move);
	ADCStatus1 = 1;
	ADCStatus2 = 1;
}

void Input_Init(void){
	player1shoot = 0;
	player1shootcount = 0;
	
	SYSCTL_RCGCGPIO_R |= 0x20;  //   10 0000
	while((SYSCTL_RCGCGPIO_R&0x20) == 0){}; //delay
	GPIO_PORTF_AFSEL_R &= ~0x01; // Regular function on PF0
	GPIO_PORTF_AMSEL_R &= ~0x01; // disable analog function on PF0
	GPIO_PORTF_DEN_R |= 0x01; // enable digital on PF0
	GPIO_PORTF_DIR_R &= ~0x01; // Input assigned to PF0
	
	ADC_Init();
	Timer1_Init(&Joy_In,2000000);
}

uint8_t PlayerMove(void){
	
	while(ADCStatus1 == 0){};
	ADCStatus1 = 0;
	//Move[0] - P1 Horizontal
	//Move[1] - P1 Vertical
	if(Move[0] < 1250){
		if(Move[1] < 1250){
			return DOWNRIGHT;
		}
		if(Move[1] >2850){
			return UPRIGHT;
		}
		else{
			return RIGHT;
		}
	}
	if(Move[0] > 2850){
		if(Move[1] <1250){
			return DOWNLEFT;
		}
		if(Move[1] > 2850){
			return UPLEFT;
		}
		else{
			return LEFT;
		}
	}
	if(Move[1] < 1250){
		if(Move[0] < 1250){
			return DOWNRIGHT;
		}
		if(Move[0] > 2850){
			return DOWNLEFT;
		}
		else{
			return DOWN;
		}
	}
	if(Move[1] > 2850){
		if(Move[0] < 1250){
			return UPRIGHT;
		}
		if(Move[0] > 2850){
			return UPLEFT;
		}
		else{
			return UP;
		}
	}
	else{
		return NOTHING;
	}
}

uint8_t Input_Player1Shoot(void){
	if((GPIO_PORTF_DATA_R >> 4) & 0x01 && player1shootcount == 0){
		player1shootcount = weapons[player1.currentweapon].rate;
		return 1;
	}
	else if(player1shootcount != 0){
		player1shootcount--;
	}
	
	return 0;
}
	
