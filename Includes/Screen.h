#pragma once

# include "Constants.h"
# include "Note.h"
#include <SFML\Audio.hpp>

enum ePhase;
class IEvent;
class Button;
class Skin;
class Song;

enum				eGamestate
{
	EXIT = -1,
	MENU,
	OPTIONS,
	GAME
};

class				IScreen
{
public:
	IScreen(sf::RenderWindow& window, eGamestate state);
	virtual ~IScreen();

	//GETTERS
	sf::RenderWindow&		getWindow();
	std::vector<IEvent *>&	getEvents();
	eGamestate				getState() const;
	const unsigned int		getIndex() const;
	const float				getFPS() const;
	const sf::Text&			getFPSText() const;
	const unsigned int		getFrameLimiter() const;
	const sf::Font&			getMainFont() const;

	//SETTERS
	void	updateFPS();
	void	setFrameLimiter(const unsigned int frame_limiter);

	//METHODS
	virtual int		run();
	virtual void	draw(const sf::Drawable& object, sf::RenderStates states = sf::RenderStates());

protected:
	sf::RenderWindow&		_window;
	std::vector<IEvent *>	_events;
	eGamestate				_state;
	unsigned int			_index;

	//FPS handling
	sf::Clock		_clock;
	float			_fps;
	unsigned int	_frame_limiter;
	sf::Text		_fps_text;
	sf::Font		_main_font;
};

class				MenuScreen : public IScreen
{
public:
	MenuScreen(sf::RenderWindow& window);
	virtual ~MenuScreen();

	std::vector<Button *>&	getButtons();

protected:
	sf::Text				_title;
	std::vector<Button *>	_buttons;
};

class				GameScreen : public IScreen
{
public:
	GameScreen(sf::RenderWindow& window);
	virtual ~GameScreen();

	//GETTERS
	// General
	const unsigned int				getSpeed() const;
	const float						getUserAccuracy() const;
	const Skin&						getSkin() const;
	Song&							getSong();
	const std::vector<eAccuracy>&	getNotesPlayed() const;
	const sf::Sprite&				getCursor() const;
	const sf::Sprite&				getSpriteAccuracy() const;
	const sf::Text&					getPhaseText() const;
	sf::Sound&						getMetronome();

	// Attack phase
	const sf::Sprite&				getArrow() const;
	const sf::CircleShape&			getArrowRadiusShape() const;

	// Defense phase
	const std::vector<sf::VertexArray>&	getCross() const;

	//SETTERS
	void	addSpeed(const int offset);
	void	setSpriteAccuracy(const eAccuracy accuracy);
	void	addUserAccuracy(const eAccuracy accuracy);
	void	addAccuracy(const eAccuracy accuracy);
	void	setPhaseText(const std::string& text);

	//METHODS
	int		run();
	void	restart();

private:
	// Game phase
	unsigned int	_speed;
	float			_accuracy_ratio[eAccuracy::ACC_SIZE];
	Skin*			_skin;
	Song*			_song;

	// To put on a Stat class
	std::vector<eAccuracy>	_notes_played;
	float					_user_accuracy;
	float					_current_accuracy;
	sf::Sprite				_cursor;
	sf::Sprite				_sprite_accuracy;
	sf::Text				_phase_text;
	sf::Sound				_metronome;

	// Attack phase
	sf::Sprite				_arrow;
	sf::CircleShape			_arrow_radius_shape; // Will be a sprite changeable through the skin component

	// Defense phase
	std::vector<sf::VertexArray>	_cross;
};