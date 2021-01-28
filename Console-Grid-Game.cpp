// Austin Lanphear - This program contains an adventure game where the player navigates a 5x5 grid to find a key and locked chest, while navigating through obstacles.
// Created on: December 2nd, 2018 || Last Updated: December 16th, 2018

#include <iostream>	//For Console Out
#include <stdlib.h>	//For random number generation
#include <time.h>	//For time and random number generation
#include <vector>	//For vectors
#include <conio.h>	//For keyboard input
#include <string>	//For strings

///////////////////
// Program Setup //
///////////////////


//Defining special keys as char type
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define EXIT 113

//Game Screen Identifiers
void pregame();
void postgame();
void gameLoop();

//Move Player Identifiers
void movePlayer_directions();
void movePlayer_keyboard();

//Checks Identifiers
void gridBoundaryCheck();
void gridObject_Check();
void turnsTaken_Check();

//Populate Identifiers
void populate_positions();
void populate_gridObjects();

//Grid Identifiers
void Questions(int);
void createGrid();
bool obstacle(std::string, std::string[], std::string[], bool);

//Debug Indentifers
void DEBUG();
void DEBUG_GetPosList();
void DEBUG_GetPlayerInfo();
void DEBUG_GetObjects();


/////////////////////
//Global Variables //
///////////////////// 

//Struct containing the Game's Data
struct gameData
{
	//Grid
	const int WIDTH = 5;	//5x5 Grid to Follow Instruction can be modified to any width and height
	const int HEIGHT = 5;
	const int GRID_X_MIN = 0;
	const int GRID_Y_MIN = 0;
	const int GRID_X_MAX = WIDTH - 1;
	const int GRID_Y_MAX = HEIGHT - 1;

	//Obstacles
	//2D Vectors in (X,Y) format
	std::vector<std::vector<int>> gridObjects = {};
	std::vector<std::vector<int>> positions = {};
	std::vector<bool> gridObjectCompletion = {};
	const int OB_MIN = 3;	//Obstacle Count Min
	const int OB_MAX = 4;	//Obstacle Count Max

	//Turn Elements
	const int TURN_LIMIT = 20;
	int turnsTaken = 0;

	//General Booleans
	bool gameComplete = false;
	bool keyboardControlled = true;
	bool DebugMode = false;

};

//Object of gameData as gd
gameData gd = gameData();

//Struct containing the Player's Data
struct playerData
{
	//Player's name
	std::string name = "";

	//Player's Position (Starting Position at center of grid)
	std::vector<int> pos = {
		((gd.WIDTH / 2) + (gd.WIDTH % 2)) - 1 ,
		((gd.HEIGHT / 2) + (gd.HEIGHT % 2)) - 1
	};

	//Statistical Data
	std::vector<int> dir_moved = { 0,0,0,0 };	//Order of Ints: 0 = North, 1 = South, 2 = East, 3 = West
	int correctAnswers = 0;
};

//Object of playerData as player
playerData player = playerData();




//////////////////////
// The Main Program //
//////////////////////
int main()
{

	//Renaming Console Window
	system("title Maze Game by Austin Lanphear");

	//Generates random seed from system time
	srand((signed int)time(NULL));

	//Game Screens
	pregame();
	gameLoop();
	postgame();

	return 0;
}

//////////////////
// Game Screens //
//////////////////


//Setup for game global variables and instructing player how the game is played
void pregame()
{
	//Ask user their name
	std::cout << "Welcome to \"C++ Maze Game\" adventurer! What is your name? \n";
	std::cin >> player.name;
	system("cls");

	//Welcome them and tell them what the game rules are
	std::cout << player.name << ", your adventure is just beginning!\n"
		<< "You will try to find the treasure in this maze, all you have to do is navigate the " << gd.WIDTH << "x" << gd.HEIGHT << " map!\n"
		<< "If you come across an obstacle you are tasked with answering C++ questions!\n"
		<< "Answer them to the best of the ability and you will proceed forward!\n"
		<< "Reach the treasure to win the game! But be wary you only get " << gd.TURN_LIMIT << " turns, so tread lightly.\n"
		<< "Good luck and have fun! \n\n";

	//Ask the user for type of movement option
	std::cout << "One last thing, " << player.name << std::endl
		<< "Would you like to play with the Arrow Keys/WASD?\n"
		<< "If not you can type in the directions of North, South, East, or West to move.\n"
		<< "What method of movement do you prefer?\n"
		<< "(D)irectional Movement or (C)ompass movement: ";

	//Change type of movement dependent on user choice
	std::cin.ignore();
	char movementChoice = std::cin.get();
	if (movementChoice == 'D' || movementChoice == 'd')
	{
		gd.keyboardControlled = true;
	}
	else if (movementChoice == 'C' || movementChoice == 'c')
	{
		gd.keyboardControlled = false;
	}
	else
	{
		//Directional Movement Chosen as Default
		gd.keyboardControlled = true;
	}

	//Clears screen on game startup
	system("cls");
}

//Shows the players score, moves, and etc
void postgame()
{
	std::cout << player.name << "'s Stats: \n"
		<< "Moved North " << player.dir_moved[0] << " times." << std::endl
		<< "Moved South " << player.dir_moved[1] << " times." << std::endl
		<< "Moved East  " << player.dir_moved[2] << " times." << std::endl
		<< "Moved West  " << player.dir_moved[3] << " times." << std::endl
		<< "Number of Correct Answers: " << player.correctAnswers << std::endl << std::endl
		<< "Press Enter to Exit Game\n\n";

	std::cin.ignore();
	system("cls");
}

//The main game loop where the player decides to move in a direction and the relevant operations happen until the game ends.
void gameLoop()
{
	//Data Functions to fill variables at run-time
	populate_positions();
	populate_gridObjects();

	//Loop that pertains to game logic
	while (!gd.gameComplete)
	{
		//If not in Debug Mode update game as normal
		if (!gd.DebugMode)
		{
			//Gives the player's position
			std::cout << player.name << ", you are at: (" << player.pos[0] << "," << player.pos[1] << ")\n";

			//Display Grid
			createGrid();

			//Shows player turns
			std::cout << "You have " << (gd.TURN_LIMIT - gd.turnsTaken) << " turns left.\n";

			//Turn Checker
			turnsTaken_Check();

			//Move player based on prefered input
			if (gd.keyboardControlled)
			{
				movePlayer_keyboard();
			}
			else
			{
				movePlayer_directions();
			}

			//Movement Checks
			gridBoundaryCheck();
			gridObject_Check();
		}
		else
		{
			DEBUG();
		}
	}
}

////////////////////////
// Checks & Obstacles ////////////////////////////////////////////////////////////
// Functions that check for validity in player position or other game functions //
//////////////////////////////////////////////////////////////////////////////////

//Checks how many turns are taken
void turnsTaken_Check()
{
	if (gd.turnsTaken >= gd.TURN_LIMIT)
	{
		gd.gameComplete = true;
	}
}

//Checks if player has "landed" on grid object
void gridObject_Check()
{
	//Index Counter
	int indexCounter = 0;

	//Checks for obstacles
	for (int i = 0; i < (signed int)gd.gridObjects.size(); i++)
	{
		//Checks if there is an obstacle on the player's current position
		//If an obstacle is there, use the obstacle function
		if (player.pos == gd.gridObjects[i])
		{
			//When an object is found count up on index counter
			indexCounter++;

			//Asks question dependent on space using index
			Questions(i);
		}
	}
}

//Preset Questions that may be used within the program, in any order based on index chosen
void Questions(int index)
{
	//Used for updating all arrays regarding questions
	const int NUMBER_OF_QUESTIONS = 8;

	//Questions based on Array
	std::string question[NUMBER_OF_QUESTIONS] =
	{
		"What data type do C++ \"main()\" functions run in?",
		"What type of statement would you use for true/false questions in C++?",
		"What is the data type of code that use \"while\", \"do\", and \"for\" statements?",
		"How would a programmer output code to a console screen?",
		"How would a programmer end a statement? With what symbol?",
		"What statement type of variable would you use for a true/false situatation?",
		"What data type is a modifiable list, at program run-time?",
		"What is a collection of code bases usually called?"
	};

	//Options based on 2D Array
	std::string options[NUMBER_OF_QUESTIONS][4] =
	{
		{ "A.) void", "B.) int", "C.) string", "D.) boolean" },
		{ "A.) if", "B.) while", "C.) get", "D.) do" },
		{ "A.) Variables", "B.) Pointers ", "C.) Functions", "D.) Loops" },
		{ "A.) print", "B.) console.log", "C.) cin", "D. cout"},
		{ "A.) |", "B.) :", "C.) ;", "D.) $"},
		{ "A.) boolean", "B.) header", "C.) float", "vector"},
		{ "A.) array", "B.) vector", "C.) structs", "D.) string"},
		{ "A.) block", "B.) book", "C.) library", "D.) index"}
	};

	//Answers based on 2D Array
	std::string answers[NUMBER_OF_QUESTIONS][4] =
	{
		{ "B", "b", "int", "Int" },
		{ "A", "a", "if", "If" },
		{ "D", "d", "Loops", "loops" },
		{ "D", "d", "Cout", "cout"},
		{ "C", "c", ";", ";"},
		{ "A", "a", "Boolean", "boolean"},
		{ "B", "b", "Vector", "vector"},
		{ "C", "c", "Library", "library"}
	};

	//Used for checking if obstacle is chest or not
	bool chestChecker = false;

	//Choose obstacle depending on index
	for (int i = 0; i < (signed int)gd.gridObjects.size(); i++)
	{
		//Check which obstacle to look for
		if (index == i)
		{
			//Check if obstacle is chest or not
			if (i == 0)
			{
				chestChecker = true;
			}
			else
			{
				chestChecker = false;
			}

			//Check if obstacle has been completed yet or not
			if (!gd.gridObjectCompletion[i])
			{
				gd.gridObjectCompletion[i] = obstacle(question[i], options[i], answers[i], chestChecker);
			}
		}
	}
}

//A possible obstacle in maze for player to come across
//Contains a position value, question, and answer
bool obstacle(std::string question, std::string options[], std::string answer[], bool isChest)
{
	//Clears Screen for better readability of console
	system("cls");

	//Local Variables
	bool isValid = false;	//Checks if a valid answer has been chosen
	int attempts = 0;		//Counter for checking how many times user has tried question
	int loopAttempts = 0;	//Used in answer loop to check if not the user answered correctly from list
	std::string userInput = "";	//The user's string input for the question asked of them

	//Keep asking question is question is false
	while (!isValid)
	{
		//Ask question and get user input
		std::cout << question << std::endl;
		for (int i = 0; i < sizeof(options); i++)
		{
			std::cout << options[i] << std::endl;
		}
		std::cout << "Answer: ";
		std::cin >> userInput;

		//Check answer validity
		for (int i = 0; i < sizeof(answer); i++)
		{
			if (userInput == answer[i])
			{
				//If obstacle not a chest continue as normal
				if (!isChest)
				{
					isValid = true;

					//Stat Addition
					player.correctAnswers++;

					std::cout << "That is correct " << player.name << "!" << std::endl << "Press Enter to Continue";
					std::cin.ignore();
					std::cin.get();
					system("cls");

				}
				//If obstacle is the chest congratulate them and end the game
				else
				{
					isValid = true;

					//Stat Addition
					player.correctAnswers++;

					std::cout << "That is correct and you've found the treasure chest!\n"
						<< "Congratulations " << player.name << " you've won!\n\n"
						<< "Press Enter to Continue\n";

					std::cin.ignore();
					std::cin.get();
					system("cls");

					gd.gameComplete = true;
				}
			}
			else
			{
				loopAttempts++;
			}

			//If answer is wrong warn user and give them one more chance
			if (loopAttempts >= sizeof(answer))
			{
				//Adds to attempt counter and resets loopAttempt counter
				attempts++;
				loopAttempts = 0;

				//Tells user to try again and warns them about losing
				if (attempts == 1)
				{
					std::cout << "Sorry that is incorrect. Try again, you have one more try!\nPress Any Key to Retry Question";
					std::cin.ignore();
					std::cin.get();

					//Clear screen for formatting
					system("cls");
				}

				//Max Number of Attempts before game is over
				else if (attempts >= 2)
				{
					std::cout << "Sorry but you ran out of attempts " << player.name << " game over!";
					std::cin.ignore();
					std::cin.get();

					isValid = true;
					gd.gameComplete = true;
				}
			}
		}

		//In case of error reset local variables
		attempts = 0;
		loopAttempts = 0;
		userInput = "";
	}

	return isValid;

	//Clears screen when done with function
	system("cls");
}

//Check if user is trying to move outside of the grid boundary
void gridBoundaryCheck()
{
	//If Y position is greater than grid size, set to min value of grid
	if (player.pos[1] > gd.GRID_Y_MAX)
	{
		player.pos[1] = gd.GRID_Y_MIN;
	}

	//If Y position is less than grid size, set to max value of grid
	if (player.pos[1] < gd.GRID_Y_MIN)
	{
		player.pos[1] = gd.GRID_Y_MAX;
	}

	//If X position is greater than grid size, set to min value of grid
	if (player.pos[0] > gd.GRID_X_MAX)
	{
		player.pos[0] = gd.GRID_X_MIN;
	}

	//If X position is less than grid size, set to max value of grid
	if (player.pos[0] < gd.GRID_X_MIN)
	{
		player.pos[0] = gd.GRID_X_MAX;
	}
}


//////////////////////
// Player Controls ////////////////////////
// Such as Keyboard or Directional Input //
///////////////////////////////////////////


//Moves player N,S,E,W dependent on user input using the W,A,S,D or arrow keys
void movePlayer_keyboard()
{
	//Ask user to move
	std::cout << "Move using the directional keys.\n\nPress 'Q' to Quit Game\n";

	//Local Variables
	bool validInput = false;
	char input = _getch();
	const int VALIDKEYS_SIZE = 10;
	char validKeys[VALIDKEYS_SIZE] = { 'w','a','s','d', KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT ,'`', EXIT };

	//Formatting
	system("cls");

	//Move depending on keypress
	for (int i = 0; i < sizeof(validKeys); i++)
	{

		if (input == validKeys[i])
		{
			//Go Up (North)
			if (input == 'w' || input == KEY_UP)
			{
				player.pos[1]++;
				player.dir_moved[0]++;
				gd.turnsTaken++;
			}

			//Go Down (South
			if (input == 's' || input == KEY_DOWN)
			{
				player.pos[1]--;
				player.dir_moved[1]++;
				gd.turnsTaken++;
			}

			//Go East (Left)
			if (input == 'a' || input == KEY_LEFT)
			{
				player.pos[0]--;
				player.dir_moved[2]++;
				gd.turnsTaken++;
			}

			//Go West (Right)
			if (input == 'd' || input == KEY_RIGHT)
			{
				player.pos[0]++;
				player.dir_moved[3]++;
				gd.turnsTaken++;
			}

			//Enter Debug Mode
			if (input == '`')
			{
				gd.DebugMode = true;
			}

			//Exits the Game
			if (input == EXIT)
			{
				gd.gameComplete = true;
			}

		}
	}
}


//Moves player based on compass directions via string input
void movePlayer_directions()
{
	//Local Variables
	bool validInput = false;
	std::string input = "";

	//Text Input Variables
	const int VALID_DIR_SIZE = 24;
	const std::string VALID_DIRECTIONS[VALID_DIR_SIZE] = { "n", "N", "north", "North", "S", "s", "south", "South", "e", "E", "east", "East", "w", "W", "west", "West", "q", "Q", "quit", "Quit", "d", "D", "debug", "Debug" };
	int dirValue = -1;

	//Ask user to move
	std::cout << "Move in a direction: ";
	std::cin >> input;
	system("cls");

	//Determine is user input is valid or not
	while (!validInput)
	{
		//Check if input has any valid directions given
		for (int i = 0; i < VALID_DIR_SIZE; i++)
		{
			//If input is valid set validInput to true and set 
			if (input == VALID_DIRECTIONS[i])
			{
				dirValue = i;
				validInput = true;
			}
		}

		//If the input does not match any of the valid predefined directions tell user to enter direction again
		if (validInput == false)
		{
			//Formatting
			system("cls");

			//Tell user direction is invalid and asks them again for direction
			std::cout << "Sorry but '" << input << "' is not a valid direction try entering: N,S,E,W. \n"
				<< "Move in a direction: ";
			std::cin >> input;

			//Formatting
			system("cls");
		}
	}

	//If input is valid move the player and check for obstacle
	//If north move player up 1 on Y Axis
	if (dirValue == 0 || dirValue == 1 || dirValue == 2 || dirValue == 3)
	{
		player.pos[1]++;
		player.dir_moved[0]++;
		gd.turnsTaken++;
		gridBoundaryCheck();
	}

	//If south move player up -1 on Y Axis
	if (dirValue == 4 || dirValue == 5 || dirValue == 6 || dirValue == 7)
	{
		player.pos[1]--;
		player.dir_moved[1]++;
		gd.turnsTaken++;
		gridBoundaryCheck();

	}

	//If east move player up 1 on X Axis
	if (dirValue == 8 || dirValue == 9 || dirValue == 10 || dirValue == 11)
	{
		player.pos[0]++;
		player.dir_moved[2]++;
		gd.turnsTaken++;
		gridBoundaryCheck();

	}

	//If west move player up 1 on X Axis
	if (dirValue == 12 || dirValue == 13 || dirValue == 14 || dirValue == 15)
	{
		player.pos[0]--;
		player.dir_moved[3]++;
		gd.turnsTaken++;
		gridBoundaryCheck();

	}

	//If quit option chosen quit the game
	if (dirValue == 16 || dirValue == 17 || dirValue == 18 || dirValue == 19)
	{
		gd.gameComplete = true;
	}

	//If debug option chosen go to debug mode
	if (dirValue >= 20)
	{
		gd.DebugMode = true;
	}

}



///////////////////////////
// Generation Functions /////////////////////////////////////////////////
// Any function that relates to generating anything within the program //
/////////////////////////////////////////////////////////////////////////

//Generates random number and returns it as an integer
int random(int min, int max)
{
	//Generate a random number from rand()
	int num = (rand() % max + min);

	return num;
}

//Used to update Game Data's positions vector (in case grid size changes)
void populate_positions()
{
	int current_x = 0;
	int current_y = 0;

	//Add point to Game Data's positions vector
	for (int count = 0; count < (gd.WIDTH * gd.HEIGHT); ++count)
	{
		gd.positions.push_back({ current_x,current_y });

		//Adds 1 to X value until width is hit, then reset to 0
		if (current_x < gd.WIDTH)
		{
			current_x++;

			//Add 1 to Y value until height is hit
			if (current_y < gd.HEIGHT && current_x >= gd.WIDTH)
			{
				current_x = 0;
				current_y++;
			}
		}
	}
}

//Used to update the Game Data's positions vector for obstacles or the chest on the grid (in case grid size changes)
void populate_gridObjects()
{
	//Local Variables
	bool isComplete = false;
	int amountOfObjects = random(gd.OB_MIN, gd.OB_MAX);
	std::vector<int> newPos = {};

	//Initialize GameData's GridObject Vector Size
	gd.gridObjects.reserve(amountOfObjects);

	//Add point to Game Data's positions vector
	for (int i = 0; i < amountOfObjects; i++)
	{
		//For first position generate as normal (first object is always the chest)
		if (i == 0)
		{
			newPos = { random(0, gd.WIDTH),random(0,gd.HEIGHT) };
			gd.gridObjects.push_back(newPos);
			gd.gridObjectCompletion.push_back(isComplete);
		}
		//Otherwise generate new position and check if it already exists
		else
		{
			//Generate new position
			newPos = { random(0, gd.WIDTH), random(0, gd.HEIGHT) };

			//Keep Checking if position is in vector or not, to avoid obstacle overlap
			for (int j = 0; j < (signed int)gd.gridObjects.size(); j++)
			{
				//Keep generating new positions if they match existing positions
				while (newPos == gd.gridObjects[j])
				{
					newPos = { random(0, gd.WIDTH), random(0, gd.HEIGHT) };
				}
			}

			//Once new position is unique add to vector
			gd.gridObjects.push_back(newPos);
			gd.gridObjectCompletion.push_back(isComplete);
		}

	}
}

//Used for drawing a grid item
std::string drawItem(int width, std::string decoration, bool endRow)
{
	//Counter for adding new items
	int counter = 0;
	std::string row = "";

	//Adds multiple open rows dependent on width
	while (counter < width)
	{
		row += decoration;
		counter++;
	}

	//Adds + endcap and starts new row
	if (endRow)
	{
		row += "+\n";
	}

	counter = 0;

	//Return Row to be added to grid
	return row;
}

//Creates grid from set size
void createGrid()
{
	//Strings for drawing
	//O - Player | # - Obstacles
	std::string grid = "";
	std::string openRow = "+    ";
	std::string closedRow = "+----";
	std::string playerRow = "+ () ";

	//Measurement Variables
	int area = (gd.WIDTH * gd.HEIGHT);
	int currentX = gd.WIDTH - 1;
	int currentY = 0;
	int currentPos = 0;
	int rowCounter = 0;

	//Bottom Row Decoration
	grid += drawItem(gd.WIDTH, closedRow, true);

	//Main Grid
	for (int i = 0; i < area; i++)
	{
		//Fixes grid order reading (From Left-Right-Down normal reading to Grid System of Left-Right-Up)
		//-1 Is for array reading since currentPos starts with 1, while arrays/vectors start at 0
		currentPos = (area - ((currentY * gd.WIDTH) + currentX)) - 1;

		//Adds new row when WIDTH is reached
		if (rowCounter >= gd.WIDTH)
		{
			grid += "+\n";
			rowCounter = 0;

			//Draw Decoration Row
			grid += drawItem(gd.WIDTH, closedRow, true);
		}

		//Check where player is
		if (player.pos == gd.positions[currentPos])
		{
			grid += drawItem(1, playerRow, false);
		}
		else
		{
			grid += drawItem(1, openRow, false);
		}

		//If X is less than width add by 1 until it reaches the Width value
		if (currentX <= 0)
		{
			currentX = gd.WIDTH - 1;

			//Once X is done restarting go down a row
			if (currentY <= gd.HEIGHT)
			{
				currentY++;
			}
		}
		else
		{
			currentX--;
		}

		//Row Counter
		rowCounter++;
	}

	//End Last Row before decoration row
	grid += "+\n";

	//Top Decoration Row
	grid += drawItem(gd.WIDTH, closedRow, true);

	//Draws grid on-screen
	std::cout << grid;
}


//////////////////////
// Debug Functions /////////////////////////////////////
// Ingame functions to test if certain functions work //
////////////////////////////////////////////////////////

void DEBUG()
{
	//Local Variables
	std::string input = "";
	const int VALID_INPUT_SIZE = 13;
	std::string validInputs[VALID_INPUT_SIZE] = { "Help", "help", "GetPos", "GetStats", "GetObjects", "Exit", "exit", "cls", "CLS", "q", "Q", "quit", "Quit" };
	std::cin.ignore();

	//Formatting
	std::cout << "Debug Mode: (Type help for command list)\n";

	//Loop for Debug Mode Input
	while (gd.DebugMode)
	{
		//Ask user for Input
		std::cout << "Input: ";
		std::cin >> input;
		std::cout << std::endl;

		//Setup Bool
		bool validChoice = false;

		//Checks if input is valid or not
		while (!validChoice)
		{
			for (int i = 0; i < VALID_INPUT_SIZE; i++)
			{
				if (input == validInputs[i])
				{
					validChoice = true;

					if (input == validInputs[0] || input == validInputs[1])
					{
						std::cout << "Usable Commands: \n"
							<< "GetPos - Shows all the positions on the grid\n"
							<< "GetStats - Shows all of the player's info\n"
							<< "GetObjects - Shows the positions of every obstacle or chest on the grid\n"
							<< "CLS or cls - Clears the console screen\n"
							<< "Exit - Exits debug mode\n\n";
					}

					if (input == validInputs[2])
					{
						DEBUG_GetPosList();
					}

					if (input == validInputs[3])
					{
						DEBUG_GetPlayerInfo();
					}

					if (input == validInputs[4])
					{
						DEBUG_GetObjects();
					}

					if (input == validInputs[5] || input == validInputs[6])
					{
						system("cls");
						gd.DebugMode = false;
					}

					if (input == validInputs[7] || input == validInputs[8])
					{
						system("cls");
						std::cout << "Debug Mode: (Type help for command list)\n";
					}

					if (input == validInputs[9] || input == validInputs[10] || input == validInputs[11] || input == validInputs[12])
					{
						system("cls");
						gd.DebugMode = false;
						gd.gameComplete = true;
					}
				}
			}

			if (!validChoice)
			{
				std::cout << "\"" << input << "\" is not a valid input, try typing \"help\" for more info.\nInput: ";
				std::cin >> input;
				std::cout << std::endl;
			}
		}

		//Stops Debug from exiting automatically
		std::cin.ignore();
	}
}z

void DEBUG_GetObjects()
{
	//Formatting
	std::cout << std::endl << "Object Positions: " << std::endl;

	//Displaying Positions
	for (int i = 0; i < (signed int)gd.gridObjects.size(); i++)
	{
		std::cout << "(" << gd.gridObjects[i][0] << "," << gd.gridObjects[i][1] << ")\n";
	}

	//Formatting
	std::cout << std::endl;
}

void DEBUG_GetPosList()
{
	//Formatting
	std::cout << std::endl << "Grid Positions: " << std::endl;

	//Displaying Positions
	for (int i = 0; i < (signed int)gd.positions.size(); i++)
	{
		std::cout << "(" << gd.positions[i][0] << "," << gd.positions[i][1] << ")\n";
	}

	//Formatting
	std::cout << "Vector Size: " << gd.positions.size() << std::endl;
}

void DEBUG_GetPlayerInfo()
{
	//Player Info
	std::cout << "\nPlayer Info: \n"
		<< "Player Name: " << player.name << std::endl
		<< "Times Moved North " << player.dir_moved[0] << " times.\n"
		<< "Times Moved South " << player.dir_moved[1] << " times.\n"
		<< "Times Moved East  " << player.dir_moved[2] << " times.\n"
		<< "Times Moved West  " << player.dir_moved[3] << " times.\n"
		<< "Number of Correct Answers: " << player.correctAnswers;

	//Formatting
	std::cout << std::endl;
}