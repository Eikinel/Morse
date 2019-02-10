#pragma once

# include "Constants.h"
# include "Entity.h"

enum	eGamestate;
enum	eAccuracy;
class	IScreen;
class	Note;
class	Phase;


class				IEvent
{
public:
	IEvent();
	virtual ~IEvent() {}

	virtual int		update(IScreen& screen, sf::Event& event) = 0;
	virtual void	draw(IScreen& screen) = 0;

	//GETTERS
	bool				getToggleBoundingBoxes();
	std::vector<bool>&	getToggleOptions();
	template <typename T> void	getBoundingBoxes(const Entity<T>& e, std::vector<sf::VertexArray>& boxes)
	{
		const sf::Color type[5] = { sf::Color::White, sf::Color::Cyan, sf::Color::Blue, };

		for (std::vector<T *>::const_iterator it = e.getColliders().begin(); it != e.getColliders().end(); ++it)
		{
			sf::FloatRect	box = (*it)->getShape().getGlobalBounds();
			sf::VertexArray	lines(sf::LinesStrip, 5);

			lines[0].position = sf::Vector2f(box.left, box.top);
			lines[1].position = sf::Vector2f(box.left + box.width, box.top);
			lines[2].position = sf::Vector2f(box.left + box.width, box.top + box.height);
			lines[3].position = sf::Vector2f(box.left, box.top + box.height);
			lines[4].position = sf::Vector2f(box.left, box.top);

			for (unsigned int i = 0; i < lines.getVertexCount(); i++)
				lines[i].color = type[e.getType()];
			boxes.push_back(lines);
		}
	}

	//SETTERS
	void	setToggleOptions(const std::vector<bool>& toggle_options);

	//METHODS
	int	changeScreen(eGamestate gamestate, IScreen* screen);
	int	createScreen(eGamestate gamestate, IScreen* screen);
	int	toggleBoundingBoxes(int index);

protected:
	std::vector<bool>	_toggle_options;
	sf::Clock			_general_clock;
};

class				WindowDefaultEvent : public IEvent
{
public:
	WindowDefaultEvent();

	int		update(IScreen& screen, sf::Event& event);
	void	draw(IScreen& screen) {}
};

class				MenuEvent : public IEvent
{
public:
	MenuEvent();

	int		update(IScreen& screen, sf::Event& event);
	void	draw(IScreen& screen);
};

class				GameEvent : public IEvent
{
public:
	GameEvent();
	~GameEvent();

	int		update(IScreen& screen, sf::Event& event);
	void	draw(IScreen& screen);

	//GETTERS
	const sf::Clock&							getGameClock() const;
	const std::vector<std::shared_ptr<Note>>&	getNextNotes() const;
	const std::vector<sf::Vector2f>				getTimingGaps() const;
	const eAccuracy								getAccuracy(const sf::Time& delta) const;
	const sf::Time&								getSkipFreeze() const;

	//METHODS
	auto	removeNote(const Note& note);
	void	restart();

private:
	sf::Clock							_game_clock;
	sf::Time							_skip_freeze; // Freeze the time when the user skipped the song
	const Phase*						_current_phase;
	std::vector<IEvent *>				_phases_events;
	std::vector<std::shared_ptr<Note>>	_next_notes;
	std::vector<sf::Vector2f>			_timing_gaps;
	bool								_metronome_played;
};

class				AttackEvent : public IEvent
{
public:
	AttackEvent(GameEvent& gevent);
	~AttackEvent();

	int		update(IScreen& screen, sf::Event& event);
	void	draw(IScreen& screen);

private:
	GameEvent&		_gevent;
	float			_arrow_angle;	// Angle value of the arrow
	sf::Transform	_arrow_trans;	// Transformation to apply to the arrow
	sf::Transform	_pos_curve;		// Transformation to apply to the curve (if each curve, maybe use a vector ?)
};

class				DefenseEvent : public IEvent
{
public:
	DefenseEvent(GameEvent& gevent);
	~DefenseEvent();

	int		update(IScreen& screen, sf::Event& event);
	void	draw(IScreen& screen);

private:
	GameEvent&	_gevent;
};

std::string	setPrecision(float value, unsigned int precision);