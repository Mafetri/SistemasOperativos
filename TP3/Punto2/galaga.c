#include "titlescreen.h"
#include "playerImage.h"
#include "enemy.h"
#include "boss.h"
#include "gameover.h"
#include "shoot.h"
#include <xinu.h>
#include <stdio.h> 
#include <stdlib.h>

extern unsigned char tecla_actual;
typedef unsigned short u16;

// ====== DEFINE ======
// Color
#define RGB(r, g, b) (r | (g << 5) | (b << 10))
// REG_DISPCNT *(u16 *)0x4000000
#define extern videoBuffer
#define MODE3 3
#define BG2_ENABLE (1 << 10)
#define WHITE RGB(31, 31, 31)
#define BLACK RGB(0, 0, 0)
// Buttons *(volatile unsigned int *)0x4000130
#define BUTTON_A	0x24
#define BUTTON_B	0x25 
#define BUTTON_SELECT	0x03
#define BUTTON_START	0x2c
#define BUTTON_RIGHT	0x20 	// D
#define BUTTON_LEFT	0x1e		// A
#define BUTTON_UP	0x11		// W
#define BUTTON_DOWN	0x1f		// S
#define BUTTON_R	'1'
#define BUTTON_L	'2'
#define KEY_DOWN_NOW(key)  (tecla_actual == key)
// Variable definitions
#define playerspeed 2
#define enemyspeed 1
#define fastXSpeed 3
#define fastYSpeed 2
// Constants
#define MAX_LIVES 2
#define SHOT_SCORE 10
#define N_SHOTS 10

// ====== Objects ======
struct Players {
	volatile u16 playerX;
	volatile u16 playerY;
};
struct Enemy {
	volatile u16 enemyX;
	volatile u16 enemyY;
};
struct FastEnemy {
	volatile u16 fastX;
	volatile u16 fastY;
};

// ====== Methods ======
void setPixel(int x, int y, u16 color);
void drawRect(int x, int y, int width, int height, u16 color);
void drawHollowRect(int x, int y, int width, int height, u16 color);
void drawImage3(int x, int y, int width, int height, const u16* image);
void delay_galaga();
void waitForVBlank();
// Helpers
void initialize();
void drawEnemies();
void endGame();
int collision(u16 enemyX, u16 enemyY, u16 enemyWidth, u16 enemyHeight, u16 playerX, u16 playerY);
// Processes
int input(int);
int shipPosition(struct Players*, int*, int*);
int enemies(struct Enemy*, struct Enemy*, struct FastEnemy, int, int*);
int collisions(int, int, int, int, struct Players*, struct Enemy*, struct Enemy*, struct FastEnemy, int*);
int score(int, int, int*, int);

// ===== Semaphores =====
sid32 semFinish;
sid32 semPlayer;
sid32 semEnemies;
sid32 semShot;

// ===== Main =====
int galaga(void) {
	// ----- Set Up ----- 
	// Easy enemy wave
	struct Enemy easyEnemies[9];
	for (int a = 0; a < 9; a++) {
		easyEnemies[a].enemyX = (28*a);
		easyEnemies[a].enemyY = 0;
	} 
	easyEnemies[0].enemyX = 240;
	easyEnemies[4].enemyX = 240;
	easyEnemies[8].enemyX = 240;
	// Hard enemies
	struct Enemy hardEnemies[9];
	for (int a = 0; a < 9; a++) {
		hardEnemies[a].enemyX = (28*a);
		hardEnemies[a].enemyY = 160;
	} 
	hardEnemies[3].enemyX = 240;
	hardEnemies[6].enemyX = 240;
	// Fast enemy "boss"
	struct FastEnemy fast;
	fast.fastX = 0;
	fast.fastY = 30;
	// Player
	struct Players player;
	player.playerX = 120;
	player.playerY = 136;
	// Shots
	int shots[N_SHOTS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int curr_shot = 0;
	// Semaphores
	semFinish = semcreate(0);
	semShot = semcreate(1);
	semEnemies = semcreate(1);
	semPlayer = semcreate(1);

	// REG_DISPCNT = MODE3 | BG2_ENABLE;
	// Initalize title screen
	print_text_on_vga(10, 20, "GALAGA ");
	drawImage3(0, 0, 240, 160, titlescreen);
	while(1) {
		if (KEY_DOWN_NOW(BUTTON_START)) {
			break;
		}
	}	
	// Start black screen for drawing
	for (int i = 0; i < 240; i++) {
		for (int j = 0; j < 160; j++) {
			setPixel(i, j, BLACK);
		}
	}

	// ----- Processes -----
	// PIDs
	int pidInput;
	int pidShipPosition;
	int pidEnemies;
	int pidCollision;
	int pidScore;
	// Ship Position
	pidShipPosition = create(shipPosition, 1024, 20, "shipPosition", 3, &player, &shots, &curr_shot);
	// Input
	pidInput = create(input, 1024, 20, "input", 1, pidShipPosition);
	// Enemies
	pidEnemies = create(enemies, 1024, 20, "enemyPosition", 5, &easyEnemies, &hardEnemies, fast, pidCollision, &shots);
	// Finisher
	pidScore = create(score, 1024, 20, "finisher", 4, pidInput, pidEnemies, &pidCollision, pidShipPosition);
	// Collision
	pidCollision = create(collisions, 1024, 20, "collisions", 9, pidEnemies, pidShipPosition, pidInput, pidScore, &player, &easyEnemies, &hardEnemies, fast, &shots);

	resume(pidShipPosition);
	resume(pidInput);
	resume(pidEnemies);
	resume(pidCollision);
	resume(pidScore);

	wait(semFinish);
	
	return 0;
}

// ===== Input Process =====
int input(int pidShipPosition){
	while(1){
		//Go back to title screen if select button is pressed
		if (KEY_DOWN_NOW(BUTTON_SELECT)) {
			send(pidShipPosition, BUTTON_SELECT);
		}

		//Player shots 
		if (KEY_DOWN_NOW(BUTTON_A)) {
			send(pidShipPosition, BUTTON_A);
		}

		//Player movement input
		if (KEY_DOWN_NOW(BUTTON_LEFT)) {
			send(pidShipPosition, BUTTON_LEFT);
		}
		if (KEY_DOWN_NOW(BUTTON_RIGHT)) {
			send(pidShipPosition, BUTTON_RIGHT);
		}
		if (KEY_DOWN_NOW(BUTTON_UP)) {
			send(pidShipPosition, BUTTON_UP);
		}
		if (KEY_DOWN_NOW(BUTTON_DOWN)) {
			send(pidShipPosition, BUTTON_DOWN);
		}

		// Waits for VBlank and extra time between inputs
		waitForVBlank();
		sleepms(50);
	}
	return 0;
}

// ===== Ship Position Process ======
int shipPosition(struct Players *player, int *shots, int *curr_shot){
	while(1){
		//Draw player
		drawImage3(player->playerX, player->playerY, 24, 24, playerImage);
		drawHollowRect(player->playerX - 1, player->playerY - 1, 26, 26, BLACK);
		drawHollowRect(player->playerX - 2, player->playerY - 2, 28, 28, BLACK);

		// Waits for input message
		int msg = receive();

		//Go back to title screen if select button is pressed
		if (msg == BUTTON_SELECT) {
			//initialize();
			galaga();
		}

		wait(semShot);
		wait(semPlayer);
		//Shot 
		if (msg == BUTTON_A) {
			if (shots[*curr_shot] == 0) {
				shots[*curr_shot] = 136*240 + player->playerX+9; /* 24 widht player */
				*curr_shot++;
				if (*curr_shot >= N_SHOTS){
					*curr_shot = 0;
				}
			}
		}
		signal(semShot);
		
		//Move
		if (msg == BUTTON_LEFT && (player->playerX > 0)) {
			player->playerX -= playerspeed;
		}
		if (msg == BUTTON_RIGHT && (player->playerX <= 216)) {
			player->playerX += playerspeed;
		}
		if (msg == BUTTON_UP && (player->playerY > 25)) {
			player->playerY -= playerspeed;
		}
		if (msg == BUTTON_DOWN && (player->playerY <= 136)) {
			player->playerY += playerspeed;
		}

		signal(semPlayer);
	}
	return 0;
}

// ===== Enemies Position Process ======
int enemies(struct Enemy *easyEnemies, struct Enemy *hardEnemies, struct FastEnemy fast, int pidCollisions, int *shots){
	int enemyDestroyed = -1;
	while(1){
		// ----- Checks Collision -----
		// Shots
		enemyDestroyed = receive();

		wait(semEnemies);
		if(enemyDestroyed != -1){
			drawRect(easyEnemies[enemyDestroyed].enemyX, easyEnemies[enemyDestroyed].enemyY,  20, 20, BLACK);
			easyEnemies[enemyDestroyed].enemyY = 0;
			signal(semEnemies);
		}else{
			// ----- Draw Enemies -----
			// Easy enemies with downward movement
			for (int a = 0; a < 9; a++) {
				easyEnemies[a].enemyY += enemyspeed;
				drawImage3(easyEnemies[a].enemyX, easyEnemies[a].enemyY, 20, 20, enemy);
				if (easyEnemies[a].enemyY >= 160) {
					easyEnemies[a].enemyY = 0;
				}		
			}
			// Hard enemies
			for (int a = 0; a < 9; a++) {
				hardEnemies[a].enemyY += enemyspeed;
				drawImage3(hardEnemies[a].enemyX, hardEnemies[a].enemyY, 20, 20, enemy);
				if (hardEnemies[a].enemyY >= 228) {
					hardEnemies[a].enemyY = 0;
				}
				if ((hardEnemies[a].enemyY >= 200) && (easyEnemies[a].enemyY <=45)) {
					hardEnemies[a].enemyY = 160;
				}	
				//space enemies apart
				if ((hardEnemies[a].enemyY >= 200) && (easyEnemies[a].enemyY <=45)) {
					hardEnemies[a].enemyY = 160;
				}		
				if ((easyEnemies[a].enemyY >= 120) && (hardEnemies[a].enemyY >=170)) {
					hardEnemies[a].enemyY = 160;
				}							
			}	
			// Fast enemy
			drawImage3(fast.fastX, fast.fastY, 15, 15, boss);
			drawHollowRect(fast.fastX - 1, fast.fastY - 1, 17, 17, BLACK);
			drawHollowRect(fast.fastX - 2, fast.fastY - 2, 19, 19, BLACK);
			signal(semEnemies);

			wait(semShot);
			// ----- Draw Shots -----
			for (int i = 0; i < N_SHOTS; i++) {
				if (shots[i] != 0) {
					drawRect((shots[i] % 240), (shots[i] / 240)+4, 5, 5, BLACK);
					drawImage3((shots[i] % 240), (shots[i] / 240), 5, 5, shoot);
					shots[i] = shots[i] -240*4;
					if (shots[i] <= 0){
						shots[i] = 0;
					}
				}
			}
			signal(semShot);
		}
		waitForVBlank();
	}
	return 0;
}

// ===== Collisions Process ======
int collisions(int pidEnemies, int pidShipPosition, int pidPlayer, int pidScore, struct Players *player, struct Enemy *easyEnemies, struct Enemy *hardEnemies, struct FastEnemy fast, int *shots) {
	int shipDestroyed;
	while(1){
		// ------ Ships and shots -----
		shipDestroyed = -1;
		// Check each shot
		wait(semShot);
		for (int i = 0; i < N_SHOTS; i++) {
			// Checks one shot per enemy
			wait(semEnemies);
			for (int j = 0; j < 9; j++) {
				if (collision(easyEnemies[j].enemyX, easyEnemies[j].enemyY, 15, 15, shots[i] % 240, shots[i] / 240)) {
					shipDestroyed = j;
					// Delete the shot
					drawRect((shots[i] % 240), (shots[i] / 240)+4, 5, 5, BLACK);
					shots[i] = 0;
					send(pidScore, SHOT_SCORE);
				}
			}
			signal(semEnemies);
		}
		signal(semShot);
		// Warns enemies process that a ship may has been destroyed
		send(pidEnemies, shipDestroyed);

		// ----- Player Collision -----
		wait(semPlayer);
		wait(semEnemies);
		for (int a = 0; a < 9; a++) {
			if (collision(easyEnemies[a].enemyX, easyEnemies[a].enemyY, 20, 20, player->playerX, player->playerY)) {
				// Warns enemies that an enemy has been destroyed due a player collision
				send(pidEnemies, a);
				send(pidScore, -1);
			}
			if (collision(hardEnemies[a].enemyX, hardEnemies[a].enemyY, 20, 20, player->playerX, player->playerY)) {
				// Warns enemies that an enemy has been destroyed due a player collision
				send(pidEnemies, a);
				send(pidScore, -1);
			}
		}
		if(collision(fast.fastX, fast.fastY, 15, 15, player->playerX, player->playerY)) {
			send(pidScore, -(MAX_LIVES));
		}
		signal(semPlayer);
		signal(semEnemies);

		waitForVBlank();
	}
	return 0;
}

int collision(u16 enemyX, u16 enemyY, u16 enemyWidth, u16 enemyHeight, u16 playerX, u16 playerY) {
	//check if bottom right corner of enemy is within player
	if (((enemyX + enemyWidth) > playerX) && ( (enemyY + enemyHeight) 
		> playerY ) &&  ((enemyX + enemyWidth) < (playerX + 24)) 
		&& ((enemyY + enemyWidth) < (playerY + 24))) {
		return 1;
	} 
	//check bottom left corner of enemy
	if ( (enemyX < (playerX + 24)) 
		&& (enemyX > playerX)
		&& ((enemyY + enemyHeight) > playerY)
		&& ((enemyY + enemyHeight) < (playerY + 24))
		) {
		return 1;
	}
	//check top left corner
	if ( (enemyX < (playerX + 24)) 
		&& (enemyX > playerX)
		&& (enemyY > playerY)
		&& (enemyY < (playerY + 24))
		) {
		return 1;
	}	
	//check top right corner
	if ( ((enemyX + enemyWidth) < (playerX + 24)) 
		&& ((enemyX + enemyWidth) > playerX)
		&& (enemyY > playerY)
		&& (enemyY < (playerY + 24))
		) {
		return 1;
	}	
	return 0;
}

// ===== Score Process ======
int score(int pidInput, int pidEnemies, int *pidCollision, int pidShipPosition){
	int actualLives = MAX_LIVES, msg = 0, score = 0;

	while(actualLives > 0){
		// Waits for message
		msg = receive();

		// Below 0 for ship collision, over 0 for shot collision
		if(msg < 0){
			actualLives += msg;
		}else{
			score += msg;
		}		

		waitForVBlank();
	}

	// Kills everyone
	kill(pidShipPosition);
	kill(pidInput);
	kill(pidEnemies);
	kill(*pidCollision);

	// Tells main to continue to his own death
	signal(semFinish);

	// End game, if wants to continue this last process will create another galaga process and die
	endGame(getpid());

	return 0;
}

void endGame(int pid) {
	// Start Game Over State
	drawImage3(0, 0, 240, 160, gameover);
	drawHollowRect(0, 0, 240, 160, WHITE);
	while(1) {
		if (KEY_DOWN_NOW(BUTTON_SELECT) || KEY_DOWN_NOW(BUTTON_START)) {
			// If wants to start again, a new process of gallaga is created
			resume(create(galaga, 1024, 20, "galaga", 0));
			// And this last process dies
			break;
		} else {
			// Other key pressed, die
			break;
		}
	}
}
