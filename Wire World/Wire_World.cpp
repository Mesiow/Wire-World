#include "Wire_World.h"
#include <iostream>

Wire_World::Wire_World()
{
	//cells_ = new Cell[width * height];
	cells_.resize(width * height);
	srand(time(0));

	//init cells
	for (int i = 0; i < width * height; i++) {
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
	if(started_)
		runRules();
	for (int i = 0; i < width * height; i++) {
		target->draw(cells_[i].rect);
	}
}

void Wire_World::pollInput(sf::RenderWindow &window, sf::Event &ev)
{
	switch (ev.type) {
		case sf::Event::KeyReleased: {
			if (ev.key.code == key::Space)
				started_ = !started_; //start simulation

			if (ev.key.code == key::Q) { //Electron head selected
				switchSelection_ = !switchSelection_;
				if (switchSelection_)
					selectedCell_ = State::HEAD;
				else
					selectedCell_ = State::CONDUCTOR;
				std::cout << "Switched to " << (int)selectedCell_ << std::endl;
			}
		}
		break;

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
				//Conductor cell selected
				if (selectedCell_ == State::CONDUCTOR) {
					//if click counter greater or equal to 2, call placeConductorCellsFrom
					clickCounter++;
					//place single conductor cell
					if (clickCounter % 2 != 0) {
						startX = posX;
						startY = posY;
						placeCell(posX, posY);
					} //every 2nd click
					else if (clickCounter % 2 == 0) { //place multiple cells from start position x to end position x
						clickCounter = 0;
						endX = posX;
						endY = posY;
						placeConductorCellsFrom(startX, endX, startY, endY);
					}
				}
				else if (selectedCell_ == State::HEAD) {
					placeCell(posX, posY);
				}
			}
			//Right mouse button released
			else if (button == sf::Mouse::Right) {
				if (selectedCell_ == State::CONDUCTOR) {
					removeCell(posX, posY);
				}
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

void Wire_World::setCellColor(int x, int y, State state)
{
	int index = (y * width + x);
	cells_[index].state = state;
	setCellColor(cells_[index], cells_[index].state);
}

void Wire_World::placeCell(int x, int y)
{
	//convert to grid space to index proper cell in array
	int gridX = (x / cellSize_);
	int gridY = (y / cellSize_);
	int index = (gridY * width + gridX);

	//clamp index
	if (index >= width * height)
		index = width * height;

	//place correct cell
	if (selectedCell_ == State::CONDUCTOR)
		cells_[index].state = State::CONDUCTOR;
	else if (selectedCell_ == State::HEAD)
		cells_[index].state = State::HEAD;

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

void Wire_World::removeCell(int x, int y)
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

void Wire_World::setCell(int x, int y, Cell cell)
{
	int index = (y * width + x);
	Cell& c = cells_[index];
	c = cell;
	/*c.posX = (x * cellSize_);
	c.posY = (y * cellSize_);
	c.rect.setPosition(sf::Vector2f(c.posX, c.posY));*/
	setCellColor(x, y, c.state);
}

Position Wire_World::getGridPosition(int x, int y)
{
	Position p;
	int gridX = (x / cellSize_);
	int gridY = (y / cellSize_);

	p.x = gridX;
	p.y = gridY;
	return p;
}

void Wire_World::runRules()
{
	std::vector<Cell> newCells(width * height, Cell(State::EMPTY,
		sf::RectangleShape(sf::Vector2f(cellSize_, cellSize_)), 0, 0));
	//std::vector<Cell> newCells(width * height);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {

			int index = (y * width + x);
			auto& cell = cells_[index];
			auto& newCell = newCells[index];

			switch (cell.state) {
			case State::EMPTY:
				newCell.state = State::EMPTY;
				break;

				//Electron head - turns into an electron tail
			case State::HEAD:
				newCell.state = State::TAIL;
				break;

				//Electron tail - turns into a conductor
			case State::TAIL:
				newCell.state = State::CONDUCTOR;
				break;

				//Conductor - turns into an electron head if 1 or 2 neighbors is an electron head
			case State::CONDUCTOR: {
				int headCount = 0;
				//loop over all neighbors
				for (int lx = -1; lx <= 1; lx++) {
					for (int ly = -1; ly <= 1; ly++) {
						int nx = lx + x;
						int ny = ly + y;

						//make sure in range
						if (nx == -1 || nx == width ||
							ny == -1 || ny == height ||
							(lx == 0 && ly == 0)) {
							continue;
						}
						
						State neighbor = getNeighbor(nx, ny);
						if (neighbor == State::HEAD)
							headCount++;

						/*int index = (ny * width + nx);
						auto cell = cells_[index];

						if (cell.state == State::HEAD)
							headCount++;*/
					}
				}
				if (headCount > 0) { std::cout << "head count: " << headCount << std::endl; }
				if (headCount == 1 || headCount == 2) { //turn into electron head
					newCell.state = State::HEAD;
				}
				else
					newCell.state = State::CONDUCTOR;
				break;
			}

			default:
				break;
			}
			//setCell(x, y, newCell);
			setCellColor(x, y, newCell.state);
		}
	}
	for (auto& cell : newCells) {
		setCell(cell.posX, cell.posY, cell);
	}
}

State Wire_World::getNeighbor(int x, int y)
{
	int index = (y * width + x);
	if (index >= width * height)
		index = width * height;

	return cells_[index].state;
}
