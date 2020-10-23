#include "Wire_World.h"
#include <iostream>

Wire_World::Wire_World()
{
	cells_.resize(width * height);
	srand(time(0));

	//init cells
	for (int i = 0; i < width * height; i++)
		cells_[i].state = State::EMPTY;

	currentMode_ = Mode::SINGLE;
}

Wire_World::~Wire_World()
{
	
}

void Wire_World::render(olc::PixelGameEngine *pge)
{
	//Render Text Info
	pge->DrawStringDecal(olc::vf2d(10, 10), "Mode: " + getModeAsString(currentMode_),
		olc::WHITE, olc::vf2d(3, 3));

	if(currentMode_ == Mode::RUNNING)
		runRules();

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * width + x);

			switch (cells_[index].state) {
			case State::EMPTY:
				pge->FillRect(x * cellSize_, y * cellSize_,
					cellSize_, cellSize_, olc::BLACK);
				break;

			case State::HEAD:
				pge->FillRect(x * cellSize_, y * cellSize_,
					cellSize_, cellSize_, olc::BLUE);
				break;

			case State::TAIL:
				pge->FillRect(x * cellSize_, y * cellSize_,
					cellSize_, cellSize_, olc::RED);
				break;

			case State::CONDUCTOR:
				pge->FillRect(x * cellSize_, y * cellSize_,
					cellSize_, cellSize_, olc::YELLOW);
				break;
			}
		}
	}
}

void Wire_World::pollInput(olc::PixelGameEngine *pge)
{
	//Keyboard
	if (pge->GetKey(key::SPACE).bReleased) {
		simRunning_ = !simRunning_;
		if (simRunning_) currentMode_ = Mode::RUNNING;
		else currentMode_ = Mode::PAUSED;
	}
	else if (pge->GetKey(key::Q).bReleased) { //Toggle through modes
		int nextMode = ((int)currentMode_ + 1) % (int)Mode::RUNNING;
		currentMode_ = (Mode)nextMode; //set to next mode
	}

	//Mouse Coords
	int posX = pge->GetMousePos().x;
	int posY = pge->GetMousePos().y;
	//clamp mouse coords so they can't be negative
	if (posX <= 0)
		posX = 0;
	if (posY <= 0)
		posY = 0;

	if (posX >= 1280)
		posX = 1280;
	if (posY >= 720)
		posY = 720;

	//Mouse
	//left mouse
	if (pge->GetMouse(0).bReleased) {
		//place single conductor cell
		if (currentMode_ == Mode::SINGLE) {
			startX = posX;
			startY = posY;
			placeCell(posX, posY);
		}
		else if (currentMode_ == Mode::LINE) { //place multiple cells from start position x to end position x
			endX = posX;
			endY = posY;
			placeConductorCellsFrom(startX, endX, startY, endY);
		}
		else if (currentMode_ == Mode::HEAD) {
			placeCell(posX, posY);
		}
	}
	else if (pge->GetMouse(1).bReleased) {
			removeCell(posX, posY);
	}
}


void Wire_World::setCellColor(int x, int y, State state)
{
	//int index = (y * width + x);
	//cells_[index].state = state;
	//setCellColor(cells_[index], cells_[index].state);
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

	auto& cell = cells_[index];
	//place correct cell
	switch (currentMode_) {
		case Mode::SINGLE: cell.state = State::CONDUCTOR; break;
		case Mode::HEAD: cell.state = State::HEAD; break;
	}
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
}

void Wire_World::setCell(int x, int y, Cell cell)
{
	int index = (y * width + x);
	Cell& c = cells_[index];
	c = cell;
}

std::string Wire_World::getModeAsString(Mode mode)
{
	switch (mode) {
		case Mode::LINE: return std::string("Line");
		case Mode::SINGLE: return std::string("Single");
		case Mode::HEAD: return std::string("Electron Head");
		case Mode::RUNNING: return std::string("Running");
		case Mode::PAUSED: return std::string("Paused");
	}
	return "";
}

void Wire_World::runRules()
{
	std::vector<Cell> newCells = cells_;

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
					}
				}
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
		}
	}
	cells_ = newCells;
}

State Wire_World::getNeighbor(int x, int y)
{
	int index = (y * width + x);
	if (index >= width * height)
		index = width * height;

	return cells_[index].state;
}
