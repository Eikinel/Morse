#pragma once

# include "Constants.h"
# include "Entity.h"

class			Note
{
public:
	Note(const sf::Time& time, const float duration, const sf::Texture& t_note, const sf::Texture& t_note_outline);
	virtual ~Note();

	//GETTERS
	const sf::Time&						getTime() const;
	const float							getDuration() const;
	const std::vector<sf::Sprite *>&	getSprites() const;

protected:
	sf::Time					_time;
	float						_duration; // 0.f = short note
	std::vector<sf::Sprite *>	_sprites;
};