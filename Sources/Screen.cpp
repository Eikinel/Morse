#include "Screen.h"
#include "Event.h"
#include "Button.h"
#include "Skin.h"
#include "Phase.h"
#include "Note.h"
#include "Bezier.h"
#include "Song.h"


//CONSTRUCTORS
IScreen::IScreen(sf::RenderWindow& window, GAMESTATE state) : _window(window), _state(state)
{
	sf::Vector2u	win_size(window.getSize());

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

MenuScreen::MenuScreen(sf::RenderWindow& window) : IScreen(window, GAMESTATE::MENU)
{
	std::cout << std::endl << "Creating menu screen" << std::endl;
	this->_events.push_back(new WindowDefaultEvent); // Event handler for options, close window, etc.
	this->_events.push_back(new MenuEvent); // Update menu, draw it and react in terms of user inputs.

	sf::Vector2f	win_size(window.getSize());

	this->_buttons.push_back(new Button(GAME_NAME, win_size.y / 4.f, this->_main_font, sf::Vector2f(
		win_size.x / 2.f,
		win_size.y / 20.f), INDENT::CENTER));
	this->_buttons.push_back(new Button("Play", win_size.y / 6.f, this->_main_font, sf::Vector2f(
		win_size.x / 2.f,
		win_size.y / 2.5f), INDENT::CENTER));
	this->_buttons.push_back(new Button("Options", win_size.y / 6.f, this->_main_font, sf::Vector2f(
		win_size.x / 2.f,
		win_size.y / 1.6f), INDENT::CENTER));
	this->_buttons.push_back(new Button("Exit", win_size.y / 12.f, this->_main_font, sf::Vector2f(
		win_size.x / 2.f,
		win_size.y / 1.15f), INDENT::CENTER));
	this->_buttons[1]->onClick(&IEvent::changeScreen, this->_events[1], GAMESTATE::GAME, static_cast<IScreen *>(this));
	this->_buttons[2]->onClick(&IEvent::changeScreen, this->_events[1], GAMESTATE::OPTIONS, static_cast<IScreen *>(this));
	this->_buttons[3]->onClick(&IEvent::changeScreen, this->_events[1], GAMESTATE::EXIT, static_cast<IScreen *>(this));
}

GameScreen::GameScreen(sf::RenderWindow& window) :  IScreen(window, GAMESTATE::GAME)
{
	std::cout << std::endl << "Creating game screen" << std::endl;
	this->_events.push_back(new WindowDefaultEvent); // Event handler for options, close window, etc.
	this->_events.push_back(new GameEvent); // Update game, draw it and react in terms of user inputs.
	//this->_window.setKeyRepeatEnabled(false);

	sf::Vector2f		win_size(window.getSize());
	sf::VertexArray		va_tmp(sf::LinesStrip, 2);

	// Common components
	this->_speed = 10;
	this->_accuracy_ratio[(int)ACCURACY::ACC_MISS] = 0.f;
	this->_accuracy_ratio[(int)ACCURACY::ACC_BAD] = 0.3334f;
	this->_accuracy_ratio[(int)ACCURACY::ACC_GOOD] = 0.6667f;
	this->_accuracy_ratio[(int)ACCURACY::ACC_GREAT] = 1.f;

	this->_skin = new Skin();
	this->_song = new Song("/Test/Born To Be Bone.ogg", this->_speed);
	this->_metronome.setBuffer(this->_skin->getHitSound(HIT_SOUND::COWBELL));

	this->_cursor.setTexture(this->_skin->getTexture(SKIN_TEXTURE::SK_CURSOR));
	this->_cursor.setOrigin(sf::Vector2f(
		this->_cursor.getGlobalBounds().width / 2.f,
		this->_cursor.getGlobalBounds().height / 2.f));
	this->_cursor.setPosition(win_size / 2.f);

	this->_phase_text.setFont(this->_main_font);
	this->_phase_text.setCharacterSize(42);
	this->_phase_text.setPosition(sf::Vector2f(10, win_size.y - 50));

	// Defense phase components
	va_tmp[0].color = va_tmp[1].color = sf::Color::Green;
	va_tmp[0].position = sf::Vector2f(win_size.x / 2.f, 0);
	va_tmp[1].position = sf::Vector2f(win_size.x / 2.f, win_size.y);
	this->_cross.push_back(va_tmp);
	va_tmp[0].position = sf::Vector2f(0, win_size.y / 2.f);
	va_tmp[1].position = sf::Vector2f(win_size.x, win_size.y / 2.f);
	this->_cross.push_back(va_tmp);

	// Attack phase components
	this->_arrow.setTexture(this->_skin->getTexture(SKIN_TEXTURE::SK_ARROW));
	this->_arrow.setOrigin(sf::Vector2f(0, this->_arrow.getGlobalBounds().height / 2.f));
	this->_arrow.setPosition(sf::Vector2f(
		this->_cursor.getPosition().x,
		this->_cursor.getPosition().y));
	
	this->_arrow_radius_shape = sf::CircleShape();
	this->_arrow_radius_shape.setRadius(this->_arrow.getGlobalBounds().width);
	this->_arrow_radius_shape.setOrigin(sf::Vector2f(
		this->_arrow_radius_shape.getGlobalBounds().width / 2.f,
		this->_arrow_radius_shape.getGlobalBounds().height / 2.f));
	this->_arrow_radius_shape.setPosition(this->_cursor.getPosition());
	this->_arrow_radius_shape.setFillColor(sf::Color::Transparent);
	this->_arrow_radius_shape.setOutlineThickness(1.f);
	this->_arrow_radius_shape.setOutlineColor(sf::Color::White);

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
	delete(this->_song);
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

GAMESTATE				IScreen::getState() const
{
	return (this->_state);
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

Song&	GameScreen::getSong()
{
	return (*this->_song);
}

const std::vector<ACCURACY>&	GameScreen::getNotesPlayed() const
{
	return (this->_notes_played);
}

const sf::Sprite&	GameScreen::getCursor() const
{
	return (this->_cursor);
}

const sf::Sprite&	GameScreen::getSpriteAccuracy() const
{
	return (this->_sprite_accuracy);
}

const sf::Text&		GameScreen::getPhaseText() const
{
	return (this->_phase_text);
}

sf::Sound&	GameScreen::getMetronome()
{
	return (this->_metronome);
}


// Attack phase
const sf::Sprite&	GameScreen::getArrow() const
{
	return (this->_arrow);
}

const sf::CircleShape&	GameScreen::getArrowRadiusShape() const
{
	return (this->_arrow_radius_shape);
}


// Defense phase
const std::vector<sf::VertexArray>&	GameScreen::getCross() const
{
	return (this->_cross);
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

void	GameScreen::addSpeed(const int offset)
{
	if (this->_speed + offset > 0 && this->_speed < MAX_SPEED)
	{
		std::cout << "Changing speed to " << this->_speed + offset << std::endl;
		this->_speed += offset;
	}
	else
		std::cerr << "Can't change speed : floor or ceiling reached" << std::endl;

	for (auto it : this->_song->getNotes())
		if (it->getDuration() > 0.f)
			it->scaleLongNote(this->_speed);
}

void	GameScreen::setSpriteAccuracy(const ACCURACY accuracy)
{
	this->_sprite_accuracy.setTexture(this->_skin->getTexture((SKIN_TEXTURE)((int)accuracy + (int)SKIN_TEXTURE::SK_MISS)));
	this->_sprite_accuracy.setOrigin(sf::Vector2f(
		this->_sprite_accuracy.getGlobalBounds().width / 2.f,
		this->_sprite_accuracy.getGlobalBounds().height / 2.f));
	this->_sprite_accuracy.setPosition(sf::Vector2f(
		this->_window.getSize().x / 2.f,
		this->_window.getSize().y / 2.f - this->_cursor.getGlobalBounds().height * 1.5f));
}

void	GameScreen::addUserAccuracy(const ACCURACY accuracy)
{
	this->_current_accuracy += this->_accuracy_ratio[(int)this->_notes_played[this->_notes_played.size() - 1]];
	this->_user_accuracy = (this->_current_accuracy / this->_notes_played.size()) * 100.f;
}

void	GameScreen::addAccuracy(const ACCURACY accuracy)
{
	this->_notes_played.push_back(accuracy);
	this->setSpriteAccuracy(accuracy);
	this->addUserAccuracy(accuracy);
}

void	GameScreen::setPhaseText(const std::string& text)
{
	this->_phase_text.setString(text);
}


//METHODS
GAMESTATE	IScreen::run()
{
	GAMESTATE			state;
	sf::Event			event;

	this->_window.clear();
	this->updateFPS();
	
	while (this->_window.pollEvent(event))
	{
		for (auto it : this->_events)
		{
			if ((state = it->update(*this, event)) != this->_state)
				return state;
		}
	}

	for (auto it : this->_events)
		it->draw(*this);

	this->_window.display();

	return this->_state;
}

void	IScreen::draw(const sf::Drawable& object, sf::RenderStates states)
{
	this->_window.draw(object, states);
}

void	IScreen::draw(const sf::Vertex* vertices, size_t vertexCount, sf::PrimitiveType type, const sf::RenderStates states)
{
	this->_window.draw(vertices, vertexCount, type, states);
}

GAMESTATE	GameScreen::run()
{
	GAMESTATE		state;
	sf::Event		event;
	bool			user_input = false;

	// Clear the previous window with old updates and draws
	this->_window.clear();
	this->updateFPS();
	
	// Process user's event and call each updates
	while (this->_window.pollEvent(event))
	{
		user_input = true;
		for (auto it : this->_events)
		{
			if ((state = it->update(*this, event)) != this->_state)
				return state;
		}
	}

	// Update the game even if the user doesn't do anything
	if (!user_input)
		this->_events[1]->update(*this, sf::Event());

	// Draw everything who has been updated
	for (auto it : this->_events)
		it->draw(*this);

	this->_window.display();

	return this->_state;
}

void	GameScreen::restart()
{
	std::vector<const sf::Texture *>	textures;

	textures.push_back(&this->_skin->getTexture(SKIN_TEXTURE::SK_NOTE));
	textures.push_back(&this->_skin->getTexture(SKIN_TEXTURE::SK_NOTE_OUTLINE));
	textures.push_back(&this->_skin->getTexture(SKIN_TEXTURE::SK_LN_BEGIN));
	textures.push_back(&this->_skin->getTexture(SKIN_TEXTURE::SK_LN_OUTLINE_BEGIN));
	textures.push_back(&this->_skin->getTexture(SKIN_TEXTURE::SK_LN));
	textures.push_back(&this->_skin->getTexture(SKIN_TEXTURE::SK_LN_OUTLINE));
	textures.push_back(&this->_skin->getTexture(SKIN_TEXTURE::SK_LN_END));
	textures.push_back(&this->_skin->getTexture(SKIN_TEXTURE::SK_LN_OUTLINE_END));

	std::cout << "Restarting game" << std::endl;

	this->_user_accuracy = 100.f;
	this->_current_accuracy = 0.f;
	this->_notes_played.clear();
	this->_sprite_accuracy = sf::Sprite();

	this->_song->restart(textures);
	static_cast<GameEvent*>(this->_events[1])->restart();

	std::cout << "END OF RESTART" << std::endl << std::endl;
}