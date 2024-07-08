#define _CRT_SECURE_NO_WARNINGS
#include"stdio.h"
#include"CC212SGL.h"
#include"time.h"
#include"stdlib.h"
#include"conio.h"
#include"windows.h"
#include"MMsystem.h"
#pragma comment (lib,"CC212SGL")
#pragma comment(lib, "winmm.lib")
CC212SGL pvz;

struct Zombie {
	int Walking[7];
	int Splash[2];
	int Eating[6];
	int Dying[14];
};

//Function to load multiple images
void loadAndResizeImages(int Images_Num, char* Images_Names, int* Images_Ids, int Width, int Height) {

	char ID[100];

	for (int i = 0; i < Images_Num; i++) {
		sprintf(ID, "%s (%d).PNG", Images_Names, i + 1);
		Images_Ids[i] = pvz.loadImage(ID);
		pvz.resizeImage(Images_Ids[i], Width, Height);
	}
}

int main()
{
	//Makes console color green(2) and the text black(0)
	system("color 20");

	char Name[100];

	printf("Enter your name to check your completion time (the least time wins): ");
	gets_s(Name);

	//Appends in completion times file
	FILE* CompletionTime = fopen("Completion Times.txt", "a");

	pvz.setup();
	pvz.setFullScreenMode();
	pvz.hideCursor();
	pvz.setDrawingColor(pvz.generateFromRGB(98, 90, 196));
	PlaySound(TEXT("MainMenu.wav"), NULL, SND_ASYNC | SND_LOOP);

	//Variables for sun position, timer and counter
	int X_Sun = 500, Y_Sun = 0;
	int SunTimer = 0;
	int SunEnable = 0;
	int SunCounter = 0;

	//Variables for sun UI position
	int X_SunUI = 70;
	int Y_SunUI = 10;

	//Variables for plant UI position
	int X_PlantUI = 70;
	int Y_PLantUI = 100;

	//Arrays for plants positions
	int X_Plant[5], Y_Plant[5];
	X_Plant[0] = 800, Y_Plant[0] = 120;

	for (int i = 1; i < 5; i++)
	{
		X_Plant[i] = 800, Y_Plant[i] = Y_Plant[i - 1] + 170;
	}

	//Arrays for greenballs
	int X_Greenball[5], Y_Greenball[5];
	X_Greenball[0] = 870, Y_Greenball[0] = 130;

	for (int i = 1; i < 5; i++)
	{
		X_Greenball[i] = 860, Y_Greenball[i] = Y_Greenball[i - 1] + 170;
	}

	//Arrays for zombies walking positions
	int X_ZombieWalk[5], Y_ZombieWalk[5];
	X_ZombieWalk[0] = pvz.getWindowWidth(), Y_ZombieWalk[0] = 70;

	for (int i = 1; i < 5; i++)
	{
		X_ZombieWalk[i] = pvz.getWindowWidth(), Y_ZombieWalk[i] = Y_ZombieWalk[i - 1] + 170;
	}

	int W1920 = pvz.getWindowWidth(), H1080 = pvz.getWindowHeight();

	//Loads start screen
	int Start = pvz.loadImage("Start.PNG");
	pvz.resizeImage(Start, W1920, H1080);

	//Loads pause sreen
	int Pause = pvz.loadImage("Pause.PNG");
	pvz.resizeImage(Pause, W1920, H1080);

	//Loads background
	int Background = pvz.loadImage("Background.PNG");

	pvz.resizeImage(Background, W1920, H1080);

	//Loads gameover screen
	int GameOver = pvz.loadImage("GameOver.PNG");
	pvz.resizeImage(GameOver, W1920, H1080);

	//Loads victory screen
	int VictoryTimer = 0;
	int Victory = pvz.loadImage("Victory.PNG");
	pvz.resizeImage(GameOver, W1920, H1080);

	//Loads greenball
	int Greenball = pvz.loadImage("Greenball.PNG");
	pvz.resizeImage(Greenball, 50, 50);

	//Loads plant
	char Plant_Name[] = "Plant";
	int Plants_Ids[11];
	loadAndResizeImages(11, Plant_Name, Plants_Ids, 100, 100);

	//Loads zombie walking, eating and dying
	Zombie Zombie_Ids[5];

	char Walking_Name[] = "Zombie";
	loadAndResizeImages(7, Walking_Name, Zombie_Ids->Walking, 160, 160); //(*Zombie_Ids).Walking can also be used

	char Splash_Name[] = "Splash";
	loadAndResizeImages(2, Splash_Name, Zombie_Ids->Splash, 90, 90);

	char Eating_Name[] = "Eating";
	loadAndResizeImages(6, Eating_Name, Zombie_Ids->Eating, 160, 160);

	char Dying_Name[] = "Dying";
	loadAndResizeImages(14, Dying_Name, Zombie_Ids->Dying, 160, 160);

	//Loads sun
	int Sun = pvz.loadImage("Sun.PNG");

	//Loads sun UI
	char SunUI_Name[] = "SunUI";
	int SunUI_Ids[6];
	loadAndResizeImages(6, SunUI_Name, SunUI_Ids, 200, 76);

	//Loads plant UI
	int PlantUI = pvz.loadImage("PlantUI.PNG");
	pvz.resizeImage(PlantUI, 140, 91);
	int AltPlantUI = pvz.loadImage("AltPlantUI.PNG");
	pvz.resizeImage(AltPlantUI, 140, 91);

	//Animations and flags

	int PlantSpawned[5] = { 0 };
	int BallShot[5] = { 0 };
	int PlantsAnimation[5] = { 0 };
	int ZombieAnimation[5] = { 0 };
	int EatingAnimation[5] = { 0 };
	int SplashAnimation[5] = { 0 };
	int DyingAnimation[5] = { 0 };
	int EatingDamage[5] = { 0 };
	int SplashDamage[5] = { 0 };
	int EatingFlag[5] = { 0 };
	int Dead[5] = { 0 };
	int PressedButton[5] = { 0 };
	int PressedTab = 0;
	int PlayedVictory = 0;
	int PlayedGameOver = 0;

	//Randomises zombies spawning
	int MatchZombieTimer[5] = { 0 };
	int RandomZombieTimer[5];
	for (int i = 0; i < 5; i++)
	{
		RandomZombieTimer[i] = rand() % 65;
	}

	int PauseFlag = -1;
	int Started = 0;

	int Stopwatch = 0;

	while (true)
	{
		CompletionTime = fopen("Completion Times.txt", "a");
		pvz.beginDraw();

		//Draws start screen
		if (Started == 0)
			pvz.drawImage(Start, 0, 0, pvz.generateFromRGB(275, 255, 255));

		//Starts the game
		if (GetAsyncKeyState(VK_TAB) && PressedTab == 0)/*The Tab Key*/ {
			PressedTab = 1;
			Started = 1;
			PlaySound(NULL, NULL, SND_ASYNC); // Stops the current music
			PlaySound(TEXT("Gameplay.wav"), NULL, SND_ASYNC | SND_LOOP);
		}

		//Pauses and resumes the game
		if (GetAsyncKeyState(VK_CONTROL) && Started == 1 && PlayedVictory == 0 && PlayedGameOver == 0)/*The Control Key*/ {
			if (PauseFlag == -1) {
				PlaySound(NULL, NULL, SND_ASYNC);
				PlaySound(TEXT("MainMenu.wav"), NULL, SND_ASYNC | SND_LOOP);

			}
			if (PauseFlag == 1) {
				PlaySound(NULL, NULL, SND_ASYNC);
				PlaySound(TEXT("Gameplay.wav"), NULL, SND_ASYNC | SND_LOOP);
			}
			PauseFlag *= -1;
		}

		if (PauseFlag == 1 && Started == 1) {
			pvz.drawImage(Pause, 0, 0, pvz.generateFromRGB(275, 255, 255));
		}

		if (PauseFlag == -1 && Started == 1)
		{
			//Increases play time when game is not paused
			Stopwatch++;

			//Draws background
			pvz.drawImage(Background, 0, 0, pvz.generateFromRGB(275, 255, 255));

			//Timer for sun to spawn
			SunTimer++;
			if (SunTimer == 24)
			{
				X_Sun += rand() % 400;
				SunEnable = 1;
			}
			if (SunEnable == 1)
			{
				pvz.drawImage(Sun, X_Sun, Y_Sun, pvz.generateFromRGB(76, 175, 80));
				Y_Sun += 40;
			}

			//Collects the sun
			if (GetAsyncKeyState('C') && SunEnable == 1)
			{
				Y_Sun = 0;
				SunEnable = 0;
				SunTimer = 0;
				SunCounter++;

				pvz.deleteImage(Sun);
			}

			//Deletes the sun if it exits boundaries
			if (Y_Sun > pvz.getWindowHeight())
			{
				Y_Sun = 0;
				SunEnable = 0;
				SunTimer = 0;

				pvz.deleteImage(Sun);
			}

			//Ensures Sun UI doesn't exceed the specified value
			if (SunCounter >= 5)
				SunCounter = 5;
			pvz.drawImage(SunUI_Ids[SunCounter], X_SunUI, Y_SunUI, pvz.generateFromRGB(0, 0, 0));

			//Draws the plant UI
			if (SunCounter >= 1)
				pvz.drawImage(PlantUI, X_PlantUI, Y_PLantUI, pvz.generateFromRGB(0, 0, 0));
			else
				pvz.drawImage(AltPlantUI, X_PlantUI, Y_PLantUI, pvz.generateFromRGB(0, 0, 0));

			//------------------------------------------------------------------------------------------------------------------------------------//

			// Collapse/Extend for logic of the first row
			if (true) {

				//Random spawn time
				if (MatchZombieTimer[0] == RandomZombieTimer[0])
				{
					MatchZombieTimer[0]--;

					//Draws normal walking
					if (EatingFlag[0] == 0 && Dead[0] == 0) {
						pvz.drawImage(Zombie_Ids->Walking[ZombieAnimation[0]], X_ZombieWalk[0], Y_ZombieWalk[0], pvz.generateFromRGB(0, 0, 0));
						ZombieAnimation[0]++;
						if (ZombieAnimation[0] == 7)
							ZombieAnimation[0] = 0;
						X_ZombieWalk[0] -= 10;
					}
				}
				MatchZombieTimer[0]++;

				//Ball collision to zombie
				if (X_Greenball[0] <= X_ZombieWalk[0] + 150 && X_Greenball[0] >= X_ZombieWalk[0] - 150 && BallShot[0] == 1) {
					pvz.deleteImage(Greenball);
					BallShot[0] = 0;
					X_Greenball[0] = 860;
					pvz.drawImage(Zombie_Ids->Splash[SplashAnimation[0]], X_ZombieWalk[0] - 10, Y_ZombieWalk[0] + 40, pvz.generateFromRGB(0, 0, 0));
					SplashAnimation[0]++;
					SplashDamage[0]++;

					//Resets the splash animation
					if (SplashAnimation[0] == 2)
					{
						SplashAnimation[0] = 0;
					}
				}

				//Zombie dies
				if (SplashDamage[0] == 4)
				{
					Dead[0] = 1;
				}
				if (Dead[0] == 1)
				{
					//Ensures dying frames are not exceeded
					if (DyingAnimation[0] >= 0 && DyingAnimation[0] < 14) {
						pvz.drawImage(Zombie_Ids->Dying[DyingAnimation[0]], X_ZombieWalk[0], Y_ZombieWalk[0], pvz.generateFromRGB(0, 0, 0));
						DyingAnimation[0]++;
					}
				}

				//Ensures that when zombie reaches plant, the plant is not dead and the zombie is not dead
				if (X_ZombieWalk[0] <= X_Plant[0] + 50 && X_ZombieWalk[0] >= X_Plant[0] - 70 && PlantSpawned[0] == 1 && Dead[0] == 0) {
					EatingFlag[0] = 1;
					pvz.drawImage(Zombie_Ids->Eating[EatingAnimation[0]], X_ZombieWalk[0], Y_ZombieWalk[0], pvz.generateFromRGB(0, 0, 0));
					EatingAnimation[0]++;
					if (EatingAnimation[0] == 6)
						EatingAnimation[0] = 0;
					EatingDamage[0]++;

					//Plant is eaten
					if (EatingDamage[0] == 15) {
						PlantSpawned[0] = 0;
						PressedButton[0] = 0;
						BallShot[0] = 0;
						X_ZombieWalk[0] -= 10;
						EatingFlag[0] = 0;
					}
				}

				//Gets player input
				if (GetAsyncKeyState('1') && SunCounter >= 1 && PressedButton[0] == 0)
				{
					PlantSpawned[0] = 1;
					SunCounter--;

					//Ensures you cannot place another plant in the same spot
					PressedButton[0] = 1;
				}

				if (PlantSpawned[0] == 1)
				{
					//Draws plant
					pvz.drawImage(Plants_Ids[PlantsAnimation[0]], X_Plant[0], Y_Plant[0], pvz.generateFromRGB(0, 0, 0));
					PlantsAnimation[0]++;
					if (PlantsAnimation[0] == 5)
					{
						//Shoots ball at the correct frame
						BallShot[0] = 1;
					}

					//Resets plant firing animation if the zombie is alive (note:it's not alive ;))
					if (PlantsAnimation[0] == 11 && Dead[0] == 0)
						PlantsAnimation[0] = 0;

					//Starts idle animation if the zombie is dead 
					if (PlantsAnimation[0] == 11 && Dead[0] == 1)
						PlantsAnimation[0] = 6;
				}

				//Fires the ball if the zombie is alive
				if (BallShot[0] == 1 && Dead[0] == 0)
				{
					pvz.drawImage(Greenball, X_Greenball[0], Y_Greenball[0], pvz.generateFromRGB(0, 0, 0));
					X_Greenball[0] += 100;

					//Resets the position and deletes the ball if it exceeds boundaries
					if (X_Greenball[0] > pvz.getWindowWidth())
					{
						X_Greenball[0] = 860;
						pvz.deleteImage(Greenball);
						BallShot[0] = 0;

					}

				}
			}

			// Collapse/Extend for logic of the second row
			if (true) {

				//Random spawn time
				if (MatchZombieTimer[1] == RandomZombieTimer[1])
				{
					MatchZombieTimer[1]--;

					//Draws normal walking
					if (EatingFlag[1] == 0 && Dead[1] == 0) {
						pvz.drawImage(Zombie_Ids->Walking[ZombieAnimation[1]], X_ZombieWalk[1], Y_ZombieWalk[1], pvz.generateFromRGB(0, 0, 0));
						ZombieAnimation[1]++;
						if (ZombieAnimation[1] == 7)
							ZombieAnimation[1] = 0;
						X_ZombieWalk[1] -= 10;
					}
				}
				MatchZombieTimer[1]++;

				//Ball collision to zombie
				if (X_Greenball[1] <= X_ZombieWalk[1] + 150 && X_Greenball[1] >= X_ZombieWalk[1] - 150 && BallShot[1] == 1) {
					pvz.deleteImage(Greenball);
					BallShot[1] = 0;
					X_Greenball[1] = 860;
					pvz.drawImage(Zombie_Ids->Splash[SplashAnimation[1]], X_ZombieWalk[1] - 10, Y_ZombieWalk[1] + 40, pvz.generateFromRGB(0, 0, 0));
					SplashAnimation[1]++;
					SplashDamage[1]++;

					//Resets the splash animation
					if (SplashAnimation[1] == 2)
					{
						SplashAnimation[1] = 0;
					}
				}

				//Zombie dies
				if (SplashDamage[1] == 4)
				{
					Dead[1] = 1;
				}
				if (Dead[1] == 1)
				{
					//Ensures dying frames are not exceeded
					if (DyingAnimation[1] >= 0 && DyingAnimation[1] < 14) {
						pvz.drawImage(Zombie_Ids->Dying[DyingAnimation[1]], X_ZombieWalk[1], Y_ZombieWalk[1], pvz.generateFromRGB(0, 0, 0));
						DyingAnimation[1]++;
					}
				}

				//Ensures that when zombie reaches plant, the plant is not dead and the zombie is not dead 
				if (X_ZombieWalk[1] <= X_Plant[1] + 50 && X_ZombieWalk[1] >= X_Plant[1] - 70 && PlantSpawned[1] == 1 && Dead[1] == 0) {
					EatingFlag[1] = 1;
					pvz.drawImage(Zombie_Ids->Eating[EatingAnimation[1]], X_ZombieWalk[1], Y_ZombieWalk[1], pvz.generateFromRGB(0, 0, 0));
					EatingAnimation[1]++;
					if (EatingAnimation[1] == 6)
						EatingAnimation[1] = 0;
					EatingDamage[1]++;

					//Plant is eaten
					if (EatingDamage[1] == 15) {
						PlantSpawned[1] = 0;
						PressedButton[1] = 0;
						BallShot[1] = 0;
						X_ZombieWalk[1] -= 10;
						EatingFlag[1] = 0;
					}
				}

				//Gets player input
				if (GetAsyncKeyState('2') && SunCounter >= 1 && PressedButton[1] == 0)
				{
					PlantSpawned[1] = 1;
					SunCounter--;

					//Ensures you cannot place another plant in the same spot
					PressedButton[1] = 1;
				}

				if (PlantSpawned[1] == 1)
				{
					//Draws plant
					pvz.drawImage(Plants_Ids[PlantsAnimation[1]], X_Plant[1], Y_Plant[1], pvz.generateFromRGB(0, 0, 0));
					PlantsAnimation[1]++;
					if (PlantsAnimation[1] == 5)
					{
						//Shoots ball at the correct frame
						BallShot[1] = 1;
					}

					//Resets plant firing animation if the zombie is alive
					if (PlantsAnimation[1] == 11 && Dead[1] == 0)
						PlantsAnimation[1] = 0;

					//Starts idle animation if the zombie is dead 
					if (PlantsAnimation[1] == 11 && Dead[1] == 1)
						PlantsAnimation[1] = 6;
				}

				//Fires the ball if the zombie is alive
				if (BallShot[1] == 1 && Dead[1] == 0)
				{
					pvz.drawImage(Greenball, X_Greenball[1], Y_Greenball[1], pvz.generateFromRGB(0, 0, 0));
					X_Greenball[1] += 100;

					//Resets the position and deletes the ball if it exceeds boundaries
					if (X_Greenball[1] > pvz.getWindowWidth())
					{
						X_Greenball[1] = 860;
						pvz.deleteImage(Greenball);
						BallShot[1] = 0;

					}

				}
			}

			// Collapse/Extend for logic of the third row
			if (true) {

				//Random spawn time
				if (MatchZombieTimer[2] == RandomZombieTimer[2])
				{
					MatchZombieTimer[2]--;

					//Draws normal walking
					if (EatingFlag[2] == 0 && Dead[2] == 0) {
						pvz.drawImage(Zombie_Ids->Walking[ZombieAnimation[2]], X_ZombieWalk[2], Y_ZombieWalk[2], pvz.generateFromRGB(0, 0, 0));
						ZombieAnimation[2]++;
						if (ZombieAnimation[2] == 7)
							ZombieAnimation[2] = 0;
						X_ZombieWalk[2] -= 10;
					}
				}
				MatchZombieTimer[2]++;

				//Ball collision to zombie
				if (X_Greenball[2] <= X_ZombieWalk[2] + 150 && X_Greenball[2] >= X_ZombieWalk[2] - 150 && BallShot[2] == 1) {
					pvz.deleteImage(Greenball);
					BallShot[2] = 0;
					X_Greenball[2] = 860;
					pvz.drawImage(Zombie_Ids->Splash[SplashAnimation[2]], X_ZombieWalk[2] - 10, Y_ZombieWalk[2] + 40, pvz.generateFromRGB(0, 0, 0));
					SplashAnimation[2]++;
					SplashDamage[2]++;

					//Resets the splash animation
					if (SplashAnimation[2] == 2)
					{
						SplashAnimation[2] = 0;
					}
				}

				//Zombie dies
				if (SplashDamage[2] == 4)
				{
					Dead[2] = 1;
				}
				if (Dead[2] == 1)
				{
					//Ensures dying frames are not exceeded
					if (DyingAnimation[2] >= 0 && DyingAnimation[2] < 14) {
						pvz.drawImage(Zombie_Ids->Dying[DyingAnimation[2]], X_ZombieWalk[2], Y_ZombieWalk[2], pvz.generateFromRGB(0, 0, 0));
						DyingAnimation[2]++;
					}
				}

				//Ensures that when zombie reaches plant, the plant is not dead and the zombie is not dead 
				if (X_ZombieWalk[2] <= X_Plant[2] + 50 && X_ZombieWalk[2] >= X_Plant[2] - 70 && PlantSpawned[2] == 1 && Dead[2] == 0) {
					EatingFlag[2] = 1;
					pvz.drawImage(Zombie_Ids->Eating[EatingAnimation[2]], X_ZombieWalk[2], Y_ZombieWalk[2], pvz.generateFromRGB(0, 0, 0));
					EatingAnimation[2]++;
					if (EatingAnimation[2] == 6)
						EatingAnimation[2] = 0;
					EatingDamage[2]++;

					//Plant is eaten
					if (EatingDamage[2] == 15) {
						PlantSpawned[2] = 0;
						PressedButton[2] = 0;
						BallShot[2] = 0;
						X_ZombieWalk[2] -= 10;
						EatingFlag[2] = 0;
					}
				}

				//Gets player input
				if (GetAsyncKeyState('3') && SunCounter >= 1 && PressedButton[2] == 0)
				{
					PlantSpawned[2] = 1;
					SunCounter--;

					//Ensures you cannot place another plant in the same spot
					PressedButton[2] = 1;
				}

				if (PlantSpawned[2] == 1)
				{
					//Draws plant
					pvz.drawImage(Plants_Ids[PlantsAnimation[2]], X_Plant[2], Y_Plant[2], pvz.generateFromRGB(0, 0, 0));
					PlantsAnimation[2]++;
					if (PlantsAnimation[2] == 5)
					{
						//Shoots ball at the correct frame
						BallShot[2] = 1;
					}

					//Resets plant firing animation if the zombie is alive
					if (PlantsAnimation[2] == 11 && Dead[2] == 0)
						PlantsAnimation[2] = 0;

					//Starts idle animation if the zombie is dead 
					if (PlantsAnimation[2] == 11 && Dead[2] == 1)
						PlantsAnimation[2] = 6;
				}

				//Fires the ball if the zombie is alive
				if (BallShot[2] == 1 && Dead[2] == 0)
				{
					pvz.drawImage(Greenball, X_Greenball[2], Y_Greenball[2], pvz.generateFromRGB(0, 0, 0));
					X_Greenball[2] += 100;

					//Resets the position and deletes the ball if it exceeds boundaries
					if (X_Greenball[2] > pvz.getWindowWidth())
					{
						X_Greenball[2] = 860;
						pvz.deleteImage(Greenball);
						BallShot[2] = 0;

					}

				}
			}

			// Collapse/Extend for logic of the fourth row
			if (true) {

				//Random spawn time
				if (MatchZombieTimer[3] == RandomZombieTimer[3])
				{
					MatchZombieTimer[3]--;

					//Draws normal walking
					if (EatingFlag[3] == 0 && Dead[3] == 0) {
						pvz.drawImage(Zombie_Ids->Walking[ZombieAnimation[3]], X_ZombieWalk[3], Y_ZombieWalk[3], pvz.generateFromRGB(0, 0, 0));
						ZombieAnimation[3]++;
						if (ZombieAnimation[3] == 7)
							ZombieAnimation[3] = 0;
						X_ZombieWalk[3] -= 10;
					}
				}
				MatchZombieTimer[3]++;

				//Ball collision to zombie
				if (X_Greenball[3] <= X_ZombieWalk[3] + 150 && X_Greenball[3] >= X_ZombieWalk[3] - 150 && BallShot[3] == 1) {
					pvz.deleteImage(Greenball);
					BallShot[3] = 0;
					X_Greenball[3] = 860;
					pvz.drawImage(Zombie_Ids->Splash[SplashAnimation[3]], X_ZombieWalk[3] - 10, Y_ZombieWalk[3] + 40, pvz.generateFromRGB(0, 0, 0));
					SplashAnimation[3]++;
					SplashDamage[3]++;

					//Resets the splash animation
					if (SplashAnimation[3] == 2)
					{
						SplashAnimation[3] = 0;
					}
				}

				//Zombie dies
				if (SplashDamage[3] == 4)
				{
					Dead[3] = 1;
				}
				if (Dead[3] == 1)
				{
					//Ensures dying frames are not exceeded
					if (DyingAnimation[3] >= 0 && DyingAnimation[3] < 14) {
						pvz.drawImage(Zombie_Ids->Dying[DyingAnimation[3]], X_ZombieWalk[3], Y_ZombieWalk[3], pvz.generateFromRGB(0, 0, 0));
						DyingAnimation[3]++;
					}
				}

				//Ensures that when zombie reaches plant, the plant is not dead and the zombie is not dead 
				if (X_ZombieWalk[3] <= X_Plant[3] + 50 && X_ZombieWalk[3] >= X_Plant[3] - 70 && PlantSpawned[3] == 1 && Dead[3] == 0) {
					EatingFlag[3] = 1;
					pvz.drawImage(Zombie_Ids->Eating[EatingAnimation[3]], X_ZombieWalk[3], Y_ZombieWalk[3], pvz.generateFromRGB(0, 0, 0));
					EatingAnimation[3]++;
					if (EatingAnimation[3] == 6)
						EatingAnimation[3] = 0;
					EatingDamage[3]++;

					//Plant is eaten
					if (EatingDamage[3] == 15) {
						PlantSpawned[3] = 0;
						PressedButton[3] = 0;
						BallShot[3] = 0;
						X_ZombieWalk[3] -= 10;
						EatingFlag[3] = 0;
					}
				}

				//Gets player input
				if (GetAsyncKeyState('4') && SunCounter >= 1 && PressedButton[3] == 0)
				{
					PlantSpawned[3] = 1;
					SunCounter--;

					//Ensures you cannot place another plant in the same spot
					PressedButton[3] = 1;
				}

				if (PlantSpawned[3] == 1)
				{
					//Draws plant
					pvz.drawImage(Plants_Ids[PlantsAnimation[3]], X_Plant[3], Y_Plant[3], pvz.generateFromRGB(0, 0, 0));
					PlantsAnimation[3]++;
					if (PlantsAnimation[3] == 5)
					{
						//Shoots ball at the correct frame
						BallShot[3] = 1;
					}

					//Resets plant firing animation if the zombie is alive
					if (PlantsAnimation[3] == 11 && Dead[3] == 0)
						PlantsAnimation[3] = 0;

					//Starts idle animation if the zombie is dead 
					if (PlantsAnimation[3] == 11 && Dead[3] == 1)
						PlantsAnimation[3] = 6;
				}

				//Fires the ball if the zombie is alive
				if (BallShot[3] == 1 && Dead[3] == 0)
				{
					pvz.drawImage(Greenball, X_Greenball[3], Y_Greenball[3], pvz.generateFromRGB(0, 0, 0));
					X_Greenball[3] += 100;

					//Resets the position and deletes the ball if it exceeds boundaries
					if (X_Greenball[3] > pvz.getWindowWidth())
					{
						X_Greenball[3] = 860;
						pvz.deleteImage(Greenball);
						BallShot[3] = 0;

					}

				}
			}

			// Collapse/Extend for logic of the fifth row
			if (true) {

				//Random spawn time
				if (MatchZombieTimer[4] == RandomZombieTimer[4])
				{
					MatchZombieTimer[4]--;

					//Draws normal walking
					if (EatingFlag[4] == 0 && Dead[4] == 0) {
						pvz.drawImage(Zombie_Ids->Walking[ZombieAnimation[4]], X_ZombieWalk[4], Y_ZombieWalk[4], pvz.generateFromRGB(0, 0, 0));
						ZombieAnimation[4]++;
						if (ZombieAnimation[4] == 7)
							ZombieAnimation[4] = 0;
						X_ZombieWalk[4] -= 10;
					}
				}
				MatchZombieTimer[4]++;

				//Ball collision to zombie
				if (X_Greenball[4] <= X_ZombieWalk[4] + 150 && X_Greenball[4] >= X_ZombieWalk[4] - 150 && BallShot[4] == 1) {
					pvz.deleteImage(Greenball);
					BallShot[4] = 0;
					X_Greenball[4] = 860;
					pvz.drawImage(Zombie_Ids->Splash[SplashAnimation[4]], X_ZombieWalk[4] - 10, Y_ZombieWalk[4] + 40, pvz.generateFromRGB(0, 0, 0));
					SplashAnimation[4]++;
					SplashDamage[4]++;

					//Resets the splash animation
					if (SplashAnimation[4] == 2)
					{
						SplashAnimation[4] = 0;
					}
				}

				//Zombie dies
				if (SplashDamage[4] == 4)
				{
					Dead[4] = 1;
				}
				if (Dead[4] == 1)
				{
					//Ensures dying frames are not exceeded
					if (DyingAnimation[4] >= 0 && DyingAnimation[4] < 14) {
						pvz.drawImage(Zombie_Ids->Dying[DyingAnimation[4]], X_ZombieWalk[4], Y_ZombieWalk[4], pvz.generateFromRGB(0, 0, 0));
						DyingAnimation[4]++;
					}
				}

				//Ensures that when zombie reaches plant, the plant is not dead and the zombie is not dead 
				if (X_ZombieWalk[4] <= X_Plant[4] + 50 && X_ZombieWalk[4] >= X_Plant[4] - 70 && PlantSpawned[4] == 1 && Dead[4] == 0) {
					EatingFlag[4] = 1;
					pvz.drawImage(Zombie_Ids->Eating[EatingAnimation[4]], X_ZombieWalk[4], Y_ZombieWalk[4], pvz.generateFromRGB(0, 0, 0));
					EatingAnimation[4]++;
					if (EatingAnimation[4] == 6)
						EatingAnimation[4] = 0;
					EatingDamage[4]++;

					//Plant is eaten
					if (EatingDamage[4] == 15) {
						PlantSpawned[4] = 0;
						PressedButton[4] = 0;
						BallShot[4] = 0;
						X_ZombieWalk[4] -= 10;
						EatingFlag[4] = 0;
					}
				}

				//Gets player input
				if (GetAsyncKeyState('5') && SunCounter >= 1 && PressedButton[4] == 0)
				{
					PlantSpawned[4] = 1;
					SunCounter--;

					//Ensures you cannot place another plant in the same spot
					PressedButton[4] = 1;
				}

				if (PlantSpawned[4] == 1)
				{
					//Draws plant
					pvz.drawImage(Plants_Ids[PlantsAnimation[4]], X_Plant[4], Y_Plant[4], pvz.generateFromRGB(0, 0, 0));
					PlantsAnimation[4]++;
					if (PlantsAnimation[4] == 5)
					{
						//Shoots ball at the correct frame
						BallShot[4] = 1;
					}

					//Resets plant firing animation if the zombie is alive
					if (PlantsAnimation[4] == 11 && Dead[4] == 0)
						PlantsAnimation[4] = 0;

					//Starts idle animation if the zombie is dead 
					if (PlantsAnimation[4] == 11 && Dead[4] == 1)
						PlantsAnimation[4] = 6;
				}

				//Fires the ball if the zombie is alive
				if (BallShot[4] == 1 && Dead[4] == 0)
				{
					pvz.drawImage(Greenball, X_Greenball[4], Y_Greenball[4], pvz.generateFromRGB(0, 0, 0));
					X_Greenball[4] += 100;

					//Resets the position and deletes the ball if it exceeds boundaries
					if (X_Greenball[4] > pvz.getWindowWidth())
					{
						X_Greenball[4] = 860;
						pvz.deleteImage(Greenball);
						BallShot[4] = 0;

					}

				}
			}

			//------------------------------------------------------------------------------------------------------------------------------------//

			//Player lost
			if (X_ZombieWalk[0] <= 600 || X_ZombieWalk[1] <= 600 || X_ZombieWalk[2] <= 600 || X_ZombieWalk[3] <= 600 || X_ZombieWalk[4] <= 600) {
				pvz.drawImage(GameOver, 0, 0, pvz.generateFromRGB(275, 255, 255));

				if (PlayedGameOver == 0) {
					PlaySound(NULL, NULL, SND_ASYNC);
					PlaySound(TEXT("GameOver.wav"), NULL, SND_ASYNC);
					PlayedGameOver = 1;
				}

				//Restarts the game if player loses :(
				if (GetAsyncKeyState(VK_RIGHT))/*The Right Arrow Key*/ {
					PlaySound(NULL, NULL, SND_ASYNC);
					PlaySound(TEXT("MainMenu.wav"), NULL, SND_ASYNC | SND_LOOP);

					Stopwatch = 0;
					PressedTab = 0;
					PlayedVictory = 0;
					PlayedGameOver = 0;
					X_Sun = 500;
					Y_Sun = 0;
					SunTimer = 0;
					SunEnable = 0;
					SunCounter = 0;
					VictoryTimer = 0;

					X_ZombieWalk[0] = pvz.getWindowWidth(), Y_ZombieWalk[0] = 70;
					for (int i = 1; i < 5; i++)
					{
						X_ZombieWalk[i] = pvz.getWindowWidth(), Y_ZombieWalk[i] = Y_ZombieWalk[i - 1] + 170;
					}

					X_Greenball[0] = 870, Y_Greenball[0] = 130;
					for (int i = 1; i < 5; i++)
					{
						X_Greenball[i] = 860, Y_Greenball[i] = Y_Greenball[i - 1] + 170;

					}

					X_Plant[0] = 800, Y_Plant[0] = 120;
					for (int i = 1; i < 5; i++)
					{
						X_Plant[i] = 800, Y_Plant[i] = Y_Plant[i - 1] + 170;
					}

					for (int i = 0; i < 5; i++)
					{
						RandomZombieTimer[i] = rand() % 65;
					}

					for (int i = 0; i < 5; i++) {
						PlantSpawned[i] = 0;
						BallShot[i] = 0;
						PlantsAnimation[i] = 0;
						ZombieAnimation[i] = 0;
						EatingAnimation[i] = 0;
						SplashAnimation[i] = 0;
						DyingAnimation[i] = 0;
						EatingDamage[i] = 0;
						SplashDamage[i] = 0;
						EatingFlag[i] = 0;
						Dead[i] = 0;
						PressedButton[i] = 0;
						MatchZombieTimer[i] = 0;
					}

					PauseFlag = -1;
					Started = 0;
					continue;
				}

				//Exits the game if player loses ;(
				else if (GetAsyncKeyState(VK_LEFT))/*The Left Arrow Key*/ {
					return 0;
				}
			}

			//Player won
			if (Dead[0] == 1 && Dead[1] == 1 && Dead[2] == 1 && Dead[3] == 1 && Dead[4] == 1) {
				//Timer to draw the victory screen less aggresively
				VictoryTimer++;
				if (VictoryTimer >= 20) {
					pvz.drawImage(Victory, 0, 0, pvz.generateFromRGB(275, 255, 255));
					if (PlayedVictory == 0) {
						PlaySound(NULL, NULL, SND_ASYNC);
						PlaySound(TEXT("Victory.wav"), NULL, SND_ASYNC);
						PlayedVictory = 1;
					}
				}
				//Restarts the game if the player wins :)
				if (GetAsyncKeyState(VK_RIGHT))/*The Right Arrow Key*/ {
					PlaySound(NULL, NULL, SND_ASYNC);
					PlaySound(TEXT("MainMenu.wav"), NULL, SND_ASYNC | SND_LOOP);

					fprintf(CompletionTime, "\n%s: %d", Name, Stopwatch);
					fclose(CompletionTime);

					Stopwatch = 0;
					PressedTab = 0;
					PlayedVictory = 0;
					PlayedGameOver = 0;
					X_Sun = 500;
					Y_Sun = 0;
					SunTimer = 0;
					SunEnable = 0;
					SunCounter = 0;
					VictoryTimer = 0;

					X_ZombieWalk[0] = pvz.getWindowWidth(), Y_ZombieWalk[0] = 70;
					for (int i = 1; i < 5; i++)
					{
						X_ZombieWalk[i] = pvz.getWindowWidth(), Y_ZombieWalk[i] = Y_ZombieWalk[i - 1] + 170;
					}

					X_Greenball[0] = 870, Y_Greenball[0] = 130;
					for (int i = 1; i < 5; i++)
					{
						X_Greenball[i] = 860, Y_Greenball[i] = Y_Greenball[i - 1] + 170;

					}

					X_Plant[0] = 800, Y_Plant[0] = 120;
					for (int i = 1; i < 5; i++)
					{
						X_Plant[i] = 800, Y_Plant[i] = Y_Plant[i - 1] + 170;
					}

					for (int i = 0; i < 5; i++)
					{
						RandomZombieTimer[i] = rand() % 65;
					}

					for (int i = 0; i < 5; i++) {
						PlantSpawned[i] = 0;
						BallShot[i] = 0;
						PlantsAnimation[i] = 0;
						ZombieAnimation[i] = 0;
						EatingAnimation[i] = 0;
						SplashAnimation[i] = 0;
						DyingAnimation[i] = 0;
						EatingDamage[i] = 0;
						SplashDamage[i] = 0;
						EatingFlag[i] = 0;
						Dead[i] = 0;
						PressedButton[i] = 0;
						MatchZombieTimer[i] = 0;
					}

					PauseFlag = -1;
					Started = 0;
					continue;
				}

				//Exits the game if the player wins :)
				else if (GetAsyncKeyState(VK_LEFT))/*The Left Arrow Key*/ {
					fprintf(CompletionTime, "\n%s: %d", Name, Stopwatch);
					fclose(CompletionTime);
					return 0;
				}
			}
		}

		if (GetAsyncKeyState(VK_ESCAPE)) //Press Escape Key to exit game
			return 0;
		Sleep(150);
		pvz.endDraw();
	}

	return 0;
}

//For correct resolutions please use 1920x1080 and set scale to 100%

/*Controls for playing:

1) Tab Key to start in main screen
2) Control Key to pause and resume the game
3) Number Keys 1 to 5 place plants up to down
4) Escape Key to exit at anytime
5) Right Arrow Key to restart after a win or a loss
6) Left Arrow Key to exit after a win or a loss

*/

/*Code written and managed by :

Toqa Hossam Roshdy-----------------221004061
Ahmed Ashraf Hassanien-------------221004510
Mahmoud Mohamed Aboheif------------221004705

*/

/*

Refrences(videos, websites and tools used):
	https://youtu.be/TIUGQr-yNyw?si=-O7oYZ7NvgwWedTG
	https://youtu.be/aqsO2VITlig?si=mhDA7Ne8SD00fZUa
	https://youtu.be/9WeDQHi6sJs?si=j4q7d_djX3qaqqDk
	https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	https://www.remove.bg
	microsoft paint

*/