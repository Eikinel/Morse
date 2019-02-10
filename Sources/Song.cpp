#include "Song.h"
#include "Note.h"
#include "Phase.h"
#include "Bezier.h"
#include <fstream>

Song::Song(const std::string& file, unsigned int& speed) : _speed(speed)
{
	std::cout << "Creating song from file " << file << std::endl;

	// To do : Parse a song file

	this->_bpm = 139.f;
	this->_song_offset_skip = sf::Time::Zero;
	if (!this->_music.openFromFile(SONG_DIR + file))
		std::cerr << "Can't open file with path : " << file << std::endl;
	
}

Song::~Song()
{
	std::cout << "Deleting song" << std::endl;

	for (auto it : this->_hit_sounds)
		delete(it);
	for (auto it : this->_phases)
		delete(it);
}

// GETTERS
const float&	Song::getBPM() const
{
	return (this->_bpm);
}

sf::Music&	Song::getMusic()
{
	return (this->_music);
}

const sf::Time&	Song::getSongOffsetSkip() const
{
	return (this->_song_offset_skip);
}

const std::vector<Phase *>&	Song::getPhases() const
{
	return (this->_phases);
}

const Phase*	Song::getPhaseByTime(const sf::Time& time) const
{
	Phase*	phase = NULL;

	for (auto it : this->_phases)
	{
		if (it->getTime() <= time)
			phase = it;
		else
			break;
	}

	return (phase);
}

const std::vector<std::shared_ptr<Note>>&	Song::getNotes() const
{
	return (this->_notes);
}

const std::vector<std::shared_ptr<Note>>	Song::getNextNotes(const sf::Time& time) const
{
	std::vector<std::shared_ptr<Note>>	next_notes;

	for (auto& it : this->_notes)
	{
		if ((it->getTiming().asSeconds() - time.asSeconds()) < MAX_TIMING_VIEW / this->_speed)
			next_notes.emplace_back(it);
		else
			break;
	}

	return (next_notes);
}

const std::vector<std::shared_ptr<Note>>	Song::getNotesWithSameTiming(const sf::Time& time, const float& length) const
{
	std::vector<std::shared_ptr<Note>>	same_timing;

	for (auto it : this->_notes)
		if (it->getTiming().asSeconds() >= time.asSeconds() && it->getTiming().asSeconds() <= length)
			same_timing.emplace_back(it);

	return (same_timing);
}

const Note&	Song::getNoteByIndex(unsigned int index) const
{
	return (*this->_notes[index]);
}

std::vector<std::unique_ptr<Bezier>>&	Song::getBezierCurves()
{
	return (this->_bezier_curves);
}


// SETTERS
void	Song::setBPM(const float& bpm)
{
	this->_bpm = bpm;
}

void	Song::setMusic(const std::string& file)
{
	this->_music.openFromFile(file);
}

void	Song::setSongOffset(const sf::Time& offset)
{
	this->_song_offset_skip = offset;
	this->_music.setPlayingOffset(this->_song_offset_skip);
}


// METHODS
void	Song::removeNote(const Note& note)
{
	for (auto& it = this->_notes.begin(); it != this->_notes.end(); ++it)
	{
		if (**it == note)
		{
			this->_notes.erase(it);
			return;
		}
	}
	std::cerr << "Note not found" << std::endl;
}

void	Song::removeBezierCurve(std::vector<std::unique_ptr<Bezier>>::const_iterator& it)
{
	this->_bezier_curves.erase(it);
	std::cerr << "Erased bezier_curve" << std::endl;
}

void	Song::restart(std::vector<const sf::Texture *>& textures)
{
	// Remove remaning element such as phases, curves and notes
	// Delete properly to free memory
	for (auto it = this->_phases.begin(); it != this->_phases.end();)
	{
		delete(*it);
		it = this->_phases.erase(it);
	}

	for (auto it = this->_bezier_curves.begin(); it != this->_bezier_curves.end();)
	{
		it = this->_bezier_curves.erase(it);
	}

	for (auto it = this->_notes.begin(); it != this->_notes.end();)
	{
		it = this->_notes.erase(it);
	}

	// Empty the vector
	this->_phases = std::vector<Phase *>();
	this->_bezier_curves = std::vector<std::unique_ptr<Bezier>>();
	this->_notes = std::vector<std::shared_ptr<Note>>();

	Bezier test({
		sf::Vector2f(0, 0),
		sf::Vector2f(-200, -200),
		sf::Vector2f(-100, -350),
		sf::Vector2f(200, -200) },
		2.f,
		this->_bpm);

	this->_phases.push_back(new Phase(ePhase::ATTACK, sf::seconds(0.f)));
	this->_notes.push_back(std::make_shared<Note>(sf::seconds(3.f), 0.f));
	this->_notes.push_back(std::make_shared<Note>(sf::seconds(3.5f), 0.f));
	this->_notes.push_back(std::make_shared<Note>(sf::seconds(5.f), 0.f));
	this->_notes.push_back(std::make_shared<Note>(sf::seconds(6.f), 0.f));
	this->_notes.push_back(std::make_shared<Note>(sf::seconds(7.f), 0.f));
	this->_notes.push_back(std::make_shared<Note>(sf::seconds(8.f), 0.f));
	this->_bezier_curves.emplace_back(new Bezier(test));

	this->_phases.push_back(new Phase(ePhase::DEFENSE, sf::seconds(21.f)));
	this->_notes.push_back(std::make_shared<Note>(sf::seconds(23.1f), 0.f, sf::Vector2i(0, 1)));
	this->_notes.push_back(std::make_shared<Note>(sf::seconds(23.5f), 1.7f, sf::Vector2i(0, -1)));

	this->_notes.push_back(std::make_shared<Note>(sf::seconds(25.4f), 0.f, sf::Vector2i(0, 1)));
	this->_notes.push_back(std::make_shared<Note>(sf::seconds(25.6f), 0.f, sf::Vector2i(-1, 0)));
	this->_notes.push_back(std::make_shared<Note>(sf::seconds(25.8f), 0.f, sf::Vector2i(1, 0)));
	this->_notes.push_back(std::make_shared<Note>(sf::seconds(26.f), 0.f, sf::Vector2i(0, -1)));

	this->_notes.push_back(std::make_shared<Note>(sf::seconds(26.5f), 0.f, sf::Vector2i(-1, 0)));
	this->_notes.push_back(std::make_shared<Note>(sf::seconds(26.7f), 0.f, sf::Vector2i(0, 1)));
	this->_notes.push_back(std::make_shared<Note>(sf::seconds(26.9f), 1.7f, sf::Vector2i(0, -1)));

	std::cout << this->_speed << std::endl;

	for (auto it : this->_notes)
	{
		it->setSprites(textures);

		if (it->getDuration() > 0.f)
			it->scaleLongNote(this->_speed);
	}

	this->_music.play();
	this->setSongOffset(sf::Time::Zero);
}