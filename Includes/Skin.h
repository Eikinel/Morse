#pragma once

#include "Constants.h"

enum	eSkinComponent
{
	SK_NOTE,
	SK_NOTE_OUTLINE,
	SK_LN_BEGIN,
	SK_LN,
	SK_LN_END,
	SK_LN_OUTLINE_BEGIN,
	SK_LN_OUTLINE,
	SK_LN_OUTLINE_END,
	SK_CURSOR,
	SK_ARROW,
	SK_MISS,
	SK_BAD,
	SK_GOOD,
	SK_GREAT,
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