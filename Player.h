


struct Character{
	int16_t xpos;    //Where to print on x-coordinate
	int16_t ypos;		// Where to print on y-coordinate
	const uint16_t *images[4];  //WHAT to print
	uint8_t direction; //What direction it is at.
	int16_t health;  //Keep track of health
	uint8_t alive;	//should be alive or not
	uint8_t width;  //how big the image is
	uint8_t height; // ^
};

struct Bullet{
	int16_t x;  
	int16_t y;
};

typedef struct Character Character_t;
