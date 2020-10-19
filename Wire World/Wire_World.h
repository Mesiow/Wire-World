#pragma once
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <random>
#include <array>

enum class State {
	EMPTY = 0, //Black
	HEAD, //blue
	TAIL, //red
	CONDUCTOR //yellow
};

struct Cell {
	State state;
	sf::RectangleShape rect;
	int posX, posY; //x and y position in screen
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
	void placeConductorCell(int x, int y);
	//place conductor cells from startX to endX at current Y position clicked on
	void placeConductorCellsFrom(int startX, int endX, int startY, int endY);
	//sets cell to empty
	void removeConductorCell(int x, int y);

private:
	//allows for 16 x 16 cells
	static constexpr int width = 80;
	static constexpr int height = 45;
	Cell *cells_;
	int cellSize_ = 16; //16x16 cell size

	int clickCounter = 0;
	
	int startX = 0, endX = 0; //start x and end x of a conductor line
	int startY = 0, endY = 0; 
};