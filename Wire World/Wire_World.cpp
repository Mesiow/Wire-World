#include "Wire_World.h"
#include <iostream>

Wire_World::Wire_World()
{
	cells_ = new Cell[width * height];
	srand(time(0));
	//init cells
	for (int i = 0; i < width * height; i++) {
		//int randState = rand() % (int)State::CONDUCTOR + 1; //0 - 3
		//std::cout << randState << std::endl;
		//cells_[i].state = (State)(randState);
		cells_[i].state = State::EMPTY;
		cells_[i].rect = sf::RectangleShape(sf::Vector2f(cellSize_, cellSize_));
	}

	////add cells to the grid in screen space
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * width + x);
			//1. set position in the world
			cells_[index].posX = x * cellSize_;
			cells_[index].posY = y * cellSize_;

			int posX = cells_[index].posX;
			int posY = cells_[index].posY;

			cells_[index].rect.setPosition(sf::Vector2f(posX, posY));

			//2. set each cells color based on its state
			setCellColor(cells_[index], cells_[index].state);
		}
	}
}

Wire_World::~Wire_World()
{
	
}

void Wire_World::render(sf::RenderTarget* target)
{
	for (int i = 0; i < width * height; i++) {
		target->draw(cells_[i].rect);
	}
}

void Wire_World::pollInput(sf::RenderWindow &window, sf::Event &ev)
{
	switch (ev.type) {
		case sf::Event::MouseButtonReleased:{
			sf::Mouse::Button button = ev.mouseButton.button;

			int posX = sf::Mouse::getPosition(window).x;
			int posY = sf::Mouse::getPosition(window).y;
			//clamp mouse coords so they can't be negative
			if (posX <= 0)
				posX = 0;
			if (posY <= 0)
				posY = 0;

			if (posX >= 1280)
				posX = 1280;
			if (posY >= 720)
				posY = 720;
			
			//Left mouse button released
			if(button == sf::Mouse::Left){
				//if click counter greater or equal to 2, call placeConductorCellsFrom
				clickCounter++;
				//place single conductor cell
				if (clickCounter % 2 != 0) {
					startX = posX;
					startY = posY;
					placeConductorCell(posX, posY);
				} //every 2nd click
				else if (clickCounter % 2 == 0) { //place multiple cells from start position x to end position x
					clickCounter = 0;
					endX = posX;
					endY = posY;
					placeConductorCellsFrom(startX, endX, startY, endY);
				}
			}
			//Right mouse button released
			else if (button == sf::Mouse::Right) {
				removeConductorCell(posX, posY);
			}
		}
		break;
	}
}

void Wire_World::setCellColor(Cell& cell, State state)
{
	sf::Color color;

	switch (state) {
		case State::EMPTY: color = sf::Color::Black; break;
		case State::HEAD: color = sf::Color::Blue; break;
		case State::TAIL: color = sf::Color::Red; break;
		case State::CONDUCTOR: color = sf::Color::Yellow; break;
	}
	cell.rect.setFillColor(color);
}

void Wire_World::placeConductorCell(int x, int y)
{
	//convert to grid space to index proper cell in array
	int gridX = (x / cellSize_);
	int gridY = (y / cellSize_);
	int index = (gridY * width + gridX);

	//clamp index
	if (index >= width * height)
		index = width * height;

	cells_[index].state = State::CONDUCTOR;
	setCellColor(cells_[index], cells_[index].state);
}

void Wire_World::placeConductorCellsFrom(int startX, int endX, int startY, int endY)
{
	int gridX = (startX / cellSize_);
	int gridEndX = (endX / cellSize_);
	int gridY = (startY / cellSize_);
	int gridEndY = (endY / cellSize_);

	auto placeCellInGrid = [&](int x, int y) {
		int index = (y * width + x);
		cells_[index].state = State::CONDUCTOR;
		setCellColor(cells_[index], cells_[index].state);
	};
	
	// Y
	//add cells to height in positive direction y (down)
	if (gridY < gridEndY) {
		for (int y = gridY; y <= gridEndY; y++) {
			placeCellInGrid(gridX, y);
		}
	}
	//add cells to height in negative direction y (up)
	else if (gridY > gridEndY) {
		for (int y = gridY; y >= gridEndY; y--) {
			placeCellInGrid(gridX, y);
		}
	}

	// X
	//add cells to width in positive direction x(right)
	if (gridX < gridEndX) {
		for (int x = gridX; x <= gridEndX; x++) {
			placeCellInGrid(x, gridEndY);
		}
	}
	//add cells to width in negative direction x (left)
	else if (gridX > gridEndX) {
		for (int x = gridX; x >= gridEndX; x--) {
			placeCellInGrid(x, gridEndY);
		}
	}
	
}

void Wire_World::removeConductorCell(int x, int y)
{
	//convert to grid space to index proper cell in array
	int gridX = (x / cellSize_);
	int gridY = (y / cellSize_);
	int index = (gridY * width + gridX);

	//clamp index
	if (index >= width * height)
		index = width * height;

	cells_[index].state = State::EMPTY;
	setCellColor(cells_[index], cells_[index].state);
}
