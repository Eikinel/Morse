#pragma once

enum			ePhase
{
	ATTACK,
	DEFENSE,
	PHASE_SIZE
};

class			Phase
{
public:
	//-STRUCTORS
	Phase(const ePhase type, const sf::Time& time)
	{
		std::string	names[] = { "Attack", "Defense" };

		std::cout << "Creating " << names[type] << " phase at " << time.asSeconds() << std::endl;

		this->_type = type;
		this->_name = names[type];
		this->_time = time;
	}

	~Phase() { std::cout << "Deleting phase " << this->_name << std::endl; }


	//GETTERS
	const ePhase	getType() const
	{
		return (this->_type);
	}

	const std::string	getName() const
	{
		return (this->_name);
	}

	const sf::Time&	getTime() const
	{
		return (this->_time);
	}


	//SETTERS
	void	setType(const ePhase type)
	{
		this->_type = type;
	}

	void	setName(const std::string name)
	{
		this->_name = name;
	}

	void	setTime(const sf::Time time)
	{
		this->_time = time;
	}

private:
	ePhase		_type;
	std::string	_name;
	sf::Time	_time;
};