#include "Skin.h"
#include <regex>

Skin::Skin(std::string path)
{
	std::string		components[2] =
	{ "note.png", "note_outline.png" };

	if (path[path.size() - 1] != '/')
		path += "/";
	std::cout << "Creating new skin with path " << path << std::endl;
	for (unsigned int i = eSkinComponent::NOTE; i < eSkinComponent::SKIN_SIZE; (int)i++)
	{
		sf::Texture*	t_tmp = new sf::Texture();
		sf::Sprite*		s_tmp = new sf::Sprite();

		std::cout << "Creating new component with path " << path + components[i] << std::endl;
		if (!t_tmp->loadFromFile(path + components[i]))
			t_tmp->loadFromFile(SKIN_DIR"/Default/" + components[i]);
		s_tmp->setTexture(*t_tmp);
		this->_textures.push_back(t_tmp);
		this->_sprites.push_back(s_tmp);
	}
}

Skin::Skin(const Skin& other)
{
	std::cout << "Switching skins" << std::endl;
	this->_textures = other._textures;
	this->_sprites = other._sprites;
}

Skin::~Skin()
{
	std::cout << "Deleting skin" << std::endl;
	for (auto it : this->_textures)
		delete(it);
	for (auto it : this->_sprites)
		delete(it);
}


//GETTERS
const sf::Sprite&	Skin::getComponent(eSkinComponent index) const
{
	return (*this->_sprites[index]);
}


//SETTERS
/*void	Skin::setComponent(const std::string& path, eSkinComponent index)
{
	std::regex					re("[^/]+");
	std::vector<std::string>	file;
	std::string					type[2] =
	{ "Note", "Note outline" };
	sf::Texture*				t_tmp = new sf::Texture();
	sf::Sprite*					s_tmp = new sf::Sprite();

	std::cout << "Adding sprite of type " << type[(int)index] << " at place " << (int)index << std::endl;
	std::sregex_token_iterator
		begin(path.begin(), path.end(), re),
		end;
	std::copy(begin, end, std::back_inserter(file));

	if (!t_tmp->loadFromFile(path))
	{
		std::cerr << "Can't load file with path " << path << std::endl;
		if (!t_tmp->loadFromFile(SKIN_DIR"/Default/"))
			std::cerr << "Can't load default file with path " << SKIN_DIR"/Default/" << std::endl;
	}
	s_tmp->setTexture(*this->_textures[(int)index]);
	this->_textures.push_back(t_tmp);
	this->_sprites.push_back(s_tmp);
}*/