#pragma once
#include "Wire_World.h"



// Override base class with your custom functionality
class App : public olc::PixelGameEngine
{
public:
	App()
	{
		// Name you application
		sAppName = "Wire World";
	}

public:
	bool OnUserCreate() override
	{
		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		wireworld_.pollInput(this);
		wireworld_.render(this);

		return true;
	}

private:
	Wire_World wireworld_;
};
