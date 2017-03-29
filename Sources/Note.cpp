#include "Note.h"
#include "Entity.h"

Note::Note(const sf::Time& time, const float duration)
{
	if (time.asMilliseconds() > 0.f && duration > 0.f)
	{
		std::cout << "Setting note with timing " << time.asMilliseconds() << " and duration " << duration << "." << std::endl;
		this->_time = time;
		this->_duration = duration;
	}
	else
		std::cerr << "Error while setting note. Values are " << time.asMilliseconds() << " and " << duration << "." << std::endl;
}

Note::~Note()
{

}