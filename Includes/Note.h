#pragma once

# include "Constants.h"
# include "Entity.h"

enum			eAccuracy
{
	ACC_MISS,
	ACC_BAD,
	ACC_GOOD,
	ACC_GREAT,
	ACC_SIZE
};

class			Note
{
public:
	// CONSTRUCTOR (for attack)
	//Note(const sf::Time& time, const float duration, const sf::Vector2i& );
	// CONSTRUCTOR (for defense)
	Note(const sf::Time& time, const float duration, const sf::Vector2i& direction = sf::Vector2i(0, 1));
	virtual ~Note();

	//GETTERS
	const sf::Time&						getTime() const;
	const float							getDuration() const;
	const sf::Vector2i&					getDirection() const;
	const std::vector<sf::Sprite *>&	getSprites() const;
	const sf::Color&					getBaseColor() const;

	const float							getLength() const;
	const bool							isHeld() const;
	const bool							hasBeenHeld() const;

	//SETTERS
	void	setDuration(float duration);
	void	setSprites(std::vector<const sf::Texture *>& textures);
	void	scaleDuration(const sf::Time& offset);
	void	scaleLongNote(const unsigned int& speed);
	void	setHeld(const bool held);
	void	setBeenHeld(const bool been_held);

	//OPERATOR
	bool	operator==(const Note& note);

protected:
	sf::Time					_time;
	float						_duration; // 0.f = short note
	sf::Vector2i				_direction;
	std::vector<sf::Sprite *>	_sprites;
	sf::Color					_base_color;

	//For long note only
	float						_length; // to perform scaling operations
	bool						_is_held; // currently played ?
	bool						_has_been_held; // don't count the note two times or more !
};