#include "Game.hpp"

#include <iostream>
#include <memory>

int main(int argc, char* argv[])
{
	(void) argc;
	(void) argv;
	
	std::unique_ptr<Game> game = std::make_unique<Game>();
	game->Run();

	return 0;
}
