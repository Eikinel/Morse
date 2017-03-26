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