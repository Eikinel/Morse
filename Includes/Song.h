#pragma once

#include "Constants.h"
#include <SFML\Audio.hpp>

class Phase;
class Bezier;
class Note;

enum		eFileParse
{
	PROPERTIES,
	PHASES,
	NOTES,
	FILE_PARSE_SIZE
};

class		Song
{
public:
	Song(const std::string& file, unsigned int& speed);
	~Song();

	// GETTERS
	const float&								getBPM() const;
	sf::Music&									getMusic();
	const sf::Time&								getSongOffsetSkip() const;
	const std::vector<Phase *>&					getPhases() const;
	const Phase*								getPhaseByTime(const sf::Time& time) const;
	const std::vector<std::shared_ptr<Note>>&	getNotes() const;
	const std::vector<std::shared_ptr<Note>>	getNextNotes(const sf::Time& time) const;
	const std::vector<std::shared_ptr<Note>>	getNotesWithSameTiming(const sf::Time& time, const float& length) const;
	const Note&									getNoteByIndex(unsigned int index) const;
	std::vector<std::unique_ptr<Bezier>>&		getBezierCurves();

	// SETTERS
	void	setBPM(const float& bpm);
	void	setMusic(const std::string& file);
	void	setSongOffset(const sf::Time& offset);

	// METHODS
	void	removeNote(const Note& note);
	void	removeBezierCurve(std::vector<std::unique_ptr<Bezier>>::const_iterator& it);
	void	restart(std::vector<const sf::Texture *>& textures);

private:
	float									_bpm;
	unsigned int&							_speed;
	sf::Music								_music;
	sf::Time								_song_offset_skip; // The offset value WHEN the song was skipped
	std::vector<sf::Sound *>				_hit_sounds;
	std::vector<Phase *>					_phases;
	std::vector<std::shared_ptr<Note>>		_notes;
	std::vector<std::unique_ptr<Bezier>>	_bezier_curves;
};