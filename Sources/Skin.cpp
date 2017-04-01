#include "Skin.h"
#include <regex>

Skin::Skin(std::string path)
{
	std::string		components[] =
	{ 
		"note.png", "note_outline.png",
		"ln_begin.png", "ln.png", "ln_end.png", "ln_outline_begin.png", "ln_outline.png", "ln_outline_end.png",
		"cursor.png",
		"miss.png", "bad.png", "good.png", "great.png"
	};

	if (path[path.size() - 1] != '/')
		path += "/";
	std::cout << "Creating new skin with path " << path << std::endl;
	for (unsigned int i = eSkinComponent::SK_NOTE; i < eSkinComponent::SK_SIZE; (int)i++)
	{
		sf::Texture*	t_tmp = new sf::Texture();

		std::cout << "Creating new component with path " << path + components[i] << std::endl;
		if (!t_tmp->loadFromFile(path + components[i]))
			t_tmp->loadFromFile(SKIN_DIR"/Default/" + components[i]);
		this->_textures.push_back(t_tmp);
	}
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
const sf::Texture&	Skin::getComponent(eSkinComponent index) const
{
	return (*this->_textures[index]);
}