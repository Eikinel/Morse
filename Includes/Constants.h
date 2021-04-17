#pragma once

# include <SFML\Graphics.hpp>
# include <iostream>
# include <memory>
# include <array>

# define GAME_NAME	"Morse"

// Directories
//   Assets
# define ASSETS_DIR					"../Assets"
# define FONTS_DIR					ASSETS_DIR"/Fonts"
# define SKIN_DIR					ASSETS_DIR"/Skins"
# define DEFAULT_SKIN_DIR			SKIN_DIR"/Default"
# define SKIN_SPRITE_DIR			"/Sprites"
# define SKIN_AUDIO_DIR				"/Sounds"

//	 Songs
# define SONG_DIR					"../Songs"

// Game component
# define MAX_SPEED			100
# define MAX_TIMING_VIEW	100.f

// Global variable for screens, containing menu, game, etc.
class IScreen;
extern std::array<std::shared_ptr<IScreen>, 3> all_screens;