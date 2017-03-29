#include "Event.h"
#include "Screen.h"
#include "Button.h"
#include "Collider.h"
#include "Skin.h"


//CONSTRUCTORS
IEvent::IEvent()
{
	std::cout << "Creating default options" << std::endl;
	this->_toggle_options.push_back(false); // On/off bounding boxes draw
	this->_general_clock.restart().asSeconds();
}

WindowDefaultEvent::WindowDefaultEvent()
{
	std::cout << "Creating window default event" << std::endl;
}

MenuEvent::MenuEvent()
{
	std::cout << "Creating menu event" << std::endl;
}

GameEvent::GameEvent()
{
	std::cout << "Creating game event" << std::endl;
	this->_clock.restart();
}


//UPDATE & DRAW METHODS
int		WindowDefaultEvent::update(IScreen& screen, sf::Event& event)
{
	switch (event.type)
	{
	case sf::Event::Closed:
		return (EXIT);
		break;
	default:
		break;
	}

	return (screen.getIndex());
}

int		MenuEvent::update(IScreen& screen, sf::Event& event)
{
	MenuScreen*	mscreen = static_cast<MenuScreen *>(&screen);

	switch (event.type)
	{
	case sf::Event::KeyPressed:
		switch (event.key.code)
		{
		case sf::Keyboard::Escape:
			return (EXIT);
			break;
		default:
			break;
		}
		break;
	case sf::Event::MouseButtonReleased:
		std::cout << "MenuEvent : Click !" << std::endl;
		std::cout << "Position : [" << event.mouseButton.x << " ; " << event.mouseButton.y << "]." << std::endl;

		for (auto it : mscreen->getButtons())
		{
			int status = mscreen->getIndex();
				
			if (it->getText().getGlobalBounds().contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y)))
				if ((status = it->triggerEvent()) != screen.getIndex())
					return (status);
		}
		break;
	default:
		break;
	}

	return (screen.getIndex());
}

void		MenuEvent::draw(IScreen& screen)
{
	MenuScreen*	mscreen = static_cast<MenuScreen *>(&screen);

	for (auto it : mscreen->getButtons())
		mscreen->draw(it->getText());
	mscreen->draw(mscreen->getFPSText());
}

int		GameEvent::update(IScreen& screen, sf::Event& event)
{
	GameScreen*	gscreen = static_cast<GameScreen *>(&screen);

	std::cout << "Time: " << this->_clock.getElapsedTime().asSeconds() << std::endl;
	switch (event.type)
	{
	case sf::Event::KeyPressed:
		switch (event.key.code)
		{
		case sf::Keyboard::Escape:
			return (this->changeScreen(eGamestate::MENU, gscreen));
			break;
		default:
			break;
		}
		break;
	case sf::Event::MouseButtonReleased:
		std::cout << "GameEvent : Click !" << std::endl;
		std::cout << "Position : [" << event.mouseButton.x << " ; " << event.mouseButton.y << "]." << std::endl;
		break;
	default:
		break;
	}

	return (screen.getIndex());
}

void		GameEvent::draw(IScreen& screen)
{
	GameScreen*	gscreen = static_cast<GameScreen *>(&screen);

	for (auto it : gscreen->getCross())
		gscreen->draw(it);
	gscreen->draw(gscreen->getFPSText());
}


//GETTERS
bool	IEvent::getToggleBoundingBoxes()
{
	return (this->_toggle_options[0]);
}

std::vector<bool>&	IEvent::getToggleOptions()
{
	return (this->_toggle_options);
}


//SETTERS
void	IEvent::setToggleOptions(const std::vector<bool>& toggle_options)
{
	this->_toggle_options = toggle_options;
}


//OTHER METHODS
int	IEvent::changeScreen(eGamestate gamestate, IScreen* screen)
{
	std::string	type[3] = { "menu", "options", "game" };
	int	index;

	if (gamestate == EXIT)
		return (gamestate);
	
	// Check if a screen with the gamestate passed as parameter already exist.
	// If true, copy options and return the gamestate.
	// Else, create the screen before performing operations.
	for (std::vector<IScreen *>::const_iterator it = all_screens.begin(); it != all_screens.end(); ++it)
	{
		if ((*it)->getState() == gamestate)
		{
			if ((*it)->getEvents()[1]->getToggleOptions() != this->_toggle_options)
			{
				std::cout << "Replacing old options" << std::endl;
				(*it)->getEvents()[1]->setToggleOptions(this->_toggle_options);
			}
			return ((*it)->getIndex());
		}
	}
	index = this->createScreen(gamestate, screen);
	std::cout << "Switching screen to " << type[(int)gamestate] << " at index " << index << "." << std::endl;
	return (index);
}

int	IEvent::createScreen(eGamestate gamestate, IScreen* screen)
{
	std::string	type[3] = { "menu", "options", "game" };
	IScreen*	new_screen = NULL;

	std::cout << std::endl << "Creating new " << type[(int)gamestate] << "." << std::endl;

	if (gamestate == MENU)
		new_screen = new MenuScreen(screen->getWindow());
	else if (gamestate == OPTIONS)
		new_screen = NULL;//new OptionsScreen(*window);
	else
		new_screen = new GameScreen(screen->getWindow());
	new_screen->getEvents()[1]->setToggleOptions(this->_toggle_options);
	all_screens.push_back(new_screen);
	return (new_screen->getIndex());
}

int		IEvent::toggleBoundingBoxes(int index)
{
	this->_toggle_options[0] = !this->_toggle_options[0];
	if (this->_toggle_options[0])
		std::cout << "Showing bounding boxes" << std::endl;
	else
		std::cout << "Hiding bounding boxes" << std::endl;
	return (index);
}