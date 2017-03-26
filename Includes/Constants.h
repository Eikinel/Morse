#pragma once

# include <SFML\Graphics.hpp>
# include <vector>
# include <iostream>

// Directories
# define ASSETS_DIR		"../Assets"
# define SPRITES_DIR	ASSETS_DIR"/Sprites"
# define FONTS_DIR		ASSETS_DIR"/Fonts"

# define GAME_NAME	"Rhythm Game"

// Global variable for screens, containing menu, game, etc.
class IScreen;
extern std::vector<IScreen *> all_screens;

// Static methods
static sf::Vector2f&	toIsometric(const sf::Vector2f& cart)
{
	return (sf::Vector2f(
		cart.x + cart.y,
		(cart.y - cart.x) / 2.f));
}

static sf::Vector2f&	toIsometric(const sf::FloatRect& cart)
{
	return (sf::Vector2f(
		cart.left + cart.top,
		(cart.top - cart.left) / 2.f));
}

static sf::Vector2f&	toCartesian(const sf::Vector2f& iso)
{
	return (sf::Vector2f(
		(iso.x - (iso.y * 2.f)) / 2.f,
		(iso.x / 2.f + iso.y)));
}