#include "Skin.h"
#include <regex>

Skin::Skin(std::string path)
{
	std::string		sprites_filename[(int)SKIN_TEXTURE::SK_SIZE] = {
		"note", "note_outline",
		"ln_begin", "ln", "ln_end", "ln_outline_begin", "ln_outline", "ln_outline_end",
		"cursor", "arrow",
		"miss", "bad", "good", "great"
	};

	std::string		hit_sounds_filename[(int)HIT_SOUND::HIT_SOUND_SIZE] = {
		"bass", "snare", "cowbell", "hi-hat", "crash", "ride", "tom1", "tom2", "tom3", "roll"
	};

	if (path[path.size() - 1] != '/')
		path += "/";

	std::cout << "Creating new skin with path " << path << std::endl;

	// Add skin's sprites and hit sounds
	for (unsigned int i = (int)SKIN_TEXTURE::SK_NOTE; i < (int)SKIN_TEXTURE::SK_SIZE; i++)
		this->addComponent(path, sprites_filename[i], SKIN_COMPONENT::TEXTURE);

	for (unsigned int i = (int)HIT_SOUND::BASS; i < (int)HIT_SOUND::HIT_SOUND_SIZE; i++)
		this->addComponent(path, hit_sounds_filename[i], SKIN_COMPONENT::HIT_SOUND);
}

Skin::Skin(const Skin& other)
{
	std::cout << "Switching skins" << std::endl;
	this->_textures = other._textures;
}

Skin::~Skin()
{
	std::cout << "Deleting skin" << std::endl;
	for (auto it : this->_textures)
		delete(it);
}


//GETTERS
const sf::Texture&	Skin::getTexture(const SKIN_TEXTURE& index) const
{
	return (*this->_textures[(int)index]);
}

const sf::SoundBuffer&	Skin::getHitSound(const HIT_SOUND& index) const
{
	return (*this->_hit_sounds[(int)index]);
}


//METHODS
void	Skin::addComponent(std::string path, const std::string& filename, const SKIN_COMPONENT& type)
{
	sf::Texture t = sf::Texture();
	sf::SoundBuffer s = sf::SoundBuffer();

	switch (type)
	{
	case SKIN_COMPONENT::TEXTURE:
		std::cout << "Creating new texture with path " << path << std::endl;

		path += SKIN_SPRITE_DIR + '/' + filename + ".png";
		if (!t.loadFromFile(path))
			t.loadFromFile(std::string(DEFAULT_SKIN_DIR) + SKIN_SPRITE_DIR + '/' + filename + ".png");
		this->_textures.push_back(new sf::Texture(t));

		break;
	case SKIN_COMPONENT::HIT_SOUND:
		std::cout << "Creating new hit sound with path " << path << std::endl;

		path += SKIN_AUDIO_DIR + '/' + filename + ".wav";
		if (!s.loadFromFile(path))
			s.loadFromFile(std::string(DEFAULT_SKIN_DIR) + SKIN_AUDIO_DIR + '/' + filename + ".wav");
		this->_hit_sounds.push_back(new sf::SoundBuffer(s));

		break;
	default:
		break;
	}
}