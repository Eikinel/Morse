#pragma once

#include "Constants.h"
#include <SFML\Audio.hpp>

enum class SKIN_TEXTURE
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

enum class HIT_SOUND
{
	BASS,
	SNARE,
	COWBELL,
	HI_HAT,
	CRASH,
	RIDE,
	TOM1,
	TOM2,
	TOM3,
	ROLL,
	HIT_SOUND_SIZE
};

enum class SKIN_COMPONENT
{
	TEXTURE,
	HIT_SOUND,
	SKIN_COMPONENT_SIZE
};

class			Skin
{
public:
	Skin(std::string path = DEFAULT_SKIN_DIR);
	Skin(const Skin& other);
	~Skin();

	// GETTERS
	const sf::Texture&		getTexture(const SKIN_TEXTURE& index) const;
	const sf::SoundBuffer&	getHitSound(const HIT_SOUND& index) const;

	// METHODS
	void	addComponent(std::string path, const std::string& filename, const SKIN_COMPONENT& type);

private:
	// TEXTURES
	std::vector<sf::Texture *>		_textures;
	std::vector<sf::SoundBuffer *>	_hit_sounds;
};