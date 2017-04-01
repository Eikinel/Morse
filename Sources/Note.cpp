#include "Note.h"
#include "Entity.h"

Note::Note(const sf::Time& time, const float duration, const sf::Vector2i& direction,
	const std::vector<const sf::Texture *>& textures, const unsigned int& speed)
{
	if (time.asSeconds() >= 0.f && duration >= 0.f && direction.x != direction.y && direction.x != -direction.y)
	{
		std::cout << "Setting note with timing " << time.asSeconds() << ", " <<
			"duration " << duration << " " <<
			"and direction [" << direction.x << " ; " << direction.y << "]" << std::endl;

		this->_time = time;
		this->_duration = duration;
		this->_length = this->_time.asSeconds() + this->_duration;
		this->_direction = direction;
		this->_is_held = false;
		this->_has_been_held = false;
		this->_sprites.push_back(new sf::Sprite(*textures[0]));
		this->_sprites.push_back(new sf::Sprite(*textures[1]));
		if (this->_duration > 0.f)
		{
			this->_sprites.push_back(new sf::Sprite(*textures[2]));
			this->_sprites.push_back(new sf::Sprite(*textures[3]));
			this->_sprites.push_back(new sf::Sprite(*textures[4]));
			this->_sprites.push_back(new sf::Sprite(*textures[5]));
			this->_sprites.push_back(new sf::Sprite(*textures[6]));
			this->_sprites.push_back(new sf::Sprite(*textures[7]));
			this->scaleLongNote(speed);
		}

		for (auto it : this->_sprites)
		{
			if (this->_direction.x != 0)
				it->setRotation(90.f * (this->_direction.x == 1 ? 0 : 2));
			else if (this->_direction.y != 0)
				it->setRotation(90.f * (this->_direction.y == 1 ? 1 : 3));
			it->setOrigin(sf::Vector2f(
				it->getLocalBounds().width / 2.f,
				it->getLocalBounds().height / 2.f));
		}
	}
	else
		std::cerr << "Error while setting note. Values are { " <<
		"time : " << time.asSeconds() << ", " <<
		"duration : " << duration << ", " <<
		"direction : [" << direction.x << " ; " << direction.y << "]" <<
		" }." << std::endl;
}

Note::~Note()
{
	std::cout << "Deleting note" << std::endl;
	for (auto it : this->_sprites)
		delete(it);
}


//GETTERS
const sf::Time&	Note::getTime() const
{
	return (this->_time);
}

const float	Note::getDuration() const
{
	return (this->_duration);
}

const float	Note::getLength() const
{
	return (this->_length);
}

const sf::Vector2i&	Note::getDirection() const
{
	return (this->_direction);
}

const std::vector<sf::Sprite *>&	Note::getSprites() const
{
	return (this->_sprites);
}

const bool	Note::isHeld() const
{
	return (this->_is_held);
}

const bool	Note::hasBeenHeld() const
{
	return (this->_has_been_held);
}


//SETTERS
void	Note::setDuration(float duration)
{
	if (duration < 0.f)
		duration = 0.f;
	this->_duration = duration;
}

void	Note::scaleDuration(const sf::Time& offset)
{
	this->setDuration(this->_length - offset.asSeconds());
}

void	Note::scaleLongNote(const unsigned int& speed)
{
	this->_sprites[4]->setScale(sf::Vector2f(this->_duration * speed * (std::abs(this->_direction.x) == 1 ? 2 : 1), 1));
	this->_sprites[5]->setScale(this->_sprites[4]->getScale());
}

void	Note::setHeld(const bool held)
{
	this->_is_held = held;
}

void	Note::setBeenHeld(const bool been_held)
{
	this->_has_been_held = been_held;
}


//OPERATOR
bool	Note::operator==(const Note& note)
{
	if (this->_time == note._time &&
		this->_duration == note._duration &&
		this->_direction == note._direction)
		return (true);
	return (false);
}