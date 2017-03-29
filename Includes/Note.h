#pragma once

# include "Constants.h"

class			Note
{
public:
	Note(const sf::Time& time, const float duration);
	virtual ~Note();

protected:
	sf::Time	_time;
	float		_duration; // 0.f = short note
};