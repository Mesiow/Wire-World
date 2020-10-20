#include "App.h"

int main() {
	App app;
	if (app.Construct(1280, 720, 1, 1))
		app.Start();

	return 0;
}