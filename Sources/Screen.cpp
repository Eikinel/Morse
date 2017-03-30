#include "Screen.h"
#include "Event.h"
#include "Button.h"
#include "Note.h"
#include "Skin.h"


//CONSTRUCTORS
IScreen::IScreen(sf::RenderWindow& window, eGamestate state) : _window(window), _state(state)
{
	sf::Vector2u	win_size(window.getSize());

	this->_index = all_screens.size();
	this->_frame_limiter = 120;
	this->_fps = 0.f;
	if (!this->_main_font.loadFromFile(FONTS_DIR"/Meatloaf Sketched.ttf"))
		std::cerr << "Failed to load font " << FONTS_DIR"/Meatloaf Sketched.ttf" << std::endl;
	this->_window.setFramerateLimit(this->_frame_limiter);
	this->_fps_text.setFont(this->_main_font);
	this->_fps_text.setCharacterSize(win_size.y / 25.f);
	this->_fps_text.setString(std::to_string(this->_fps) + " / " + std::to_string(this->_frame_limiter));
	this->_fps_text.setPosition(sf::Vector2f(
		win_size.x / 1.07f,
		win_size.y / 1.05f));
	this->_clock.restart().asSeconds();
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
	this->_buttons.push_back(new Button("Play", win_size.y / 6.f, sf::Vector2f(
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
	this->_speed = 1000;
	this->_skin = new Skin();
	this->_notes.push_back(new Note(sf::seconds(1.f), 0,
		this->_skin->getComponent(eSkinComponent::SK_NOTE),
		this->_skin->getComponent(eSkinComponent::SK_NOTE_OUTLINE)));
	this->_notes.push_back(new Note(sf::seconds(2.f), 0,
		this->_skin->getComponent(eSkinComponent::SK_NOTE),
		this->_skin->getComponent(eSkinComponent::SK_NOTE_OUTLINE)));
	this->_notes.push_back(new Note(sf::seconds(3.3f), 0,
		this->_skin->getComponent(eSkinComponent::SK_NOTE),
		this->_skin->getComponent(eSkinComponent::SK_NOTE_OUTLINE)));
	this->_notes.push_back(new Note(sf::seconds(3.6f), 0,
		this->_skin->getComponent(eSkinComponent::SK_NOTE),
		this->_skin->getComponent(eSkinComponent::SK_NOTE_OUTLINE)));
	this->_notes.push_back(new Note(sf::seconds(5.f), 0,
		this->_skin->getComponent(eSkinComponent::SK_NOTE),
		this->_skin->getComponent(eSkinComponent::SK_NOTE_OUTLINE)));
	this->_cursor.setTexture(this->_skin->getComponent(eSkinComponent::SK_CURSOR));
	this->_cursor.setOrigin(sf::Vector2f(
		this->_cursor.getGlobalBounds().width / 2.f,
		this->_cursor.getGlobalBounds().height / 2.f));
	this->_cursor.setPosition(sf::Vector2f(win_size.x / 2.f, win_size.y / 2.f));
}

IScreen::~IScreen()
{
	std::cout << "Deleting window events" << std::endl;
	for (auto it : this->_events)
		delete(it);
}

MenuScreen::~MenuScreen()
{
	std::cout << "Deleting menu screen" << std::endl;
	for (auto it : this->_buttons)
		delete (it);
}

GameScreen::~GameScreen()
{
	std::cout << "Deleting game screen" << std::endl;
	delete(this->_skin);
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

const float	IScreen::getFPS() const
{
	return (this->_fps);
}

const sf::Text&	IScreen::getFPSText() const
{
	return (this->_fps_text);
}

const unsigned int	IScreen::getFrameLimiter() const
{
	return (this->_frame_limiter);
}


std::vector<Button *>&	MenuScreen::getButtons()
{
	return (this->_buttons);
}


const std::vector<sf::VertexArray>&	GameScreen::getCross() const
{
	return (this->_cross);
}

const std::vector<Note *>&	GameScreen::getNotes() const
{
	return (this->_notes);
}

const Note&	GameScreen::getNextNote(int index) const
{
	return (*this->_notes[index]);
}

const Skin&	GameScreen::getSkin() const
{
	return (*this->_skin);
}

const unsigned int	GameScreen::getSpeed() const
{
	return (this->_speed);
}

const sf::Sprite&	GameScreen::getCursor() const
{
	return (this->_cursor);
}


//SETTERS
void	IScreen::updateFPS()
{
	float			current_time;
	static sf::Time	freeze = sf::seconds(1.f);

	freeze += this->_clock.getElapsedTime();
	current_time = this->_clock.restart().asSeconds();

	this->_fps = 1.f / current_time;
	if (freeze.asSeconds() >= 1.f)
	{
		this->_fps_text.setString(sf::String(std::to_string((int)std::round(this->_fps)) + " / " + std::to_string(this->_frame_limiter)));
		freeze = sf::Time::Zero;
	}
}

void	IScreen::setFrameLimiter(const unsigned int frame_limiter)
{
	this->_frame_limiter = frame_limiter;
}


//METHODS
int		IScreen::run()
{
	int					status;
	sf::Event			event;

	while (this->_window.pollEvent(event))
	{
		for (auto it : this->_events)
		{
			// Don't repeat the main Update Event !
			if ((status = it->update(*this, event)) != this->_index)
				return (status);
		}
	}

	this->_window.clear();
	for (auto it : this->_events)
		it->draw(*this);
	this->_window.display();

	this->updateFPS();
	return (this->_index);
}

int		GameScreen::run()
{
	int				status;
	sf::Event		event;

	this->_events[1]->update(*this, sf::Event());

	while (this->_window.pollEvent(event))
	{
		for (auto it : this->_events)
		{
			// Don't repeat the main Update Event !
			if (it != this->_events[1] && (status = it->update(*this, event)) != this->_index)
				return (status);
		}
	}

	this->_window.clear();
	for (auto it : this->_events)
		it->draw(*this);
	this->_window.display();

	this->updateFPS();
	return (this->_index);
}

void	IScreen::draw(const sf::Drawable& object)
{
	this->_window.draw(object);
}