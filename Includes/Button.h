#pragma once

# include "Constants.h"
# include <functional>

enum class GAMESTATE;

enum class INDENT
{
	LEFT = 0,
	CENTER,
	RIGHT
};

class Button
{
public:
	Button(const std::string& text, const size_t& size, const sf::Vector2f& pos = sf::Vector2f(0, 0),
		const INDENT& indent = INDENT::LEFT, const std::string& path_to_font = FONTS_DIR"/Meatloaf Sketched.ttf",
		const sf::Texture& texture = sf::Texture());
	Button(const std::string& text, const size_t& size, const sf::Font& font, const sf::Vector2f& pos = sf::Vector2f(0, 0),
		const INDENT& indent = INDENT::LEFT, const sf::Texture& texture = sf::Texture());
	Button(const Button& other);
	virtual ~Button();

	//GETTERS
	virtual const sf::Text&				getText() const;
	virtual const std::function<int()>&	getEvent() const;

	//SETTERS
	void	setText(const sf::Text& text);
	void	setEvent(const std::function<int()> event);

	//METHODS
	template <typename U, typename... T>
	void onClick(int (U::*func)(GAMESTATE, T...), U* event, GAMESTATE gamestate, T... params)
	{
		this->_event = std::bind(func, event, gamestate, params...);
	}

	int triggerEvent();

private:
	sf::Font				_font;
	sf::Text				_text;
	std::function<int()>	_event;
};