#include "Screen.h"
#include "Event.h"
#include "Button.h"
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

	this->_buttons.push_back(new Button(GAME_NAME, win_size.y / 4.f, this->_main_font, sf::Vector2f(
		win_size.x / 2.f,
		win_size.y / 20.f), CENTER));
	this->_buttons.push_back(new Button("Play", win_size.y / 6.f, this->_main_font, sf::Vector2f(
		win_size.x / 2.f,
		win_size.y / 2.5f), CENTER));
	this->_buttons.push_back(new Button("Options", win_size.y / 6.f, this->_main_font, sf::Vector2f(
		win_size.x / 2.f,
		win_size.y / 1.6f), CENTER));
	this->_buttons.push_back(new Button("Exit", win_size.y / 12.f, this->_main_font, sf::Vector2f(
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
	this->_window.setKeyRepeatEnabled(false);

	sf::Vector2f		win_size(window.getSize());
	sf::VertexArray		va_tmp(sf::LinesStrip, 2);

	va_tmp[0].color = va_tmp[1].color = sf::Color::Cyan;
	va_tmp[0].position = sf::Vector2f(win_size.x / 2.f, 0);
	va_tmp[1].position = sf::Vector2f(win_size.x / 2.f, win_size.y);
	this->_cross.push_back(va_tmp);
	va_tmp[0].position = sf::Vector2f(0, win_size.y / 2.f);
	va_tmp[1].position = sf::Vector2f(win_size.x, win_size.y / 2.f);
	this->_cross.push_back(va_tmp);
	this->_speed = 20;
	this->_accuracy_ratio[eAccuracy::ACC_MISS] = 0.f;
	this->_accuracy_ratio[eAccuracy::ACC_BAD] = 0.3334f;
	this->_accuracy_ratio[eAccuracy::ACC_GOOD] = 0.6667f;
	this->_accuracy_ratio[eAccuracy::ACC_GREAT] = 1.f;
	this->_skin = new Skin();
	this->_cursor.setTexture(this->_skin->getComponent(eSkinComponent::SK_CURSOR));
	this->_cursor.setOrigin(sf::Vector2f(
		this->_cursor.getGlobalBounds().width / 2.f,
		this->_cursor.getGlobalBounds().height / 2.f));
	this->_cursor.setPosition(sf::Vector2f(win_size.x / 2.f, win_size.y / 2.f));
	this->restart();
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

const sf::Font&	IScreen::getMainFont() const
{
	return (this->_main_font);
}


std::vector<Button *>&	MenuScreen::getButtons()
{
	return (this->_buttons);
}


const std::vector<Note *>&	GameScreen::getNotes() const
{
	return (this->_notes);
}

const std::vector<Note *>	GameScreen::getNextNotes(const sf::Time& time) const
{
	std::vector<Note *>		next_notes;

	for (auto it : this->_notes)
	{
		if ((it->getTime().asSeconds() - time.asSeconds()) < MAX_TIMING_VIEW / this->_speed)
			next_notes.push_back(it);
		else
			break;
	}

	return (next_notes);
}

const std::vector<Note *>	GameScreen::getNotesWithSameTiming(const sf::Time& time) const
{
	std::vector<Note *>		same_timing;

	for (auto it : this->_notes)
		if (it->getTime() == time)
			same_timing.push_back(it);

	return (same_timing);
}

const Note&	GameScreen::getNoteByIndex(unsigned int index) const
{
	return (*this->_notes[index]);
}

const unsigned int	GameScreen::getNotesSize() const
{
	return (this->_notes_size);
}

const std::vector<sf::VertexArray>&	GameScreen::getCross() const
{
	return (this->_cross);
}

const unsigned int	GameScreen::getSpeed() const
{
	return (this->_speed);
}

const float	GameScreen::getUserAccuracy() const
{
	return (this->_user_accuracy);
}

const Skin&	GameScreen::getSkin() const
{
	return (*this->_skin);
}

const sf::Sprite&	GameScreen::getCursor() const
{
	return (this->_cursor);
}

const sf::Sprite&	GameScreen::getSpriteAccuracy() const
{
	return (this->_sprite_accuracy);
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

void	GameScreen::removeNote(const Note& note)
{
	for (auto it = this->_notes.begin(); it != this->_notes.end(); ++it)
	{
		if (**it == note)
		{
			delete (*it);
			this->_notes.erase(it);
			std::cout << "Note deleted" << std::endl;
			return;
		}
	}
	std::cerr << "Note not found" << std::endl;
}

void	GameScreen::addSpeed(const int offset)
{
	if (this->_speed + offset > 0 && this->_speed < MAX_SPEED)
	{
		std::cout << "Changing speed to " << this->_speed + offset << std::endl;
		this->_speed += offset;
	}
	else
		std::cerr << "Can't change speed : floor or ceiling reached" << std::endl;
}

void	GameScreen::setSpriteAccuracy(const eAccuracy accuracy)
{
	this->_sprite_accuracy.setTexture(this->_skin->getComponent((eSkinComponent)(accuracy + eSkinComponent::SK_MISS)));
	this->_sprite_accuracy.setOrigin(sf::Vector2f(
		this->_sprite_accuracy.getGlobalBounds().width / 2.f,
		this->_sprite_accuracy.getGlobalBounds().height / 2.f));
	this->_sprite_accuracy.setPosition(sf::Vector2f(
		this->_window.getSize().x / 2.f,
		this->_window.getSize().y / 2.f - this->_cursor.getGlobalBounds().height * 1.5f));
}

void	GameScreen::setUserAccuracy(const eAccuracy accuracy, std::vector<eAccuracy>& notes_played)
{
	this->_current_accuracy += this->_accuracy_ratio[notes_played[notes_played.size() - 1]];
	this->_user_accuracy = (this->_current_accuracy / notes_played.size()) * 100.f;
}

void	GameScreen::setAccuracy(const eAccuracy accuracy, std::vector<eAccuracy>& notes_played)
{
	notes_played.push_back(accuracy);
	this->setSpriteAccuracy(accuracy);
	this->setUserAccuracy(accuracy, notes_played);
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

void	IScreen::draw(const sf::Drawable& object)
{
	this->_window.draw(object);
}

int		GameScreen::run()
{
	int				status;
	sf::Event		event;
	bool			user_input;

	while (user_input = this->_window.pollEvent(event))
	{
		for (auto it : this->_events)
		{
			if ((status = it->update(*this, event)) != this->_index)
				return (status);
		}
	}

	// We need to update the window even if the user doesn't do anything.
	// The game has to continue ;)
	if (!user_input)
		this->_events[1]->update(*this, sf::Event());

	this->_window.clear();
	for (auto it : this->_events)
		it->draw(*this);
	this->_window.display();

	this->updateFPS();
	return (this->_index);
}

void	GameScreen::restart()
{
	std::vector<const sf::Texture *>	textures;

	textures.push_back(&this->_skin->getComponent(eSkinComponent::SK_NOTE));
	textures.push_back(&this->_skin->getComponent(eSkinComponent::SK_NOTE_OUTLINE));
	textures.push_back(&this->_skin->getComponent(eSkinComponent::SK_LN_BEGIN));
	textures.push_back(&this->_skin->getComponent(eSkinComponent::SK_LN_OUTLINE_BEGIN));
	textures.push_back(&this->_skin->getComponent(eSkinComponent::SK_LN));
	textures.push_back(&this->_skin->getComponent(eSkinComponent::SK_LN_OUTLINE));
	textures.push_back(&this->_skin->getComponent(eSkinComponent::SK_LN_END));
	textures.push_back(&this->_skin->getComponent(eSkinComponent::SK_LN_OUTLINE_END));

	std::cout << "Restarting game" << std::endl;

	for (auto it = this->_notes.begin(); it != this->_notes.end();)
	{
		delete(*it);
		it = this->_notes.erase(it);
	}

	this->_user_accuracy = 100.f;
	this->_current_accuracy = 0.f;
	this->_notes.push_back(new Note(sf::seconds(1), 0.5f, sf::Vector2i(0, 1), textures, this->_speed));
	this->_notes.push_back(new Note(sf::seconds(2), 0.f, sf::Vector2i(0, 1), textures, this->_speed));
	this->_notes.push_back(new Note(sf::seconds(3), 0.5f, sf::Vector2i(0, -1), textures, this->_speed));
	this->_notes.push_back(new Note(sf::seconds(4), 0.f, sf::Vector2i(0, -1), textures, this->_speed));
	this->_notes_size = this->_notes.size();
	this->_sprite_accuracy = sf::Sprite();
}