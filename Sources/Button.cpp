#include "Button.h"
#include "Collider.h"


//CONSTRUCTORS
Button::Button(const std::string& text, const size_t& size, const sf::Vector2f& pos,
	const INDENT& indent, const std::string& path_to_font, const sf::Texture& texture)
{
	std::string	format[3] = { "LEFT", "CENTER", "RIGHT" };

	std::cout << "Add button with text \"" << text << "\" and identation " << format[(int)indent] << "." <<std::endl;
	this->_font.loadFromFile(path_to_font);
	this->_text.setString(text);
	this->_text.setFont(this->_font);
	this->_text.setCharacterSize(size);
	this->_text.setOrigin(sf::Vector2f(
		(this->_text.getGlobalBounds().width / 2.f) * (float)indent,
		this->_text.getGlobalBounds().height / 2.f));
	std::cout << "Position avant " << format[(int)indent] << " : " << pos.x << " ; " << pos.y << "." << std::endl;
	std::cout << "Position apres " << format[(int)indent] << " : " << pos.x + (pos.x / 2.f) * ((float)indent - 1.f) << " ; " << pos.y << "." << std::endl;
	this->_text.setPosition(pos + sf::Vector2f(
		(pos.x / 2.f) * ((float)indent - 1.f), 0));
	this->_event = NULL;
}

Button::Button(const std::string& text, const size_t& size, const sf::Font& font,
	const sf::Vector2f& pos, const INDENT& indent, const sf::Texture& texture)
{
	std::string	format[3] = { "LEFT", "CENTER", "RIGHT" };

	std::cout << "Add button with text \"" << text << "\" and identation " << format[(int)indent] << "." << std::endl;
	this->_font = font;
	this->_text.setString(text);
	this->_text.setFont(this->_font);
	this->_text.setCharacterSize(size);
	this->_text.setOrigin(sf::Vector2f(
		(this->_text.getGlobalBounds().width / 2.f) * (float)indent,
		this->_text.getGlobalBounds().height / 2.f));
	this->_text.setPosition(pos + sf::Vector2f(
		(pos.x / 2.f) * ((float)indent - 1.f), 0));
	this->_event = NULL;

	sf::FloatRect	box = this->_text.getGlobalBounds();
}

Button::Button(const Button& other)
{
	this->_font = sf::Font(other._font);
	this->_text = sf::Text(other._text);
	this->_event = other._event;
}

Button::~Button()
{
}


//GETTERS
const sf::Text&				Button::getText() const
{
	return (this->_text);
}

const std::function<int()>&	Button::getEvent() const
{
	return (this->_event);
}


//SETTERS
void	Button::setText(const sf::Text& text)
{
	this->_text = text;
}

void	Button::setEvent(const std::function<int()> event)
{
	this->_event = event;
}


//METHODS
int		Button::triggerEvent()
{
	if (this->_event != NULL)
		return(this->_event());
}