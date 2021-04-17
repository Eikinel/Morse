#pragma once

enum			PHASE
{
	ATTACK,
	DEFENSE,
	PHASE_SIZE
};

class			Phase
{
public:
	//-STRUCTORS
	Phase(const PHASE type, const sf::Time& time)
	{
		std::string	names[] = { "Attack", "Defense" };

		std::cout << "Creating " << names[type] << " phase at " << time.asSeconds() << std::endl;

		this->_type = type;
		this->_name = names[type];
		this->_time = time;
	}

	~Phase() { std::cout << "Deleting phase " << this->_name << std::endl; }


	//GETTERS
	const PHASE			getType() const
	{
		return (this->_type);
	}

	const std::string	getName() const
	{
		return (this->_name);
	}

	const sf::Time&		getTime() const
	{
		return (this->_time);
	}


	//SETTERS
	void	setType(const PHASE type)
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
	PHASE		_type;
	std::string	_name;
	sf::Time	_time;
};