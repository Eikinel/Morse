#include <SFML\Graphics.hpp>
#include "Screen.h"
#include "Constants.h"

std::array<std::shared_ptr<IScreen>, 3> all_screens;

int	main()
{
	sf::RenderWindow	window(sf::VideoMode(
		sf::VideoMode::getDesktopMode().width / 2.f, sf::VideoMode::getDesktopMode().height / 2.f),
		GAME_NAME,
		sf::Style::Close);

	// Creates first menu screen
	all_screens[(int)GAMESTATE::GAME] = std::make_shared<GameScreen>(window);

	GAMESTATE state = GAMESTATE::GAME;

	// Run every elements in the stack. If there's no more screen left or a close request is done, break the loop.
	while (all_screens.size() && (state = all_screens[(int)state]->run()) != GAMESTATE::EXIT);

	return (int)state;
}