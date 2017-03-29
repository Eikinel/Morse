#pragma once

#include "Constants.h"

enum	eSkinComponent
{
	NOTE,
	NOTE_OUTLINE,
	SKIN_SIZE
};

class			Skin
{
public:
	Skin(std::string path = SKIN_DIR"/Default");
	Skin(const Skin& other);
	~Skin();

	//GETTERS
	const sf::Sprite&	getComponent(eSkinComponent index) const;

	//SETTERS
	//void	setComponent(const std::string& path, eSkinComponent index);

private:
	//TEXTURES
	std::vector<sf::Texture *>	_textures;
	std::vector<sf::Sprite *>	_sprites;
};