
//#include "SpaceInvaders.c" 
//#include <stdint.h>
//#include "tm4c123gh6pm.h"
//int enemy_collision(Character_t *character, Character_t *character2){ //if character will collide with character2
//	int one_topleft_x = (character->xpos);
//	int one_topleft_y = (character->ypos) - (character->height) + 1;
//	int one_botright_x = (character->xpos) + (character->width) - 1;
//	int one_botright_y = (character->ypos);
//		
//	//same for 2nd char
//	int two_topright_x = (character2->xpos) + (character2->width) - 1;
//	int two_topright_y = (character2->ypos) - (character2->height) + 1 ;
//	int two_botright_x = (character2->xpos) + (character2->width) - 1;
//	int two_botright_y = (character2->ypos);
//	int two_topleft_x = (character2->xpos);
//	int two_topleft_y = (character2->ypos) - (character2->height) + 1;
//	int two_botleft_x = (character2->xpos);
//	int two_botleft_y = (character2->ypos);
//	
//	// if (topright_x is in between leftline and rightline) && (topright_y is in between topline and botomline)
//	if((one_topleft_x <= two_topright_x) && (two_topright_x <= one_botright_x) && (one_topleft_y <= two_topright_y) && (two_topright_y <= one_botright_y)){
//		return 1;
//	}
//	if((one_topleft_x <= two_botright_x) && (two_botright_x <= one_botright_x) && (one_topleft_y <= two_botright_y) && (two_botright_y <= one_botright_y)){
//		return 1;
//	}
//	if((one_topleft_x <= two_topleft_x) && (two_topleft_x <= one_botright_x) && (one_topleft_y <= two_topleft_y) && (two_topleft_y <= one_botright_y)){
//		return 1;
//	}
//	if((one_topleft_x <= two_botleft_x) && (two_botleft_x <= one_botright_x) && (one_topleft_y <= two_botleft_y) && (two_botleft_y <= one_botright_y)){
//		return 1;
//	}
//	else{
//		return 0;
//	}
//}

//int bullet_collision(Character_t *character, Bullet_t *character2){ //if character will collide with bullet
//	int leftline = (character->xpos); //left line. x = xpos
//	int rightline = (character->xpos) + (character->width) - 1; // right line. x = xpos + wide
//	int topline = (character->ypos) - (character->height) + 1; //  top line. y = ypos - height since low y value is higher on screen
//	int botline = (character->ypos);                           // y = ypos
//	
//	//same for 2nd char
//	int topright_y = (character2->ypos) - (character2->height) + 1 ;
//	int topright_x = (character2->xpos) + (character->width) - 1;
//	int botleft_y = (character2->ypos);
//	int botleft_x = (character2->xpos);
//	int topleft_y = (character2->ypos) - (character2->height) + 1;
//	int topleft_x = (character2->xpos);
//	int botright_y = (character2->ypos);
//	int botright_x = (character2->xpos) + (character->width) - 1;
//	
//	// if (topright_x is in between leftline and rightline) && (topright_y is in between topline and botomline)
//	if((leftline <= topright_x) && (topright_x <= rightline) && (topline <= topright_y) && (topright_y <= botline)){
//		return 1;
//	}
//	if((leftline <= botright_x) && (botright_x <= rightline) && (topline <= botright_y) && (botright_y <= botline)){
//		return 1;
//	}
//	if((leftline <= topleft_x) && (topleft_x <= rightline) && (topline <= topleft_y) && (topleft_y <= botline)){
//		return 1;
//	}
//	if((leftline <= botleft_x) && (botleft_x <= rightline) && (topline <= botleft_y) && (botleft_y <= botline)){
//		return 1;
//	}
//	else{
//		return 0;
//	}
//}
//int Player_Pickedup(Character_t *character, Power_t *Power){ //if character will collide with bullet
//	int leftline = (character->xpos); //left line. x = xpos
//	int rightline = (character->xpos) + (character->width) - 1; // right line. x = xpos + wide
//	int topline = (character->ypos) - (character->height) + 1; //  top line. y = ypos - height since low y value is higher on screen
//	int botline = (character->ypos);                           // y = ypos
//	
//	//same for 2nd char
//	int topright_y = (Power->ypos) - (Power->height) + 1 ;
//	int topright_x = (Power->xpos) + (Power->width) - 1;
//	int botleft_y = (Power->ypos);
//	int botleft_x = (Power->xpos);
//	int topleft_y = (Power->ypos) - (Power->height) + 1;
//	int topleft_x = (Power->xpos);
//	int botright_y = (Power->ypos);
//	int botright_x = (Power->xpos) + (Power->width) - 1;
//	
//	// if (topright_x is in between leftline and rightline) && (topright_y is in between topline and botomline)
//	if((leftline <= topright_x) && (topright_x <= rightline) && (topline <= topright_y) && (topright_y <= botline)){
//		return 1;
//	}
//	if((leftline <= botright_x) && (botright_x <= rightline) && (topline <= botright_y) && (botright_y <= botline)){
//		return 1;
//	}
//	if((leftline <= topleft_x) && (topleft_x <= rightline) && (topline <= topleft_y) && (topleft_y <= botline)){
//		return 1;
//	}
//	if((leftline <= botleft_x) && (botleft_x <= rightline) && (topline <= botleft_y) && (botleft_y <= botline)){
//		return 1;
//	}
//	else{
//		return 0;
//	}
//}
//int Enemy_Power_C(Character_t *Enemies, Power_t *Power){ //if character will collide with bullet
//	int leftline = (Enemies->xpos); //left line. x = xpos
//	int rightline = (Enemies->xpos) + (Enemies->width) - 1; // right line. x = xpos + wide
//	int topline = (Enemies->ypos) - (Enemies->height) + 1; //  top line. y = ypos - height since low y value is higher on screen
//	int botline = (Enemies->ypos);                           // y = ypos
//	
//	//same for 2nd char
//	int topright_y = (Power->ypos) - (Power->height) + 1 ;
//	int topright_x = (Power->xpos) + (Power->width) - 1;
//	int botleft_y = (Power->ypos);
//	int botleft_x = (Power->xpos);
//	int topleft_y = (Power->ypos) - (Power->height) + 1;
//	int topleft_x = (Power->xpos);
//	int botright_y = (Power->ypos);
//	int botright_x = (Power->xpos) + (Power->width) - 1;
//	
//	// if (topright_x is in between leftline and rightline) && (topright_y is in between topline and botomline)
//	if((leftline <= topright_x) && (topright_x <= rightline) && (topline <= topright_y) && (topright_y <= botline)){
//		return 1;
//	}
//	if((leftline <= botright_x) && (botright_x <= rightline) && (topline <= botright_y) && (botright_y <= botline)){
//		return 1;
//	}
//	if((leftline <= topleft_x) && (topleft_x <= rightline) && (topline <= topleft_y) && (topleft_y <= botline)){
//		return 1;
//	}
//	if((leftline <= botleft_x) && (botleft_x <= rightline) && (topline <= botleft_y) && (botleft_y <= botline)){
//		return 1;
//	}
//	else{
//		return 0;
//	}
//}
