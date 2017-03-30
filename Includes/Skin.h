#pragma once

#include "Constants.h"

enum	eSkinComponent
{
	SK_NOTE,
	SK_NOTE_OUTLINE,
	SK_CURSOR,
	SK_SIZE
};

class			Skin
{
public:
	Skin(std::string path = SKIN_DIR"/Default");
	Skin(const Skin& other);
	~Skin();

	//GETTERS
	const sf::Texture&	getComponent(eSkinComponent index) const;

private:
	//TEXTURES
	std::vector<sf::Texture *>	_textures;
};