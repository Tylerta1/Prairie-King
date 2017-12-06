// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 11/21/2017 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2017

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2017

 Copyright 2017 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground
/************************************************************************************************
PORTS USED:
	PORT A - LCD DRIVER
	PORT B - 0-5 ARE BEING USED FOR DAC  -> Init is in DAC.C
	PORT C - 
	PORT D - 4 BUTTONS FOR SHOOTING 
		D0 - DOWN
		D1 - RIGHT
		D2 - UP
		D3 - LEFT
	PORT E - 3-4 ARE BEING USED FOR ADC  -> Init is in ADC.C	
		E0 - X ADC
		E1 - Y ADC
	PORT F -


************************************************************************************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "PLL.h"
#include "ADC.h"
#include "sound.h"
#include "Sprite.h"
#include "Timer1.h"
#include "Timer0.h"
#include "Pictures.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds

// *************************** Images ***************************
/*******************************************************************************
																	Library of images 
Map - Map of Game   ////// Top Cactus's are 10 pixels height from 0 to 128, Left Side Cactus is 8 pixels, Right side is 8 pixels, bottom cactus's are 10 pixels
	width = 128
	height = 160
Map2 - Maybe make game move a little
	width = 128
	height = 160
Sprite_Bullet
	width = 9
	height = 9
LeftEnemy - Left foot forward of enemy
	width = 24
	height = 24
RightEnemy - Right foot forward of enemy
	width = 24
	height = 24
Sprite_PlayerDown
	width = 24
	height = 24
*****************************************************************************/
/***************************************************************************
																		BOUNDARIES
	CAN'T HAVE A PIXEL OVERWRITE THESE COORDINATES
	([0-128],[0-10]) 
	([0-128],[150-160])
	([0-8],[0-160])
	([120-128],[0-160])
	
	FOR 24x24
		MINIMUM X: 8
		MAX X: 96
		MINIMUM Y: 34
		MAX Y: 150
	
	FOR 9X9
		MINIMUM X: 8
		MAX X:111 
		MINIMUM Y: 19
		MAX Y: 150

	//	xold = x before move
//  yold = yenemy_collision(&Player, &Enemies[i]);

// print old, with a square 
// print new, in that order
// void (*AI)(void) previ
// previous  = PORTE_DATA_&0sadi0a
	***************************************************************************/

struct Character{
	int16_t xpos;    //Where to print on x-coordinate
	int16_t ypos;		// Where to print on y-coordinate
	const uint16_t *images[4];  //WHAT to print
	uint8_t direction; //What direction it is at.
	int16_t health;  //Keep track of health
	uint8_t alive;	//should be alive or not
	uint8_t width;  //how big the image is
	uint8_t height; // ^
	uint8_t destroy; // if to be destroyed or not
};

struct Bullet{
	const uint16_t *image[1]; // What to print
	int16_t xpos; //Where to print on x-coordinate
	int16_t ypos; //Where to print on y-coordinate
	uint8_t width;  //how big the image is
	uint8_t height; // ^
	int16_t direction; //Where it will shoot
	uint8_t alive; //when bullet collides with something, i.e. wall, enemy, barrell
	uint8_t destroy;
};
typedef struct Character Character_t;
typedef struct Bullet Bullet_t;
Character_t Player;  // Struct of Main Player
Character_t Enemy;   //Struct of an Enemy
Bullet_t Bullet;		//Struct of Bullet
struct Character Enemies[10];
struct Bullet Bullets[10];
int UP = 0;
int DOWN = 1;
int LEFT = 2;
int RIGHT = 3;
int UPRIGHT = 4;
int UPLEFT = 5;
int DOWNRIGHT = 6;
int DOWNLEFT = 7;
int STAY = 8;
int ADCStatus = 0;
int Move[2] = {0,0};
int speed_n=1;
int bullet_count=0;
int Gate=0;	
int xpos=0;
int ypos=0;
int Num_Enemies=0;
int South_status=0;
int South_count=0;
int East_status=0;
int East_count=0;
int North_status=0;
int North_count=0;
int West_status=0;
int West_count=0;
int check;
int count_down = 0;
int count_up = 0;
int count_left = 0;
int count_right = 0;
int walk = 0;
int hundreds=35;
int tens=40;
int ones=45;
int killed =0;


void PortD_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x08; //Port D clock turned on
	while((SYSCTL_PRGPIO_R&0x08) == 0){
	}; //delay
	GPIO_PORTD_DIR_R &= ~0x0F;  //PD0-3 Input
	GPIO_PORTD_AFSEL_R &= ~0x0F; // Regular for PD0-3
	GPIO_PORTD_DEN_R |= 0x0F; // Enable digital I/O for PD0-3
	GPIO_PORTD_AMSEL_R &= ~0x0F; // Disable analog for PD0-3
	GPIO_PORTD_PCTL_R &= ~0xFF;
}
void Print_Char(Character_t *character){
	ST7735_DrawBitmap(character->xpos, character->ypos, character->images[character->direction], character->width, character->height);
};
void Print_Bullet(Bullet_t *bullet){
	ST7735_DrawBitmap(bullet->xpos, bullet->ypos, bullet->image[0], bullet->width, bullet->height);
};
void LCD_RemoveChar(Character_t *character){
	ST7735_FillRect(character->xpos, character->ypos, character->width, character->height, 0x55FE);
}
void LCD_RemoveBullet(Bullet_t *bullet){
	ST7735_FillRect(bullet->xpos, bullet->ypos, bullet->width, bullet->height, 0x55FE);
}
void char_init(){ 
	Player.alive = 1;
	Player.direction = 0;
	Player.xpos = 50;
	Player.ypos = 80;
	Player.height = 24;
	Player.width = 24;
	Player.images[0] = Sprite_PlayerDown;
	Player.destroy = 0;
}
void bullet_init(){
	for(int i = 0; i<10; i++){
		Bullets[i].image[0] = Sprite_Bullet;
		Bullets[i].height = 9;
		Bullets[i].width = 9;
		Bullets[i].alive = 0;
		Bullets[i].destroy = 0;
	}
}
void enemy_init(){
	for(int i = 0; i<10; i++){
		Enemies[i].alive = 0;
		Enemies[i].direction = 0;
		Enemies[i].height = 24;
		Enemies[i].width = 24;
		Enemies[i].images[0] = LeftEnemy;
		Enemies[i].images[1] = RightEnemy;
		Enemies[i].destroy = 0;
	}
}

uint16_t enemy_collision(Character_t *character, Character_t *character2){ //if character will collide with character2
	int one_topleft_x = (character->xpos);
	int one_topleft_y = ((character->ypos) - (character->height) + 1);
	int one_botright_x = ((character->xpos) + (character->width) - 1);
	int one_botright_y = (character->ypos);
		

	//same for 2nd char
	int two_topright_x = (character2->xpos) + (character2->width) - 1;
	int two_topright_y = (character2->ypos) - (character2->height) + 1 ;
	int two_botright_x = (character2->xpos) + (character2->width) - 1;
	int two_botright_y = (character2->ypos);
	int two_topleft_x = (character2->xpos);
	int two_topleft_y = (character2->ypos) - (character2->height) + 1;
	int two_botleft_x = (character2->xpos);
	int two_botleft_y = (character2->ypos);
	if(character2->alive == 0){
		return 0;
	}
	if(character->alive == 0){
		return 0;
	}
	
	// if (topright_x is in between leftline and rightline) && (topright_y is in between topline and botomline)
	if((one_topleft_x <= two_topright_x) && (two_topright_x <= one_botright_x) && (one_topleft_y <= two_topright_y) && (two_topright_y <= one_botright_y)){
		return 1;
	}
	if((one_topleft_x <= two_botright_x) && (two_botright_x <= one_botright_x) && (one_topleft_y <= two_botright_y) && (two_botright_y <= one_botright_y)){
		return 1;
	}
	if((one_topleft_x <= two_topleft_x) && (two_topleft_x <= one_botright_x) && (one_topleft_y <= two_topleft_y) && (two_topleft_y <= one_botright_y)){
		return 1;
	}
	if((one_topleft_x <= two_botleft_x) && (two_botleft_x <= one_botright_x) && (one_topleft_y <= two_botleft_y) && (two_botleft_y <= one_botright_y)){
		return 1;
	}
	else{
		return 0;
	}
}

uint16_t bullet_collision(Character_t *character, Bullet_t *character2){ //if character will collide with bullet
	int leftline = (character->xpos)+4; //left line. x = xpos
	int rightline = ((character->xpos) + (character->width) - 1)-4; // right line. x = xpos + wide
	int topline = ((character->ypos) - (character->height) + 1)+3; //  top line. y = ypos - height since low y value is higher on screen
	int botline = (character->ypos)-4;                           // y = ypos
	
	//same for 2nd char
	int topright_y = (character2->ypos) - (character2->height) + 1 ;
	int topright_x = (character2->xpos) + (character->width) - 1;
	int botleft_y = (character2->ypos);
	int botleft_x = (character2->xpos);
	int topleft_y = (character2->ypos) - (character2->height) + 1;
	int topleft_x = (character2->xpos);
	int botright_y = (character2->ypos);
	int botright_x = (character2->xpos) + (character->width) - 1;
	if(character->alive == 0){
		return 0;
	}
	if(character2->alive == 0){
		return 0;
	}
	
	// if (topright_x is in between leftline and rightline) && (topright_y is in between topline and botomline)
	if((leftline <= topright_x) && (topright_x <= rightline) && (topline <= topright_y) && (topright_y <= botline)){
		return 1;
	}
	if((leftline <= botright_x) && (botright_x <= rightline) && (topline <= botright_y) && (botright_y <= botline)){
		return 1;
	}
	if((leftline <= topleft_x) && (topleft_x <= rightline) && (topline <= topleft_y) && (topleft_y <= botline)){
		return 1;
	}
	if((leftline <= botleft_x) && (botleft_x <= rightline) && (topline <= botleft_y) && (botleft_y <= botline)){
		return 1;
	}
	else{
		return 0;
	}
}
void Create_Bullet(int16_t bullet_count, int16_t xpos, int16_t ypos, int16_t direction){
	Bullets[bullet_count].alive=1;
	Bullets[bullet_count].xpos=xpos;
	Bullets[bullet_count].ypos=ypos;
	Bullets[bullet_count].direction=direction;
}

void Check_Gun_Buttons(void){
	if((GPIO_PORTD_DATA_R&0x01)==0x01){				//shooting down offset to print below the characters feet
		if(Bullets[(bullet_count+1)].alive==0){
			if(count_down == 0){
				bullet_count = (bullet_count+1)%10;
				Create_Bullet(bullet_count, ((Player.xpos)+8),((Player.ypos)+8),DOWN);
				count_down = 10;
			}
			count_down--;
		}
	}
	if((GPIO_PORTD_DATA_R&0x02)==0x02){				//shooting right offset to print from the right of the character
		if(Bullets[(bullet_count+1)].alive==0){
			if(count_right == 0){
				bullet_count = (bullet_count+1)%10;
				Create_Bullet(bullet_count, ((Player.xpos)+20),((Player.ypos)-6),RIGHT);
				count_right = 10;
			}
			count_right--;
		}
	}
	if((GPIO_PORTD_DATA_R&0x04)==0x04){				//shooting up offset to print above character's head
		if(Bullets[(bullet_count+1)].alive==0){
			if(count_up == 0){
				bullet_count = (bullet_count+1)%10;
				Create_Bullet(bullet_count, ((Player.xpos)+8),((Player.ypos)-20),UP);
				count_up = 10;
			}
			count_up--;
		}
	}
	if((GPIO_PORTD_DATA_R&0x08)==0x08){				//shooting left offset to print to the left of the character
		if(Bullets[(bullet_count+1)].alive==0){
			if(count_left == 0){
				bullet_count = (bullet_count+1)%10;
				Create_Bullet(bullet_count, ((Player.xpos)-8),((Player.ypos)-6),LEFT);
				count_left = 10;
			}
			count_left--;
		}
	}
}

void Move_Bullets(void){
	for(int i=0; i<10; i++){
		if(Bullets[i].alive==1){
			if(Bullets[i].direction==DOWN){		//down
				if(Bullets[i].ypos > 148){
					Bullets[i].alive=0;
					Bullets[i].destroy = 1;
				}
				else{
					Bullets[i].ypos=(Bullets[i].ypos+1);
					ST7735_DrawBitmap(Bullets[i].xpos, Bullets[i].ypos, Bullets[i].image[0], Bullets[i].width, Bullets[i].height);
				}
			}
			if(Bullets[i].direction==RIGHT){		//right
				if(Bullets[i].xpos>109){
					Bullets[i].alive=0;
					Bullets[i].destroy = 1;
				}
				else{
					Bullets[i].xpos=(Bullets[i].xpos+1);
					ST7735_DrawBitmap(Bullets[i].xpos, Bullets[i].ypos, Bullets[i].image[0], Bullets[i].width, Bullets[i].height);
				}
			}		
			if(Bullets[i].direction==UP){		//up
				if(Bullets[i].ypos<21){
					Bullets[i].alive=0;
					Bullets[i].destroy = 1;
				}
				else{
					Bullets[i].ypos=(Bullets[i].ypos-1);
					ST7735_DrawBitmap(Bullets[i].xpos, Bullets[i].ypos, Bullets[i].image[0], Bullets[i].width, Bullets[i].height);
				}
			}
			if(Bullets[i].direction==LEFT){		//left
				if(Bullets[i].xpos<9){
					Bullets[i].alive=0;
					Bullets[i].destroy = 1;
				}
				else{
					Bullets[i].xpos=(Bullets[i].xpos-1);
					ST7735_DrawBitmap(Bullets[i].xpos, Bullets[i].ypos, Bullets[i].image[0], Bullets[i].width, Bullets[i].height);
				}
			}
		}
	}
}

void Create_Enemy(int16_t Enemy_Count, int16_t direction,int16_t xpos, int16_t ypos){
	Enemies[Enemy_Count].alive=1;
	Enemies[Enemy_Count].xpos=xpos;
	Enemies[Enemy_Count].ypos=ypos;
	Enemies[Enemy_Count].direction=direction;
}
void Spawn_Enemies(void){
	/*
	for(int i=0;i<10;i++){
		if(Enemies[i].alive==1){
			count++;
		}
	}
	*/
	if(Num_Enemies <= 7){
		Gate=(Random()%4);				//pick Random location to spawn enemy
		if(Gate==0){							//spawn at bottom of map
			South_count++;
			if(South_count==24){
				South_status=0;
				South_count = 0;
			}
			if(South_status==0){
				Create_Enemy(Num_Enemies, DOWN ,52, 150); //
				Num_Enemies++;
				South_status=1;
			}
		}
		if(Gate==1){							//spawn at right of map
			East_count++;
			if(East_count==24){
				East_status=0;
				East_count = 0;
			}
			if(East_status==0){
				Create_Enemy(Num_Enemies, RIGHT ,96, 96);
				Num_Enemies++;
				East_status = 1;
			}
		}
		if(Gate==2){							//spawn at top of map
			North_count++;
			if(North_count==24){
				North_status=0;
				North_count = 0;
			}
			if(North_status==0){
				Create_Enemy(Num_Enemies, UP, 52, 34);
				Num_Enemies++;
				North_status = 1;
			}
		}
		if(Gate==3){							//spawn at left of map
			West_count++;
			if(West_count==24){
				West_status=0;
				West_count = 0;
			}
			if(West_status==0){
				Create_Enemy(Num_Enemies, LEFT ,8, 96);
				Num_Enemies++;
				West_status = 1;
			}
		}
	}
}

void Enemy_Track(int i, int x, int y){
	int calc_x;
	int calc_y;
	int dont_move = 0;
	calc_x=(Player.xpos-x);
	calc_y=(Player.ypos-y);
	if(calc_x>0){
		if(i != 0){
			for(int k = 0; k < i; k++){
				if(enemy_collision(&Enemies[k], &Enemies[i]) == 1){
					dont_move = 1;
				}
			}
			if(i < 9){
				for(int a = (i+1); a < 10; a++){
					if(enemy_collision(&Enemies[a], &Enemies[i]) == 1){
						dont_move = 1;
					}
				}
			}
			if(i == 9){
				if(enemy_collision(&Enemies[10], &Enemies[i]) == 1){
					dont_move = 1;
				}
			}
		}
		else{
			for(int b = 1; b < 10; b++){
				if(enemy_collision(&Enemies[b], &Enemies[i]) == 1){
					dont_move = 1;
				}
			}
		}
		if(dont_move == 0){
			Enemies[i].xpos++;
		}
		else{
			dont_move = 0;
		}
	}
	if(calc_y>0){
		if(i != 0){
			for(int k = 0; k < i; k++){
				if(enemy_collision(&Enemies[k], &Enemies[i]) == 1){
					dont_move = 1;
				}
			}
			if(i < 0){
				for(int a = (i+1); a < 10; a++){
					if(enemy_collision(&Enemies[a], &Enemies[i]) == 1){
						dont_move = 1;
					}
				}
			}
			if(i == 9){
				if(enemy_collision(&Enemies[10], &Enemies[i]) == 1){
					dont_move = 1;
				}
			}
		}
		else{
			for(int b = 1; b < 10; b++){
				if(enemy_collision(&Enemies[b], &Enemies[i]) == 1){
					dont_move = 1;
				}
			}
		}
		if(dont_move == 0){
			Enemies[i].ypos++;
		}
		else{
			dont_move = 0;
		}
	}
	if(calc_x<0){
		if(i != 0){
			for(int k = 0; k < i; k++){
				if(enemy_collision(&Enemies[k], &Enemies[i]) == 1){
					dont_move  = 1;
				}
			}
			if(i < 9){
				for(int a = (i+1); a < 10; a++){
					if(enemy_collision(&Enemies[a], &Enemies[i]) == 1){
						dont_move = 1;
					}
				}
			}
			if(i == 9){
				if(enemy_collision(&Enemies[10], &Enemies[i]) == 1){
					dont_move = 1;
				}
			}
		}
		else{
			for(int b = 1; b < 10; b++){
				if(enemy_collision(&Enemies[b], &Enemies[i]) == 1){
					dont_move = 1;
				}
			}
		}
		if(dont_move == 0){
			Enemies[i].xpos--;
		}
		else{
			dont_move = 0;
		}
	}
	if(calc_y<0){
		if(i != 0){
			for(int k = 0; k < i; k++){
				if(enemy_collision(&Enemies[k], &Enemies[i]) == 1){
					dont_move = 1;
				}
			}
			if(i < 9){
				for(int a = (i+1); a < 10; a++){
					if(enemy_collision(&Enemies[a], &Enemies[i]) == 1){
						dont_move = 1;
					}
				}
			}
			if(i == 9){
				if(enemy_collision(&Enemies[10], &Enemies[i]) == 1){
					dont_move = 1;
				}
			}
		}
		else{
			for(int b = 1; b < 10; b++){
				if(enemy_collision(&Enemies[b], &Enemies[i]) == 1){
					dont_move = 1;
				}
			}
		}
		if(dont_move == 0){
			Enemies[i].ypos--;
		}
		else{
			dont_move = 0;
		}
	}
}
void Move_Enemies (void){
	for(int i=0; i<10; i++){
		if((Enemies[i].alive)==1){
			Enemy_Track(i, Enemies[i].xpos , Enemies[i].ypos);
			walk = (walk+1)%2;
			ST7735_DrawBitmap(Enemies[i].xpos, Enemies[i].ypos, Enemies[i].images[walk], Enemies[i].width, Enemies[i].height);
		}
	}
}
uint8_t Input_PlayerMove(void){
	//Move[0] - Horizontal , >E00 is right or up, <550 is left or down
	//Move[1] - Vertical
	while(ADCStatus == 0){};
	ADCStatus = 0;
	if(Move[0] > 0xE00){
		if(Move[1] < 0x550){
			return DOWNRIGHT;
		}
		if(Move[1] > 0xE00){
			return UPRIGHT;
		}
		else{
			return RIGHT;
		}
	}
	if(Move[0] < 0x550 ){
		if(Move[1] < 0x550){
			return DOWNLEFT;
		}
		if(Move[1] > 0xE00){
			return UPLEFT;
		}
		else{
			return LEFT;
		}
	}
	if(Move[1] < 0x550){
		if(Move[0] > 0xE00){
			return DOWNRIGHT;
		}
		if(Move[0] < 0x550){
			return DOWNLEFT;
		}
		else{
			return DOWN;
		}
	}
	if(Move[1] > 0xE00){
		if(Move[0] > 0xE00){
			return UPRIGHT;
		}
		if(Move[0] < 0x550){
			return UPLEFT;
		}
		else{
			return UP;
		}
	}
	else{
		return STAY;
	}
}
void Input_Joystick(void){
	ADC_In(Move);
	ADCStatus = 1;
}

void PlayerMove(void){
	int move = Input_PlayerMove();
	if( move == UP){
		if( Player.ypos <= 150 && Player.ypos > 34){
			Player.ypos -= speed_n;
		}
		if(Player.ypos > 150){
			Player.ypos = 150;
		}
		if(Player.ypos <= 34){
			Player.ypos = 35;
		}
	}
	if( move == DOWN){
		if( Player.ypos < 150 && Player.ypos >= 34){
			Player.ypos += speed_n;
		}
		if(Player.ypos >= 150){
			Player.ypos = 149;
		}
		if(Player.ypos < 34){
			Player.ypos = 34;
		}
	}
	if( move == RIGHT){
		if( Player.xpos >= 8 && Player.xpos < 96){
			Player.xpos += speed_n;
		}
		if(Player.xpos >= 96){
			Player.xpos = 95;
		}
		if(Player.xpos < 8){
			Player.xpos = 8;
		}
	}
	if( move == LEFT){
		if( Player.xpos > 8 && Player.xpos <= 96){
			Player.xpos -= speed_n;
		}
		if(Player.xpos > 96){
			Player.xpos = 96;
		}
		if(Player.xpos <= 8){
			Player.xpos = 9;
		}
	}
	if( move == UPLEFT){
		if( Player.xpos > 8 && Player.xpos <= 96){
			Player.xpos -= speed_n;
		}
		if(Player.xpos > 96){
			Player.xpos = 96;
		}
		if(Player.xpos <= 8){
			Player.xpos = 9;
		}
		if( Player.ypos <= 150 && Player.ypos > 34){
			Player.ypos -= speed_n;
		}
		if(Player.ypos > 150){
			Player.ypos = 150;
		}
		if(Player.ypos <= 34){
			Player.ypos = 35;
		}
	}
	if( move == DOWNLEFT){
		if( Player.xpos > 8 && Player.xpos <= 96){
			Player.xpos -= speed_n;
		}
		if(Player.xpos > 96){
			Player.xpos = 96;
		}
		if(Player.xpos <= 8){
			Player.xpos = 9;
		}
		if( Player.ypos < 150 && Player.ypos >= 34){
			Player.ypos += speed_n;
		}
		if(Player.ypos >= 150){
			Player.ypos = 149;
		}
		if(Player.ypos < 34){
			Player.ypos = 34;
		}
	}
	if( move == UPRIGHT){
		if( Player.xpos >= 8 && Player.xpos < 96){
			Player.xpos += speed_n;
		}
		if(Player.xpos >= 96){
			Player.xpos = 95;
		}
		if(Player.xpos < 8){
			Player.xpos = 8;
		}
		if( Player.ypos <= 150 && Player.ypos > 34){
			Player.ypos -= speed_n;
		}
		if(Player.ypos > 150){
			Player.ypos = 150;
		}
		if(Player.ypos <= 34){
			Player.ypos = 35;
		}
	}
	if( move == DOWNRIGHT){
		if( Player.xpos >= 8 && Player.xpos < 96){
			Player.xpos += speed_n;
		}
		if(Player.xpos >= 96){
			Player.xpos = 95;
		}
		if(Player.xpos < 8){
			Player.xpos = 8;
		}
		if( Player.ypos < 150 && Player.ypos >= 34){
			Player.ypos += speed_n;
		}
		if(Player.ypos >= 150){
			Player.ypos = 149;
		}
		if(Player.ypos < 34){
			Player.ypos = 34;
		}
	}
}
void scoreboard_init(void){
	ST7735_SetCursor(0,0);
	ST7735_OutString("Killed:");
	ST7735_SetCursor(hundreds,0);
	ST7735_OutChar(0x30);
	ST7735_SetCursor(tens,0);
	ST7735_OutChar(0x30);
	ST7735_SetCursor(ones,0);
	ST7735_OutChar(0x30);
}

void scoreboard_update(int killed){
	ST7735_SetCursor(0,0);
	ST7735_OutString("Killed:");
	hundreds=(killed/100);
	ST7735_SetCursor(35,0);
	ST7735_OutChar((0x30+hundreds));
	tens=((killed%100)/10);
	ST7735_SetCursor(40,0);
	ST7735_OutChar((0x30)+tens);
	ones=(killed%10);
	ST7735_SetCursor(45,0);
	ST7735_OutChar((0x30)+ones);
}

int main(void){
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Output_Init();						// Initializes screen
//	Sound_Init();						 // Initializes DAC and sets up sound struct
	ADC_Init(); 							// Initializes Joystick
	PortD_Init();							// Initializes the 4 shoot buttons
	Random_Init(1);						// Initializes random
	Random_Init(NVIC_ST_CURRENT_R );  // Random seed
	Timer1_Init(&Input_Joystick, 8000000);		//Grabs analog data from joystick every 1ms
	char_init();							// Creates Player object
	bullet_init();						// Defines Bullet object array
	enemy_init();							// Defines Enemy object array
	
	ST7735_DrawBitmap(0,160,StartMenu,128,160); // StartMenu
	while((GPIO_PORTD_DATA_R&0x0F) == 0){
	}
	ST7735_FillScreen(0); 
	ST7735_DrawBitmap(56,93,three,16,26); // StartMenu
	Delay100ms(10);
	ST7735_DrawBitmap(56,93,two,16,26);
	Delay100ms(10);
	ST7735_DrawBitmap(56,93,one,16,26); // StartMenu
	Delay100ms(10);
	ST7735_DrawBitmap(0,160,Map,128,160);
	scoreboard_init();
	check = 0;
	Delay100ms(30);
	
	
	while(1){
		scoreboard_update(killed);
		Spawn_Enemies(); // Creates the 10 Enemy objects
		Move_Enemies();  // Moves then prints enemy to screen
		Check_Gun_Buttons(); // Checks if button is pressed, if so, instantiate bullet object
		Move_Bullets();	// Move and print bullet
		
		/*************************COLLISIONS**************************************/
		for(int i = 0; i < 10; i++){	
			if(Enemies[i].alive == 1){ // Checks if Player has been collided with Enemy
				if(enemy_collision(&Player, &Enemies[i]) == 1){
					Player.alive = 0;
				}
				if(Player.alive == 0){
					Player.ypos = Player.ypos - Player.height;
					Delay100ms(5);
					LCD_RemoveChar(&Player);
					Delay100ms(5);
					while(1){
						ST7735_OutString("You Lose! ");
					}
				}
			}
			for(int j = 0; j < 10; j++){
				if(Bullets[j].alive == 1){	// Checks if Enemy has been collided with a bullet
					if(Enemies[i].alive ==1){
						if(bullet_collision(&Enemies[i], &Bullets[j]) == 1){
							Enemies[i].alive = 0;
							Enemies[i].destroy = 1;
							Bullets[j].alive = 0;
							Bullets[j].destroy = 1;
						}
					}
				}
				if(Enemies[i].destroy == 1){
					Enemies[i].ypos = Enemies[i].ypos - Enemies[i].height;
					LCD_RemoveChar(&Enemies[i]);
					Enemies[i].destroy = 0;
					killed++;
				}
				if(Bullets[j].destroy == 1){
					Bullets[j].ypos = Bullets[j].ypos - Bullets[j].height;
					LCD_RemoveBullet(&Bullets[j]);
					Bullets[j].destroy = 0;
				}
			}
		}
		PlayerMove();
		if(Player.alive == 1){
			Print_Char(&Player);
		}
		if(killed == 8){
			ST7735_FillScreen(0); 
			while(1){
				ST7735_DrawBitmap(0,160,WinScreenBright,128,160);
				Delay100ms(10);
				ST7735_DrawBitmap(0,160,WinScreenDim,128,160);
				Delay100ms(10);
			}
		}
	}
}


// You can use this timer only if you learn how it works
void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
