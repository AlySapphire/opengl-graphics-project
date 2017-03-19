#include "LandscapeApp.h"

int main() {
	
	auto app = new LandscapeApp();
	app->run("SHADERZZZZZ", 1280, 720, false);
	delete app;

	return 0;
}