#pragma once

# include <SFML\Graphics.hpp>
# include <vector>
# include <iostream>

# define GAME_NAME	"Morse"

// Directories
# define ASSETS_DIR		"../Assets"
# define FONTS_DIR		ASSETS_DIR"/Fonts"
# define SKIN_DIR		ASSETS_DIR"/Skin"

// Game component
# define MAX_SPEED			100
# define MAX_TIMING_VIEW	10.f

// Global variable for screens, containing menu, game, etc.
class IScreen;
extern std::vector<IScreen *> all_screens;