#include "Screen.h"
#include "Event.h"
#include "Button.h"
#include "Note.h"


//CONSTRUCTORS
IScreen::IScreen(sf::RenderWindow& window, eGamestate state) : _window(window), _state(state)
{
	this->_index = all_screens.size();
	this->_window.setFramerateLimit(120);
}

MenuScreen::MenuScreen(sf::RenderWindow& window) : IScreen(window, MENU)
{
	std::cout << std::endl << "Creating menu screen" << std::endl;
	this->_events.push_back(new WindowDefaultEvent); // Event handler for options, close window, etc.
	this->_events.push_back(new MenuEvent); // Update menu, draw it and react in terms of user inputs.

	sf::Vector2f	win_size(window.getSize());

	this->_buttons.push_back(new Button(GAME_NAME, win_size.y / 4.f, sf::Vector2f(
		win_size.x / 2.f,
		win_size.y / 20.f), CENTER));
	this->_buttons.push_back(new Button("New Game", win_size.y / 6.f, sf::Vector2f(
		win_size.x / 2.f,
		win_size.y / 2.5f), CENTER));
	this->_buttons.push_back(new Button("Options", win_size.y / 6.f, sf::Vector2f(
		win_size.x / 2.f,
		win_size.y / 1.6f), CENTER));
	this->_buttons.push_back(new Button("Exit", win_size.y / 12.f, sf::Vector2f(
		win_size.x / 2.f,
		win_size.y / 1.15f), CENTER));
	this->_buttons[1]->onClick(&IEvent::changeScreen, this->_events[1], GAME, static_cast<IScreen *>(this));
	this->_buttons[2]->onClick(&IEvent::changeScreen, this->_events[1], OPTIONS, static_cast<IScreen *>(this));
	this->_buttons[3]->onClick(&IEvent::changeScreen, this->_events[1], EXIT, static_cast<IScreen *>(this));
}

GameScreen::GameScreen(sf::RenderWindow& window) :  IScreen(window, GAME)
{
	std::cout << std::endl << "Creating game screen" << std::endl;
	this->_events.push_back(new WindowDefaultEvent); // Event handler for options, close window, etc.
	this->_events.push_back(new GameEvent); // Update game, draw it and react in terms of user inputs.

	sf::Vector2f		win_size(window.getSize());
	sf::VertexArray		va_tmp(sf::LinesStrip, 2);

	va_tmp[0].color = va_tmp[1].color = sf::Color::Green;
	va_tmp[0].position = sf::Vector2f(win_size.x / 2.f, 0);
	va_tmp[1].position = sf::Vector2f(win_size.x / 2.f, win_size.y);
	this->_cross.push_back(va_tmp);
	va_tmp[0].position = sf::Vector2f(0, win_size.y / 2.f);
	va_tmp[1].position = sf::Vector2f(win_size.x, win_size.y / 2.f);
	this->_cross.push_back(va_tmp);
}

IScreen::~IScreen()
{
	std::cout << "Deleting window events" << std::endl;
	for (std::vector<IEvent *>::const_iterator it = this->_events.begin(); it != this->_events.end(); ++it)
		delete(*it);
}

MenuScreen::~MenuScreen()
{
	std::cout << "Deleting menu screen" << std::endl;
	for (std::vector<Button *>::const_iterator it = this->_buttons.begin(); it != this->_buttons.end(); ++it)
		delete (*it);
}

GameScreen::~GameScreen()
{
	std::cout << "Deleting game screen" << std::endl;
}


//GETTERS
sf::RenderWindow&		IScreen::getWindow()
{
	return (this->_window);
}

std::vector<IEvent *>&	IScreen::getEvents()
{
	return (this->_events);
}

eGamestate				IScreen::getState() const
{
	return (this->_state);
}

const unsigned int		IScreen::getIndex() const
{
	return (this->_index);
}

std::vector<Button *>&	MenuScreen::getButtons()
{
	return (this->_buttons);
}

const std::vector<sf::VertexArray>&	GameScreen::getCross() const
{
	return (this->_cross);
}


//METHODS
int		IScreen::run()
{
	int			status;
	sf::Event	event;

	while (this->_window.pollEvent(event))
	{
		for (std::vector<IEvent *>::const_iterator it = this->_events.begin(); it != this->_events.end(); ++it)
		{
			if ((status = (*it)->update(*this, event)) != this->_index)
				return (status);
		}
	}

	this->_window.clear();
	for (std::vector<IEvent *>::const_iterator it = this->_events.begin(); it != this->_events.end(); ++it)
		(*it)->draw(*this);
	this->_window.display();

	return (this->_index);
}

void	IScreen::draw(const sf::Drawable& object)
{
	this->_window.draw(object);
}