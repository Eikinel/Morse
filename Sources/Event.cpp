#include "Event.h"
#include "Screen.h"
#include "Button.h"
#include "Collider.h"
#include "Skin.h"
#include "Note.h"


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
	this->_game_clock.restart();

	//Convert second to milliseconds for proper timing gaps, then apply calcul
	float	max_timing_view_ms = MAX_TIMING_VIEW * 1000.f;
	this->_timing_gaps.push_back(sf::Vector2f(-max_timing_view_ms / 400.f, max_timing_view_ms / 400.f)); //GREAT
	this->_timing_gaps.push_back(sf::Vector2f(-max_timing_view_ms / 100.f, max_timing_view_ms / 100.f)); //GOOD
	this->_timing_gaps.push_back(sf::Vector2f(-max_timing_view_ms / 50.f, max_timing_view_ms / 50.f)); //BAD
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

	if ((this->_next_notes = gscreen->getNextNotes(this->_game_clock.getElapsedTime())).size() > 0)
	{
		for (auto it : this->_next_notes)
		{
			for (auto it2 : it->getSprites())
				it2->setPosition(sf::Vector2f(
					gscreen->getCursor().getPosition().x + ((it->getTime().asSeconds() - this->_game_clock.getElapsedTime().asSeconds()) * gscreen->getSpeed() * (gscreen->getWindow().getSize().x / (2.f * MAX_TIMING_VIEW))) * it->getDirection().x,
					gscreen->getCursor().getPosition().y + ((it->getTime().asSeconds() - this->_game_clock.getElapsedTime().asSeconds()) * gscreen->getSpeed() * (gscreen->getWindow().getSize().y / (2.f * MAX_TIMING_VIEW))) * it->getDirection().y));
		}

		for (auto it = this->_next_notes.begin(); it != this->_next_notes.end();)
		{
			if ((*it)->getTime().asMilliseconds() - this->_game_clock.getElapsedTime().asMilliseconds() < this->_timing_gaps[this->_timing_gaps.size() - 1].x)
			{
				gscreen->setSpriteAccuracy(eAccuracy::ACC_MISS);
				gscreen->removeNote(**it);
				it = this->_next_notes.erase(it);
			}
			else
				++it;
		}
	}

	switch (event.type)
	{
	case sf::Event::KeyPressed:
		if (this->_next_notes.size() > 0)
		{
			sf::Vector2i	direction = this->_next_notes[0]->getDirection();
			sf::Time		delta_accuracy = sf::seconds(MAX_TIMING_VIEW + 1.f);

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && direction.y == -1)
				delta_accuracy = this->_game_clock.getElapsedTime() - this->_next_notes[0]->getTime();
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && direction.x == -1)
				delta_accuracy = this->_game_clock.getElapsedTime() - this->_next_notes[0]->getTime();
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && direction.y == 1)
				delta_accuracy = this->_game_clock.getElapsedTime() - this->_next_notes[0]->getTime();
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && direction.x == 1)
				delta_accuracy = this->_game_clock.getElapsedTime() - this->_next_notes[0]->getTime();

			if (delta_accuracy < sf::seconds(MAX_TIMING_VIEW + 1.f))
			{
				std::cout << "Delta accuracy : " << delta_accuracy.asMilliseconds() << std::endl;
				gscreen->setSpriteAccuracy(this->getAccuracy(delta_accuracy));
				gscreen->removeNote(*this->_next_notes[0]);
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F3))
				gscreen->addSpeed(-1);
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F4))
				gscreen->addSpeed(1);
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			{
				gscreen->restart();
				this->_game_clock.restart();
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			return (this->changeScreen(eGamestate::MENU, gscreen));

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
	gscreen->draw(gscreen->getCursor());
	gscreen->draw(gscreen->getFPSText());

	for (auto it : this->_next_notes)
		for (auto it2 : it->getSprites())
			gscreen->draw(*it2);
	gscreen->draw(gscreen->getSpriteAccuracy());
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

const eAccuracy	GameEvent::getAccuracy(const sf::Time& delta) const
{
	int	accuracy = eAccuracy::ACC_GREAT;

	for (auto it : this->_timing_gaps)
	{
		if (delta.asMilliseconds() >= it.x && delta.asMilliseconds() <= it.y)
			return ((eAccuracy)accuracy);
		accuracy--;
	}
	return ((eAccuracy)accuracy);
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