#pragma once
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <random>
#include <array>
#include <vector>

enum class State {
	EMPTY = 0, //Black
	HEAD, //blue
	TAIL, //red
	CONDUCTOR //yellow
};

struct Cell {
	Cell(){}
	Cell(State st, sf::RectangleShape rect, int x, int y){
		this->state = st;
		this->rect = rect;
		this->posX = x;
		this->posY = y;
		this->rect.setPosition(x, y);
	}
	State state;
	sf::RectangleShape rect;
	int posX, posY; //x and y position in screen
};

struct Position {
	int x, y;
};

//input typedefs
using key = sf::Keyboard::Key;
using mouse = sf::Mouse::Button;

class Wire_World {
public:
	Wire_World();
	~Wire_World();
	void render(sf::RenderTarget *target);
	void pollInput(sf::RenderWindow &window, sf::Event &ev);


private:
	//set cell color based on state
	void setCellColor(Cell &cell, State state);
	//set cell color based on position and state
	void setCellColor(int x, int y, State state);
	void placeCell(int x, int y);
	//place conductor cells from startX to endX at current Y position clicked on
	void placeConductorCellsFrom(int startX, int endX, int startY, int endY);
	//sets cell to empty
	void removeCell(int x, int y);
	void setCell(int x, int y, Cell cell);
	Position getGridPosition(int x, int y);

	//rule functions
	void runRules();
	//get state of a neighbor
	State getNeighbor(int x, int y);


private:
	//allows for 10 x 10 cells
	//divide screen width by 10(cell size) to get grid width(128) and height(72)
	static constexpr int width = 128;
	static constexpr int height = 72;
	std::vector<Cell> cells_;
	int cellSize_ = 10; //10x10 cell size

	int clickCounter = 0;
	
	//Conductor line
	int startX = 0, endX = 0; //start x and end x of a conductor line
	int startY = 0, endY = 0;

	//once we place an electron head on a conductor, start the simulation (run the rules)
	bool started_ = false;
	bool switchSelection_ = true;
	State selectedCell_ = State::CONDUCTOR; //current cell to place down
};