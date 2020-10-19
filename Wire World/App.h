#pragma once
#include "Wire_World.h"

class App {
public:
	App(const std::string &appName, int width, int height);
	~App();
	void run();

private:
	void pollEvent(sf::Event& ev);
	void render();
	void display();
	void clear(sf::Color color);

private:
	Wire_World wire_world_;
	sf::RenderWindow* window_;
};