//------------------------------------------- main.c CODE STARTS ---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Draw2D.h"
#include "picture.h"

#define TIMER0_COUNTS 10000
#define SYSTICK_DLAY_us 1000000
#define Xmax 128
#define Ymax 50
#define Xmin 1
#define Ymin 1
//------------------------------------------------------------------------------------------------------------------------------------
// Functions declaration
//------------------------------------------------------------------------------------------------------------------------------------
void System_Config(void);
void SPI3_Config(void);
void LCD_start(void);
void LCD_command(unsigned char temp);
void LCD_data(unsigned char temp);
void LCD_clear(void);
void LCD_SetAddress(uint8_t PageAddr, uint8_t ColumnAddr);
void KeyPadEnable(void);
uint8_t KeyPadScanning(void);

void print_TitleScreen_START(void);
void print_TitleScreen_HowToPlay(void);
void print_TitleScreen_SCORES(void);
void erase_TitleScreen(void);

void print_Difficuly_EASY(void);
void print_Difficuly_NORMAL(void);
void print_Difficuly_HARD(void);
void erase_Difficulty(void);

void print_backgroundline(void);
void erase_backgroundline(void);

//------------------------------------------------------------------------------------------------------------------------------------
// Main program
//------------------------------------------------------------------------------------------------------------------------------------
typedef enum 
{
	welcome_screen, title_screen, select_difficulty, 
	game_rules, game_background, main_game, end_game, highest_scores
} STATES;

typedef enum
{
	easy, normal, hard
} difficulty;

typedef struct _raindrop
	{
		uint8_t x;
		uint8_t	y;
		uint8_t prev_x;
		uint8_t	prev_y;
		uint8_t speed;
	} rain;

typedef struct _player
	{
		int8_t x;
		uint8_t	y;
		uint8_t dx;
		uint8_t dy;
		uint8_t speed;
		uint8_t x_steps;
		uint8_t y_steps;
		uint8_t	hit;
	} player;

int main(void)
	{
		difficulty difficulty_level = easy;
		STATES game_state;
		uint8_t title_choice = 0;
		uint8_t i = 0;
		uint8_t key_pressed = 0;
		uint8_t game_pad=0;
		uint8_t the_number_of_raindrops = 0;
		uint8_t the_number_of_raindrops_max = 0;
		
		uint16_t score = 0;
		uint16_t high_score = 0;
		uint16_t high_score_easy = 0;
		uint16_t high_score_normal = 0;
		uint16_t high_score_hard = 0;
		uint8_t new_record;
		char score_txt[]="0";
		char high_score_txt[]="0";
		char high_score_easy_txt[]="0";
		char high_score_normal_txt[]="0";
		char high_score_hard_txt[]="0";
		//--------------------------------
		//System initialization
		//--------------------------------
		System_Config();
		KeyPadEnable();
		//--------------------------------
		//SPI3 initialization
		//--------------------------------
		SPI3_Config();
		//--------------------------------
		//LCD initialization
		//--------------------------------
		LCD_start();
		LCD_clear();
		//--------------------------------
		//LCD static content
		//--------------------------------
		//--------------------------------
		//LCD dynamic content
		//--------------------------------
		
		// Declare the player and raindrops
		player player;
		rain raindrop[25];
		
		// Characteristics for player
		player.x = 64;
		player.y = 55;
		player.hit = 0;
		player.x_steps = 6;
		
		
		// Initialize the characteristics for raindrops
		for (int k = 0; k < 25; k++)
			{
				raindrop[k].x = (rand() % 124) + 2;
				raindrop[k].y = Ymin + 11;
				raindrop[k].prev_x = 0;
				raindrop[k].prev_y = 0;
				raindrop[k].speed = 1;			
			}

		game_state = welcome_screen;
		
		score = 0;
		high_score = 0;
		new_record = 0;

		
		while (1) 
			{
				switch(game_state)
					{
						case welcome_screen:
							//welcome state code here
							draw_Bmp32x32(45, 5, 1, 0, clouds32x32);	
							printS_5x7(15, 40, "Team Clouds presents");
							for (i=0;i<9;i++) CLK_SysTickDelay(SYSTICK_DLAY_us);
							draw_Bmp32x32(45, 5, 0, 0, clouds32x32);
							LCD_clear();
							game_state = title_screen; // state transition
							break;
						
						case title_screen:
							// Title screen code here
							
							draw_Bmp64x64(0, 0, 1, 0, title_screen64x64);	
							printS_5x7(82, 0, "Between");
							printS_5x7(90, 10, "The");
							printS_5x7(77, 20, "Raindrops");
							// Depending on the current choice differ the display

							if (title_choice == 0)
								{
									print_TitleScreen_START();
								}
							else if (title_choice == 1)
								{
									print_TitleScreen_HowToPlay();
								}
							else if (title_choice == 2)
								{
									print_TitleScreen_SCORES();					
								}
							CLK_SysTickDelay(SYSTICK_DLAY_us*2);
							
							while(key_pressed==0)
								{ 
									key_pressed = KeyPadScanning();
								}
							
							CLK_SysTickDelay(300000);
							switch(key_pressed)
								{
									case 2:
										if ((title_choice == 0)||(title_choice == 1))
											{
												title_choice = 0;
												print_TitleScreen_START();
											}
										else
											{
												title_choice = 1;
												print_TitleScreen_HowToPlay();
											}		
										
										key_pressed = 0;
										game_state = title_screen;
										
										break;

									case 5:
										LCD_clear();
										if (title_choice == 0)
											{
												erase_TitleScreen();
												key_pressed = 0;
												game_state = select_difficulty;
											}
										else if (title_choice == 1)
											{
												erase_TitleScreen();
												key_pressed = 0;
												game_state = game_rules;
											}
										else if (title_choice == 2)
											{
												erase_TitleScreen();
												key_pressed = 0;
												game_state = highest_scores;
											}			
										else
											{
												erase_TitleScreen();
												key_pressed = 0;
												game_state = select_difficulty;
											}
										break;
										
									case 8:
										if (title_choice == 0)
										{
											print_TitleScreen_HowToPlay();
											title_choice = 1;
										}
										else
										{
											title_choice = 2;
											print_TitleScreen_SCORES();
										}
										
										key_pressed = 0;
										game_state = title_screen;
										
										break;
										
									default:
										key_pressed = 0;
										break;
								}
							break;
							
						case select_difficulty:
							// Difficulty choice screen code here
							printS_5x7(24, 15, "Choose the level");
							
							if (difficulty_level == easy)
								{
									print_Difficuly_EASY();
								}
							if (difficulty_level == normal)
								{
									print_Difficuly_NORMAL();

								}
							if (difficulty_level == hard)
								{
									print_Difficuly_HARD();
								}
								
							CLK_SysTickDelay(SYSTICK_DLAY_us*2);
								
							while(key_pressed == 0)
							{
								key_pressed = KeyPadScanning();
							}
							CLK_SysTickDelay(300000);
							switch(key_pressed)
								{								
									case 4:
										if ((difficulty_level == easy)||(difficulty_level == normal))
											{
												difficulty_level = easy;
												print_Difficuly_EASY();
											}
										else
											{
												difficulty_level = normal;
												print_Difficuly_NORMAL();
											}		
										
										key_pressed = 0;
										game_state = select_difficulty;
										break;
									case 5:
										LCD_clear();
										erase_Difficulty();
										game_state = game_background;
										break;
									case 6:
										if ((difficulty_level == normal)||(difficulty_level == hard))
											{
												difficulty_level = hard;
												print_Difficuly_HARD();
											}
										else
											{
												difficulty_level = normal;
												print_Difficuly_NORMAL();
											}	
										game_state = select_difficulty;
										key_pressed = 0;
										break;
											
									default:
										key_pressed = 0;
										break;
								}	
								
							break;
							
						case game_rules:
							// game_rules state code here
							printS_5x7(10, 1, "Use Keypad to control");
							printS_5x7(10, 20, "4: LEFT      6: RIGHT");
							printS_5x7(4, 38, "Avoid all the raindrops!");
							printS_5x7(1, 49, "Press any key to go back!");
							while(key_pressed==0)								
								{
									key_pressed = KeyPadScanning();
								}
							CLK_SysTickDelay(SYSTICK_DLAY_us);
							key_pressed = 0;
							LCD_clear();
							game_state = title_screen;
							break;
						
						case highest_scores:
							// Print out the highest scores for each mode
							printS_5x7(20, 1, "The highest scores");
							sprintf(high_score_easy_txt, "%d", high_score_easy);
							sprintf(high_score_normal_txt, "%d", high_score_normal);
							sprintf(high_score_hard_txt, "%d", high_score_hard);
							printS_5x7(30, 20, "EASY ");
							printS_5x7(70, 20, high_score_easy_txt);
							printS_5x7(30, 38, "NORMAL ");
							printS_5x7(70, 38, high_score_normal_txt);
							printS_5x7(30, 49, "HARD ");
							printS_5x7(70, 49, high_score_hard_txt);
							
							while(key_pressed==0)
								{
									key_pressed = KeyPadScanning();
								}
							CLK_SysTickDelay(SYSTICK_DLAY_us);
							key_pressed = 0;
							LCD_clear();
							game_state = title_screen;
							break;								
						
						case game_background:
							// static display information should be here
							printS_5x7(1, 0, "SCORE ");
							sprintf(score_txt, "%d", score);
							printS_5x7(31, 0, score_txt);
						
							// Record the highest score
							if (difficulty_level == easy)
								{
									high_score = high_score_easy;
									if (score >= high_score_easy)
										{
											high_score_easy = score;
										}
								}
							else if (difficulty_level == normal)
								{
									high_score = high_score_normal;
									if (score >= high_score_normal)
										{
											high_score_normal = score;
										}										
								}
							else 
								{
									high_score = high_score_hard;
									if (score >= high_score_hard)
									{
										high_score_hard = score;
									}												
								}
							
							printS_5x7(68, 0, "HIGH ");
							sprintf(high_score_txt, "%d", high_score);
							printS_5x7(94, 0, high_score_txt);
							print_backgroundline();
							game_state = main_game;
							break;
						
						case main_game:
							// ------------------------ RAINDROP Setting begins -------------------------------
							the_number_of_raindrops++;
							if (difficulty_level == easy)
								{
									the_number_of_raindrops_max = 10;
								}
							else if (difficulty_level == normal)
								{
									the_number_of_raindrops_max = 15;
								}
							else
								{
									the_number_of_raindrops_max = 20;
								}
							
							for (int k = 0; k < the_number_of_raindrops; k++)
							{
								if (the_number_of_raindrops >= the_number_of_raindrops_max)
								{
									the_number_of_raindrops = the_number_of_raindrops_max;
								}
								raindrop[k].speed = 1;
								if (raindrop[k].y <= 60)
									{
										raindrop[k].prev_x = raindrop[k].x;
										raindrop[k].prev_y = raindrop[k].y;
										if ((raindrop[k].y >= 5))
											{
												draw_Line(raindrop[k].prev_x, raindrop[k].prev_y, raindrop[k].x, raindrop[k].prev_y-3, 0, 0);
												raindrop[k].speed = 2;
											}
										if (raindrop[k].y >= 10)
											{
												draw_Line(raindrop[k].prev_x, raindrop[k].prev_y, raindrop[k].x, raindrop[k].prev_y-3, 0, 0);
												raindrop[k].speed = 4;
											}
										if (raindrop[k].y >= 25)
											{
												draw_Line(raindrop[k].prev_x, raindrop[k].prev_y, raindrop[k].x, raindrop[k].prev_y-3, 0, 0);
												raindrop[k].speed = 6;
											}
										if (raindrop[k].y >= 40)
											{
												draw_Line(raindrop[k].prev_x, raindrop[k].prev_y, raindrop[k].x, raindrop[k].prev_y-3, 0, 0);
												raindrop[k].speed = 9;
											}
										
										raindrop[k].y = raindrop[k].y + raindrop[k].speed;
										draw_Line(raindrop[k].x, raindrop[k].y, raindrop[k].x, raindrop[k].y-3, 1, 0);
									}
								if (raindrop[k].y > 60)
									{
										draw_Line(raindrop[k].x, raindrop[k].y, raindrop[k].x, raindrop[k].y-3, 0, 0);
										raindrop[k].y = Ymin + 11;
										raindrop[k].x = (rand() % 124) + 2;
										score++;
									}							
							}

							
							// ------------------------ PLAYER Setting begins --------------------------------
							//draw objects
							draw_Bmp8x8(player.x, player.y, 1, 0, player_8x8);
							//delay for vision
							game_pad= KeyPadScanning();
							CLK_SysTickDelay(170000);
							//erase the objects
							draw_Bmp8x8(player.x, player.y, 0, 0, player_8x8);
							
							//check changes
							switch(game_pad) 
								{
									case 4: player.dx = -1; player.dy = 0; break;
									case 6: player.dx = +1; player.dy = 0; break;
									default: player.dx = player.dy = 0; break;
								}
							game_pad = 0;
							//update objects information (position, etc.)
							player.x = player.x + (player.dx * player.x_steps);
							//boundary condition
							//wrap around on X
							// Limit the range of the player movement beyond the LCD
							if (player.x <= Xmin+3) 
								{
									player.x = 2;
								}
							if (player.x > Xmax-7) 
								{
									player.x = Xmax-9;
								}
						
							
							// Detect whether the player is hit
							for (int k = 0; k < 15; k++)
								{
									if ((player.x <= raindrop[k].x)	&& (raindrop[k].x <= (player.x + 8)) && (player.y <= (raindrop[k].y)))
										{
											player.hit = player.hit + 1;
										}
								}

							
							
							// ------------------------ PLAYER Setting ends --------------------------------
							
					

							if (player.hit >= 1) 
								{
									LCD_clear();
									game_state = end_game;
								}
							else
								{
									game_state = game_background;
								}
							break;
							
							case end_game:
								//end_game code here
								key_pressed=0;
								printS_5x7(40, 10, "GAME OVER");
								printS_5x7(25, 30, "Your score: ");
								printS_5x7(82, 30, score_txt);
								printS_5x7(5, 50, "Press any key to replay!");
								for (i=0;i<2;i++) CLK_SysTickDelay(SYSTICK_DLAY_us);
								while(key_pressed==0) key_pressed = KeyPadScanning();
								key_pressed=0;
								
								LCD_clear();		
								// Reset the player characeter
								player.x = 64;	player.y = 55; // Reset player location
								player.hit = 0; // Reset the number of player hit
								the_number_of_raindrops = 0;
								
								// Initialize the characteristics for raindrops
								for (int k = 0; k < 25; k++)
									{
										draw_Line(raindrop[k].x, raindrop[k].y, raindrop[k].x, raindrop[k].y-3, 0, 0);
										draw_Line(raindrop[k].prev_x, raindrop[k].prev_y, raindrop[k].x, raindrop[k].prev_y-3, 0, 0);
										draw_Line(raindrop[k].prev_x, raindrop[k].y, raindrop[k].x, raindrop[k].y-3, 0, 0);
										raindrop[k].y = Ymin + 11;
									}
								for (int k = 0; k < 25; k++)
									{							
										raindrop[k].x = (rand() % 124) + 2;
									}
								
								// Initialize the scores
								score = 0;
								high_score = 0;
									
								// Erase the background for clean title screen
								erase_backgroundline();
								title_choice = 0;
								game_state = title_screen;
								break;
							
							default:
								game_state = welcome_screen;
							break;
						}
					}
	}
//------------------------------------------------------------------------------------------------------------------------------------
// Functions definition
//------------------------------------------------------------------------------------------------------------------------------------
void LCD_start(void)
	{
		LCD_command(0xE2); // Set system reset
		LCD_command(0xA1); // Set Frame rate 100 fps
		LCD_command(0xEB); // Set LCD bias ratio E8~EB for 6~9 (min~max)
		LCD_command(0x81); // Set V BIAS potentiometer
		LCD_command(0xA0); // Set V BIAS potentiometer: A0 ()
		LCD_command(0xC0);
		LCD_command(0xAF); // Set Display Enable
	}
	
void LCD_command(unsigned char temp)
	{
		SPI3->SSR |= 1ul << 0;
		SPI3->TX[0] = temp;
		SPI3->CNTRL |= 1ul << 0;
		while(SPI3->CNTRL & (1ul << 0));
		SPI3->SSR &= ~(1ul << 0);
	}

void LCD_data(unsigned char temp)
	{
		SPI3->SSR |= 1ul << 0;
		SPI3->TX[0] = 0x0100+temp;
		SPI3->CNTRL |= 1ul << 0;
		while(SPI3->CNTRL & (1ul << 0));
		SPI3->SSR &= ~(1ul << 0);
	}
	
void LCD_clear(void)
	{
		int16_t i;
		LCD_SetAddress(0x0, 0x0);
		for (i = 0; i < 132 *8; i++)
			{
				LCD_data(0x00);
			}
	}
void LCD_SetAddress(uint8_t PageAddr, uint8_t ColumnAddr)
	{
		LCD_command(0xB0 | PageAddr);
		LCD_command(0x10 | (ColumnAddr>>4)&0xF);
		LCD_command(0x00 | (ColumnAddr & 0xF));
	}

void KeyPadEnable(void)
	{
		GPIO_SetMode(PA, BIT0, GPIO_MODE_QUASI);
		GPIO_SetMode(PA, BIT1, GPIO_MODE_QUASI);
		GPIO_SetMode(PA, BIT2, GPIO_MODE_QUASI);
		GPIO_SetMode(PA, BIT3, GPIO_MODE_QUASI);
		GPIO_SetMode(PA, BIT4, GPIO_MODE_QUASI);
		GPIO_SetMode(PA, BIT5, GPIO_MODE_QUASI);
	}

uint8_t KeyPadScanning(void)
	{
		PA0=1; PA1=1; PA2=0; PA3=1; PA4=1; PA5=1;
		if (PA3==0) return 1;
		if (PA4==0) return 4;
		if (PA5==0) return 7;
		PA0=1; PA1=0; PA2=1; PA3=1; PA4=1; PA5=1;
		if (PA3==0) return 2;
		if (PA4==0) return 5;
		if (PA5==0) return 8;
		PA0=0; PA1=1; PA2=1; PA3=1; PA4=1; PA5=1;
		if (PA3==0) return 3;
		if (PA4==0) return 6;
		if (PA5==0) return 9;
		return 0;
	}

void System_Config (void)
	{
		SYS_UnlockReg(); // Unlock protected registers
		// Enable clock sources
		//LXT - External Low frequency Crystal 32 kHz
		CLK->PWRCON |= (0x01ul << 1);
		while(!(CLK->CLKSTATUS & (1ul << 1)));
		CLK->PWRCON |= (0x01ul << 0);
		while(!(CLK->CLKSTATUS & (1ul << 0)));
		//PLL configuration starts
		CLK->PLLCON &= ~(1ul<<19); //0: PLL input is HXT
		CLK->PLLCON &= ~(1ul<<16); //PLL in normal mode
		CLK->PLLCON &= (~(0x01FFul << 0));
		CLK->PLLCON |= 48;
		CLK->PLLCON &= ~(1ul<<18); //0: enable PLLOUT
		//clock source selection
		CLK->CLKSEL0 &= (~(0x07ul << 0));
		CLK->CLKSEL0 |= (0x02ul << 0);
		TIMER0->TCMPR = TIMER0_COUNTS;
		while(!(CLK->CLKSTATUS & (0x01ul << 2)));
		//PLL configuration ends
		
			
		//clock frequency division
		CLK->CLKDIV &= (~0x0Ful << 0);
		
		//enable clock of SPI3
		CLK->APBCLK |= 1ul << 15;
		
		// ------------------------
		// Configure tm0
		CLK->CLKSEL1 &= ~(7ul << 8);
		CLK->CLKSEL1 |= 2ul << 8;
		CLK->APBCLK |= 1ul << 2;
		TIMER0->TCSR &= ~(0xFFul << 0);
		TIMER0->TCSR |= 49ul << 0; // prescale = 49
		TIMER0->TCSR |= 1ul << 26; // Reset counter
		
		// continuous
		TIMER0->TCSR &= ~(3ul << 27);
		TIMER0->TCSR |= (1ul << 27);
		TIMER0->TCSR &= ~(1ul << 24);
		TIMER0->TCSR |= (1ul << 16);

		//clock source selection
		CLK->CLKSEL0 &= (~(0x07ul << 0));
		CLK->CLKSEL0 |= (0x02ul << 0);
		TIMER0->TCMPR = TIMER0_COUNTS;
		// --------------------------


		SYS_LockReg(); // Lock protected registers
	}
void SPI3_Config (void)
	{
		SYS->GPD_MFP |= 1ul << 11; //1: PD11 is configured for alternative function
		SYS->GPD_MFP |= 1ul << 9; //1: PD9 is configured for alternative function
		SYS->GPD_MFP |= 1ul << 8; //1: PD8 is configured for alternative function
		SPI3->CNTRL &= ~(1ul << 23); //0: disable variable clock feature
		SPI3->CNTRL &= ~(1ul << 22); //0: disable two bits transfer mode
		SPI3->CNTRL &= ~(1ul << 18); //0: select Master mode
		SPI3->CNTRL &= ~(1ul << 17); //0: disable SPI interrupt
		SPI3->CNTRL |= 1ul << 11; //1: SPI clock idle high
		SPI3->CNTRL &= ~(1ul << 10); //0: MSB is sent first
		SPI3->CNTRL &= ~(3ul << 8); //00: one transmit/receive word will be executed in one data transfer
		SPI3->CNTRL &= ~(31ul << 3); //Transmit/Receive bit length
		SPI3->CNTRL |= 9ul << 3; //9: 9 bits transmitted/received per data transfer
		SPI3->CNTRL |= (1ul << 2); //1: Transmit at negative edge of SPI CLK
		SPI3->DIVIDER = 0; // SPI clock divider. SPI clock = HCLK / ((DIVIDER+1)*2). HCLK = 50 MHz
	}


void print_TitleScreen_START()
	{
		draw_Bmp8x8(95, 30, 0, 0, arrow_up_8x8);
		draw_Bmp8x8(95, 50, 1, 0, arrow_down_8x8);
		printS_5x7(67, 42, "               ");
		printS_5x7(87, 42, "START");
		return;
	}

void print_TitleScreen_HowToPlay()
	{
		draw_Bmp8x8(95, 30, 1, 0, arrow_up_8x8);
		draw_Bmp8x8(95, 50, 1, 0, arrow_down_8x8);
		printS_5x7(67, 42, "               ");
		printS_5x7(72, 42, "HOW TO PLAY");			
		return;
	}

void print_TitleScreen_SCORES()
	{
		draw_Bmp8x8(95, 30, 1, 0, arrow_up_8x8);
		draw_Bmp8x8(95, 50, 0, 0, arrow_down_8x8);
		printS_5x7(67, 42, "               ");
		printS_5x7(85, 42, "SCORES");	
		return;
	}

void erase_TitleScreen()
	{
		draw_Bmp64x64(0, 0, 0, 0, title_screen64x64);
		draw_Bmp8x8(95, 30, 0, 0, arrow_up_8x8);
		draw_Bmp8x8(95, 50, 0, 0, arrow_down_8x8);
		LCD_clear();
		return;
	}

void print_Difficuly_EASY()
	{
		draw_Bmp8x8(20, 31, 0, 0, arrow_left_8x8);
		draw_Bmp8x8(100, 31, 1, 0, arrow_right_8x8);
		printS_5x7(50, 35, "       ");
		printS_5x7(53, 35, "EASY");
	}
	
void print_Difficuly_NORMAL()
	{
		draw_Bmp8x8(20, 31, 1, 0, arrow_left_8x8);
		draw_Bmp8x8(100, 31, 1, 0, arrow_right_8x8);
		printS_5x7(50, 35, "       ");
		printS_5x7(50, 35, "NORMAL");
	}
	
void print_Difficuly_HARD()
	{
		draw_Bmp8x8(20, 31, 1, 0, arrow_left_8x8);
		draw_Bmp8x8(100, 31, 0, 0, arrow_right_8x8);
		printS_5x7(50, 35, "       ");
		printS_5x7(53, 35, "HARD");
	}

void erase_Difficulty()
	{
		draw_Bmp8x8(20, 31, 0, 0, arrow_left_8x8);
		draw_Bmp8x8(100, 31, 0, 0, arrow_right_8x8);
		LCD_clear();
	}
	
void print_backgroundline()
	{
		draw_Line(1, 10, 127, 10, 1, 0);
		draw_Line(127, 10, 127, 63, 1, 0);
		draw_Line(1, 63, 127, 63, 1, 0);
		draw_Line(1, 10, 1, 63, 1, 0);
		return;
	}

void erase_backgroundline()
	{
		draw_Line(1, 10, 127, 10, 0, 0);
		draw_Line(127, 10, 127, 63, 0, 0);
		draw_Line(1, 63, 127, 63, 0, 0);
		draw_Line(1, 10, 1, 63, 0, 0);
		return;
	}


//------------------------------------------- main.c CODE ENDS ---------------------------------------------------------------------------