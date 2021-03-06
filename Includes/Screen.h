#pragma once

# include "Constants.h"
# include "Note.h"

class IEvent;
class Button;
class Note;
class Skin;

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
	virtual sf::RenderWindow&		getWindow();
	virtual std::vector<IEvent *>&	getEvents();
	virtual eGamestate				getState() const;
	virtual const unsigned int		getIndex() const;
	virtual const float				getFPS() const;
	virtual const sf::Text&			getFPSText() const;
	virtual const unsigned int		getFrameLimiter() const;
	virtual const sf::Font&			getMainFont() const;

	//SETTERS
	virtual void	updateFPS();
	virtual void	setFrameLimiter(const unsigned int frame_limiter);

	//METHODS
	virtual int		run();
	virtual void	draw(const sf::Drawable& object);

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

	virtual std::vector<Button *>&	getButtons();

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
	const std::vector<Note *>&			getNotes() const;
	const std::vector<Note *>			getNextNotes(const sf::Time& time) const;
	const std::vector<Note *>			getNotesWithSameTiming(const sf::Time& time, const float& length) const;
	const Note&							getNoteByIndex(unsigned int index) const;
	const unsigned int					getNotesSize() const;
	const std::vector<sf::VertexArray>&	getCross() const;
	const unsigned int					getSpeed() const;
	const float							getUserAccuracy() const;
	const Skin&							getSkin() const;
	const sf::Sprite&					getCursor() const;
	const sf::Sprite&					getSpriteAccuracy() const;

	//SETTERS
	void	removeNote(const Note& note);
	void	addSpeed(const int offset);
	void	setSpriteAccuracy(const eAccuracy accuracy);
	void	setUserAccuracy(const eAccuracy accuracy, std::vector<eAccuracy>& notes_played);
	void	setAccuracy(const eAccuracy accuracy, std::vector<eAccuracy>& notes_played);

	//METHODS
	virtual int		run();
	void			restart();

protected:
	std::vector<Note *>				_notes; // Map file with all notes will be here
	unsigned int					_notes_size;
	std::vector<sf::VertexArray>	_cross;
	unsigned int					_speed;
	float							_user_accuracy;
	float							_current_accuracy;
	float							_accuracy_ratio[eAccuracy::ACC_SIZE];
	Skin*							_skin;
	sf::Sprite						_cursor;
	sf::Sprite						_sprite_accuracy;
};