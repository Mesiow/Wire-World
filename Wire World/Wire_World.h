#pragma once
#include <cstdlib>
#include <random>
#include <array>
#include <vector>
#include "olcPixelGameEngine.h"

enum class Mode {
	LINE = 0, //placing line of conductor cells
	SINGLE, //Placing single conductor cell
	HEAD, //Placing electron head
	RUNNING, //Sim is running, can't place cells during this mode
	PAUSED
};

enum class State {
	EMPTY = 0, //Black
	HEAD, //blue
	TAIL, //red
	CONDUCTOR //yellow
};

struct Cell {
	State state;
	int posX, posY; //x and y position in screen
};

struct Position {
	int x, y;
};

//input typedefs
using key = olc::Key;
//using mouse = sf::Mouse::Button;

class Wire_World {
public:
	Wire_World();
	~Wire_World();
	void render(olc::PixelGameEngine *pge);
	void pollInput(olc::PixelGameEngine* pge);


private:
	//set cell color based on position and state
	void setCellColor(int x, int y, State state);
	void placeCell(int x, int y);
	//place conductor cells from startX to endX at current Y position clicked on
	void placeConductorCellsFrom(int startX, int endX, int startY, int endY);
	//sets cell to empty
	void removeCell(int x, int y);
	void setCell(int x, int y, Cell cell);

	std::string getModeAsString(Mode mode);

	//rule functions
	void runRules();
	//get state of a neighbor
	State getNeighbor(int x, int y);


private:
	//allows for 10 x 10 cells
	//divide screen width by 10(cell size) to get grid width(128) and height(72)
	static constexpr int width = 256;
	static constexpr int height = 240;
	std::vector<Cell> cells_;
	int cellSize_ = 10; //10x10 cell size
	
	//Conductor line
	int startX = 0, endX = 0; //start x and end x of a conductor line
	int startY = 0, endY = 0;

	bool simRunning_ = false;
	Mode currentMode_; //current mode of the simulation
};