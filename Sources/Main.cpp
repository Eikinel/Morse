#include <SFML\Graphics.hpp>
#include "Screen.h"

std::vector<IScreen *>	all_screens;

int		main()
{
	sf::RenderWindow	window(sf::VideoMode(
		sf::VideoMode::getDesktopMode().width / 1.25f, sf::VideoMode::getDesktopMode().height / 1.25f),
		GAME_NAME,
		sf::Style::Close);

	// Pushing screens to the stack
	all_screens.push_back(new MenuScreen(window));

	// Run every elements in the stack. If there's no more screen left or a close request is done, break the loop.
	int status = MENU;
	while ((status = all_screens[status]->run()) != EXIT && status < all_screens.size());

	// On exit request, delete all screens properly
	for (auto it : all_screens)
	{
		std::cout << "Delete screen n°" << it->getIndex() << " : " << std::endl;
		delete(it);
	}
	return (status);
}