#include "Note.h"
#include "Entity.h"

Note::Note(const sf::Time& time, const float duration, const sf::Texture& t_note, const sf::Texture& t_note_outline)
{
	if (time.asMilliseconds() >= 0.f && duration >= 0.f)
	{
		std::cout << "Setting note with timing " << time.asSeconds() << " and duration " << duration << "." << std::endl;
		this->_time = time;
		this->_duration = duration;
		this->_sprites.push_back(new sf::Sprite(t_note));
		this->_sprites.push_back(new sf::Sprite(t_note_outline));
		for (auto it : this->_sprites)
			it->setOrigin(sf::Vector2f(
				it->getGlobalBounds().width / 2.f,
				it->getGlobalBounds().height / 2.f));
	}
	else
		std::cerr << "Error while setting note. Values are " << time.asSeconds() << " and " << duration << "." << std::endl;
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

const std::vector<sf::Sprite *>&	Note::getSprites() const
{
	return (this->_sprites);
}