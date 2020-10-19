#include "App.h"

App::App(const std::string& appName, int width, int height)
{
	window_ = new sf::RenderWindow(sf::VideoMode(width, height),
		appName, sf::Style::Close);
}

App::~App()
{
	delete window_;
}

void App::run()
{
	sf::Event ev;
	while (window_->isOpen()) {
		pollEvent(ev);
		wire_world_.pollInput(*window_, ev);

		clear(sf::Color(50, 50, 50, 255));

		render();

		display();
	}
}

void App::pollEvent(sf::Event& ev)
{
	while (window_->pollEvent(ev)) {
		switch (ev.type) {
		case sf::Event::Closed: window_->close(); break;
		}
	}
}

void App::render()
{
	wire_world_.render(window_);
}

void App::display()
{
	window_->display();
}

void App::clear(sf::Color color)
{
	window_->clear(color);
}
