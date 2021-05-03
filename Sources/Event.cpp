#include "Event.h"
#include "Screen.h"
#include "Button.h"
#include "Collider.h"
#include "Skin.h"
#include "Note.h"
#include "Phase.h"
#include "Bezier.h"
#include "Song.h"
#include <iomanip>
#include <sstream>

#define _USE_MATH_DEFINES
#include <math.h>

//CONSTRUCTORS
IEvent::IEvent()
{
	std::cout << "Creating default options" << std::endl;
	this->_toggle_options.push_back(false); // On/off bounding boxes draw
	this->_general_clock.restart().asSeconds();
}

WindowDefaultEvent::WindowDefaultEvent()
{
	std::cout << "Creating window default event" << std::endl;
}

MenuEvent::MenuEvent()
{
	std::cout << "Creating menu event" << std::endl;
}

GameEvent::GameEvent()
{
	std::cout << "Creating game event" << std::endl;

	this->_timing_gaps.push_back(sf::Vector2f(-25.f, 25.f)); //GREAT
	this->_timing_gaps.push_back(sf::Vector2f(-50.f, 50.f)); //GOOD
	this->_timing_gaps.push_back(sf::Vector2f(-100.f, 100.f)); //BAD
	this->_timing_gaps.push_back(sf::Vector2f(-150.f, 150.f)); //MISS

	this->_phases_events.push_back(new AttackEvent(*this)); // Subevent for the attack phase
	this->_phases_events.push_back(new DefenseEvent(*this)); // Subevent for the defense phase

	this->_metronome_played = false;
}

AttackEvent::AttackEvent(GameEvent& gevent) : _gevent(gevent)
{
	std::cout << "Creating AttackEvent" << std::endl;

	this->_arrow_angle = 0.f;
	this->_arrow_trans = sf::Transform();
}

DefenseEvent::DefenseEvent(GameEvent& gevent) : _gevent(gevent)
{
	std::cout << "Creating DefenseEvent" << std::endl;
}


GameEvent::~GameEvent()
{
	std::cout << "Deleting GameEvent subevents" << std::endl;

	for (auto it : this->_phases_events)
		delete(it);
}

AttackEvent::~AttackEvent()
{
	std::cout << "Deleting AttackEvent" << std::endl;
}

DefenseEvent::~DefenseEvent()
{
	std::cout << "Deleting DefenseEvent" << std::endl;
}


//UPDATE & DRAW METHODS
GAMESTATE	WindowDefaultEvent::update(IScreen& screen, sf::Event& event)
{
	switch (event.type)
	{
	case sf::Event::Closed:
		return (GAMESTATE::EXIT);
		break;
	default:
		break;
	}

	return screen.getState();
}

GAMESTATE	MenuEvent::update(IScreen& screen, sf::Event& event)
{
	MenuScreen*	mscreen = static_cast<MenuScreen *>(&screen);

	switch (event.type)
	{
	case sf::Event::KeyPressed:
		switch (event.key.code)
		{
		case sf::Keyboard::Escape:
			return (GAMESTATE::EXIT);
			break;
		default:
			break;
		}
		break;
	case sf::Event::MouseButtonReleased:
		std::cout << "MenuEvent : Click !" << std::endl;
		std::cout << "Position : [" << event.mouseButton.x << " ; " << event.mouseButton.y << "]." << std::endl;

		for (auto it : mscreen->getButtons())
		{
			GAMESTATE state = mscreen->getState();
				
			if (it->getText().getGlobalBounds().contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y)))
				if ((state = (GAMESTATE)it->triggerEvent()) != screen.getState())
					return state;
		}
		break;
	default:
		break;
	}

	return screen.getState();
}

void		MenuEvent::draw(IScreen& screen)
{
	MenuScreen*	mscreen = static_cast<MenuScreen *>(&screen);

	for (auto it : mscreen->getButtons())
		mscreen->draw(it->getText());
	mscreen->draw(mscreen->getFPSText());
}

GAMESTATE	GameEvent::update(IScreen& screen, sf::Event& event)
{
	GameScreen*	gscreen = static_cast<GameScreen *>(&screen);
	Song&		song = gscreen->getSong();
	GAMESTATE	state = screen.getState();
	sf::Time	elapsed = this->_game_clock.getElapsedTime() + song.getSongOffsetSkip() - this->_skip_freeze;
	
	this->_next_notes = song.getNextNotes(elapsed);
	this->_song_elapsed = this->getGameClock().getElapsedTime() + song.getSongOffsetSkip() - this->getSkipFreeze();

	switch (event.type)
	{
	case sf::Event::KeyPressed:
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F3))
				gscreen->addSpeed(-1);

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F4))
				gscreen->addSpeed(1);

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			{
				gscreen->restart();
				this->restart();
			}
		}

		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			return (GAMESTATE)this->changeScreen(GAMESTATE::MENU, gscreen);

		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
			std::cout << "T1 : " << elapsed.asSeconds() << std::endl;

		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			if (song.getNotes().size() > 0) {
				const std::shared_ptr<Note>& note = song.getNotes()[0];

				if (note->getTiming() - elapsed > sf::seconds(3.f)) {
					this->_skip_freeze = this->_game_clock.getElapsedTime();
					song.setSongOffset(note->getTiming() - sf::seconds(3.f));
				}
			}
		}

		break;
	}

	this->_current_phase = song.getPhaseByTime(elapsed);

	if (this->_current_phase != NULL)
	{
		gscreen->setPhaseText(this->_current_phase->getName());
		state = this->_phases_events[this->_current_phase->getType()]->update(screen, event);
		this->_phases_events[this->_current_phase->getType()]->draw(screen);
	}

	// For test purpose : play a sound as a metronome depending on the BPM
	if ((int)elapsed.asMilliseconds() % (int)(1 / (song.getBPM() / 60.f) * 1000) <= 20.f) {
		if (!this->_metronome_played) {
			gscreen->getMetronome().play();
			this->_metronome_played = true;
		}
	}
	else
		this->_metronome_played = false;
	
	// If the note hit the death bar and hasn't been played, delete it and set accuracy depending on short or long note (held or not)
	for (auto& it : this->_next_notes)
	{
		if (this->_song_elapsed.asMilliseconds() - it->getTiming().asMilliseconds() > this->_timing_gaps[this->_timing_gaps.size() - 1].y)
		{
			//A long not that reach the death bar always has its duration set to 0.
			if (it->getDuration() == 0.f)
			{
				// hasBeenHeld is set to "true" if the user played a long note and released the key.
				// That means if the long note reached the death bar but the user didn't released the key, he will have a "bad".
				// It also means that short notes (that cannot be hold) will always display a "miss" if it reaches the death bar.
				if (!it->hasBeenHeld())
				{
					if (it->isHeld())
						gscreen->addAccuracy(ACCURACY::ACC_BAD);
					else
						gscreen->addAccuracy(ACCURACY::ACC_MISS);
				}
				song.removeNote(*it);
			}
		}
	}

	return state;
}

void	GameEvent::draw(IScreen& screen)
{
	GameScreen*	gscreen = static_cast<GameScreen *>(&screen);
	sf::CircleShape shape(5.f);

	for (auto it : this->getNextNotes())
		for (auto it2 : it->getSprites())
			gscreen->draw(*it2);

	gscreen->draw(gscreen->getCursor());
	gscreen->draw(gscreen->getFPSText());
	gscreen->draw(gscreen->getSpriteAccuracy());
	gscreen->draw(sf::Text(setPrecision(gscreen->getUserAccuracy(), 2) + " %", gscreen->getMainFont()));
	gscreen->draw(gscreen->getPhaseText());
}

auto	GameEvent::removeNote(const Note& note)
{
	for (auto it = this->_next_notes.begin(); it != this->_next_notes.end(); ++it)
	{
		if (**it == note)
		{
			auto owo = this->_next_notes.erase(it);
			std::cout << "Note (event side) deleted" << std::endl;

			return owo;
		}
	}
	std::cerr << "Note (event side) not found" << std::endl;
}


GAMESTATE	AttackEvent::update(IScreen& screen, sf::Event& event)
{
	GameScreen*	gscreen = static_cast<GameScreen *>(&screen);
	Song& song = gscreen->getSong();
	const sf::Time song_elapsed = this->_gevent.getSongElapsed();
	const std::vector<std::shared_ptr<Note>>& next_notes = this->_gevent.getNextNotes();

	// Mouse events
	const sf::RenderWindow& window = gscreen->getWindow();
	const sf::Vector2f& cursor_pos = gscreen->getCursor().getPosition();
	sf::Vector2i mouse_pos = sf::Vector2i();
	sf::Vector2f vmouse = sf::Vector2f();

	// We should not process each curve everytime
	// Instead, we need to retrieve drawable curves, depending on the pixel length and speed
	// Additionaly, it is important to know if any curve exists before drawing notes, because a note cannot exist without a curve
	std::vector<std::unique_ptr<Bezier>>& curves = song.getBezierCurves();


	for (int i = 0; i < curves.size(); i++)
	{
		std::unique_ptr<Bezier>& curve = curves[i];
		std::vector<sf::Vertex>& vertices = curve->getBezierCurve();

		// Get the vector director of the curve
		sf::Vector2f vcurve = vertices[1].position - vertices[0].position;
		sf::Vector2f vcurve_norm = vcurve / (sqrt(pow(vcurve.x, 2) + pow(vcurve.y, 2)));

		// Get time elapsed until the first point of the curve hit the cursor
		sf::Time curve_elapsed = song_elapsed - curve->getStart();
		//std::cout << curve_elapsed.asSeconds() << std::endl;

		const int vertexIndex = curve->getVertexIndexByTiming(song_elapsed);
		sf::Transform curve_transform = sf::Transform();

		// Set curve's position according to time spent and vector
		const sf::Vector2f curve_position = sf::Vector2f(
			// Expected position of first point - offset from current point
			cursor_pos.x - vertices[0].position.x + (curve_elapsed.asSeconds() > 0 ? 0 : -curve_elapsed.asSeconds() * song.getBPM() * vcurve_norm.x),
			cursor_pos.y - vertices[0].position.y + (curve_elapsed.asSeconds() > 0 ? 0 : -curve_elapsed.asSeconds() * song.getBPM() * vcurve_norm.y)
		);
		this->_jaj = curve_position;

		curve_transform.translate(curve_position);
		curve->setTransform(curve_transform);

		if (vertexIndex > 0) {
			curve->removeVertexAt(vertexIndex);

			if (vertices.size() <= 2)
				song.removeBezierCurveAt(i);
		}

		// Give notes a position
		for (auto it : next_notes)
		{
			const std::vector<sf::Sprite *>	tmp = it->getSprites();
			const sf::Vertex& point = curve->getVertexByTiming(it->getTiming());

			if (point.position != sf::Vertex().position)
			{
				tmp[0]->setPosition(point.position + curve_position);
				tmp[1]->setPosition(tmp[0]->getPosition());
			}
		}
	}
	
	switch (event.type)
	{
	case sf::Event::MouseMoved:
		// Set arrow rotation depending on the mouse position
		mouse_pos = sf::Mouse::getPosition(window);
		vmouse = sf::Vector2f(
			mouse_pos.x - cursor_pos.x,
			mouse_pos.y - cursor_pos.y);

		// Dot product of two vectors : from cursor to mouse and arrow direction
		// Replace the arrow at origin direction
		// atan2 use dot product and determinant. atan2(dot, det);
		// Considering sf::Vector2f origin = sf::Vector2f(1, 0); we have :
		// dot = vmouse.x * origin.x + vmouse.y * origin.y = vmouse.x * 1 + vmouse.y * 0 = vmouse.x
		// det = vmouse.x * origin.y - vmouse.y * origin.x = vmouse.x * 0 - vmouse.y * 1 = -vmouse.y
		// Convert from radians to degrees and here we go
		this->_arrow_trans = sf::Transform();
		this->_arrow_angle = 180 / M_PI * atan2(-vmouse.y, vmouse.x) * -1;
		this->_arrow_trans.rotate(this->_arrow_angle, cursor_pos);

		break;
	case sf::Event::KeyPressed:
		if (next_notes.size() > 0)
		{
			std::vector<std::shared_ptr<Note>>	same_timing = song.getNotesWithSameTiming(next_notes[0]->getTiming(), next_notes[0]->getLength());

			for (auto it : same_timing)
			{
				sf::Vector2i	direction = it->getDirection();
				sf::Time		note_time = it->getTiming();
				sf::Time		delta_accuracy = sf::seconds(this->_gevent.getTimingGaps()[this->_gevent.getTimingGaps().size() - 1].y);
				sf::Clock		delay;

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
					delta_accuracy = note_time - song_elapsed;

				if (delta_accuracy > sf::milliseconds(this->_gevent.getTimingGaps()[this->_gevent.getTimingGaps().size() - 1].x) &&
					delta_accuracy < sf::milliseconds(this->_gevent.getTimingGaps()[this->_gevent.getTimingGaps().size() - 1].y))
				{
					ACCURACY	accuracy = this->_gevent.getAccuracy(delta_accuracy - delay.getElapsedTime());

					std::cout << "Delta accuracy : " << delta_accuracy.asMilliseconds() - delay.getElapsedTime().asMilliseconds() << std::endl;

					if (it->getDuration() == 0.f)
					{
						song.removeNote(*it);
						gscreen->addAccuracy(accuracy);
					}
					else
						it->setHeld(true);
				}
			}
		}

		break;
	default:
		break;
	}

	return screen.getState();
}

void	AttackEvent::draw(IScreen& screen)
{
	GameScreen*	gscreen = static_cast<GameScreen *>(&screen);
	size_t i = 0;

	for (auto& it : gscreen->getSong().getBezierCurves()) {
		const std::vector<sf::Vertex>& vertices = it->getBezierCurve();
		//const std::vector<sf::Vector2f>& points = it->getPoints();

		gscreen->draw(vertices.data(), vertices.size(), sf::LineStrip, it->getTransform());

		/*for (auto& point : points) {
			sf::CircleShape shape(2);

			shape.setFillColor(sf::Color(255, 0, 0));
			shape.setPosition(point + this->_jaj);
			gscreen->draw(shape);
		}*/
	}

	gscreen->draw(gscreen->getArrowRadiusShape());
	gscreen->draw(gscreen->getArrow(), this->_arrow_trans);
}


GAMESTATE	DefenseEvent::update(IScreen& screen, sf::Event& event)
{
	GameScreen*	gscreen = static_cast<GameScreen *>(&screen);
	Song& song = gscreen->getSong();
	const std::vector<std::shared_ptr<Note>>&	next_notes = this->_gevent.getNextNotes();
	const sf::Time& song_elapsed = this->_gevent.getSongElapsed();

	// Give notes a position
	for (auto it : next_notes)
	{
		const std::vector<sf::Sprite *>	tmp = it->getSprites();

		tmp[0]->setPosition(sf::Vector2f(
			gscreen->getCursor().getPosition().x + ((it->getTiming().asSeconds() - song_elapsed.asSeconds()) * gscreen->getSpeed() * (gscreen->getWindow().getSize().x / (2.f * MAX_TIMING_VIEW))) * it->getDirection().x,
			gscreen->getCursor().getPosition().y + ((it->getTiming().asSeconds() - song_elapsed.asSeconds()) * gscreen->getSpeed() * (gscreen->getWindow().getSize().y / (2.f * MAX_TIMING_VIEW))) * it->getDirection().y));
		tmp[1]->setPosition(tmp[0]->getPosition());

		if (it->getDuration() > 0.f)
		{
			sf::Color	playing(it->getBaseColor());

			//If the long note reach the cursor
			if (it->getTiming().asSeconds() - song_elapsed.asSeconds() <= 0.f)
			{
				it->scaleDuration(song_elapsed);
				it->scaleLongNote(gscreen->getSpeed());
				tmp[0]->setPosition(sf::Vector2f(
					gscreen->getCursor().getPosition().x,
					gscreen->getCursor().getPosition().y));
				tmp[1]->setPosition(tmp[0]->getPosition());
				if (!it->isHeld())
					playing = sf::Color(0, 0, 255, 170);
			}

			tmp[2]->setPosition(sf::Vector2f(
				tmp[0]->getPosition().x + (tmp[0]->getGlobalBounds().width / 2.f * it->getDirection().x) - (tmp[2]->getGlobalBounds().width / 2 * it->getDirection().x),
				tmp[0]->getPosition().y + (tmp[0]->getGlobalBounds().height / 2.f * it->getDirection().y) - (tmp[2]->getGlobalBounds().height / 2 * it->getDirection().y)));
			tmp[3]->setPosition(tmp[2]->getPosition());

			for (unsigned int i = 4; i < tmp.size(); i += 2)
			{
				tmp[i]->setPosition(sf::Vector2f(
					tmp[i - 2]->getPosition().x + (tmp[i - 2]->getGlobalBounds().width / 2.f + tmp[i]->getGlobalBounds().width / 2.f) * it->getDirection().x,
					tmp[i - 2]->getPosition().y + (tmp[i - 2]->getGlobalBounds().height / 2.f + tmp[i]->getGlobalBounds().height / 2.f) * it->getDirection().y));
				tmp[i + 1]->setPosition(tmp[i]->getPosition());
			}

			if (tmp[0]->getColor() != playing)
				for (auto it2 : tmp)
					it2->setColor(playing);
		}
	}

	//Check user action
	switch (event.type)
	{
	case sf::Event::KeyPressed:
		if (next_notes.size() > 0)
		{
			std::vector<std::shared_ptr<Note>>	same_timing = song.getNotesWithSameTiming(next_notes[0]->getTiming(), next_notes[0]->getLength());

			for (auto it : same_timing)
			{
				sf::Vector2i	direction = it->getDirection();
				sf::Time		note_time = it->getTiming();
				sf::Time		delta_accuracy = sf::seconds(this->_gevent.getTimingGaps()[this->_gevent.getTimingGaps().size() - 1].y);
				sf::Clock		delay;

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && direction.y == -1)
					delta_accuracy = note_time - song_elapsed;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && direction.x == -1)
					delta_accuracy = note_time - song_elapsed;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && direction.y == 1)
					delta_accuracy = note_time - song_elapsed;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && direction.x == 1)
					delta_accuracy = note_time - song_elapsed;

				if (delta_accuracy > sf::milliseconds(this->_gevent.getTimingGaps()[this->_gevent.getTimingGaps().size() - 1].x) &&
					delta_accuracy < sf::milliseconds(this->_gevent.getTimingGaps()[this->_gevent.getTimingGaps().size() - 1].y))
				{
					ACCURACY	accuracy = this->_gevent.getAccuracy(delta_accuracy - delay.getElapsedTime());

					std::cout << "Delta accuracy : " << delta_accuracy.asMilliseconds() - delay.getElapsedTime().asMilliseconds() << std::endl;

					if (it->getDuration() == 0.f)
					{
						song.removeNote(*it);
						gscreen->addAccuracy(accuracy);
					}
					else
						it->setHeld(true);
				}
			}
		}

		break;
	case sf::Event::KeyReleased:
		for (auto it : next_notes)
		{
			if (it->getDuration() >= 0.f && it->isHeld())
			{
				sf::Vector2i	direction = it->getDirection();
				sf::Time		note_length = sf::seconds(it->getLength());
				sf::Time		delta_accuracy = sf::seconds(this->_gevent.getTimingGaps()[this->_gevent.getTimingGaps().size() - 1].y);
				sf::Clock		delay;
				ACCURACY		accuracy;

				if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && direction.y == -1)
					delta_accuracy = note_length - song_elapsed;
				if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && direction.x == -1)
					delta_accuracy = note_length - song_elapsed;
				if (!sf::Keyboard::isKeyPressed(sf::Keyboard::S) && direction.y == 1)
					delta_accuracy = note_length - song_elapsed;
				if (!sf::Keyboard::isKeyPressed(sf::Keyboard::D) && direction.x == 1)
					delta_accuracy = note_length - song_elapsed;

				if (delta_accuracy < sf::seconds(this->_gevent.getTimingGaps()[this->_gevent.getTimingGaps().size() - 1].y))
				{
					std::cout << "Note length = " << note_length.asSeconds() << std::endl;
					std::cout << "Time elapsed = " << song_elapsed.asSeconds() << std::endl;
					std::cout << "Delta accuracy released : " << delta_accuracy.asMilliseconds() - delay.getElapsedTime().asMilliseconds() << std::endl;

					if (!it->hasBeenHeld())
					{
						if ((accuracy = this->_gevent.getAccuracy(delta_accuracy - delay.getElapsedTime())) == ACCURACY::ACC_MISS)
							accuracy = ACCURACY::ACC_BAD;
						gscreen->addAccuracy(accuracy);
						it->setHeld(false);
						it->setBeenHeld(true);
					}
				}
			}
		}

		break;
	case sf::Event::MouseButtonReleased:
		std::cout << "GameEvent : Click !" << std::endl;
		std::cout << "Position : [" << event.mouseButton.x << " ; " << event.mouseButton.y << "]." << std::endl;
		break;
	default:
		break;
	}

	return screen.getState();
}

void	DefenseEvent::draw(IScreen& screen)
{
	GameScreen*	gscreen = static_cast<GameScreen *>(&screen);

	for (auto it : gscreen->getCross())
		gscreen->draw(it);
}


//GETTERS
bool	IEvent::getToggleBoundingBoxes()
{
	return (this->_toggle_options[0]);
}

std::vector<bool>&	IEvent::getToggleOptions()
{
	return (this->_toggle_options);
}

const sf::Clock& GameEvent::getGameClock() const
{
	return (this->_game_clock);
}

const sf::Time& GameEvent::getSongElapsed() const
{
	return (this->_song_elapsed);
}

const std::vector<std::shared_ptr<Note>>&	GameEvent::getNextNotes() const
{
	return (this->_next_notes);
}

const std::vector<sf::Vector2f>	GameEvent::getTimingGaps() const
{
	return (this->_timing_gaps);
}

const ACCURACY	GameEvent::getAccuracy(const sf::Time& delta) const
{
	int accuracy = (int)ACCURACY::ACC_GREAT;

	for (auto it : this->_timing_gaps)
	{
		if (delta.asMilliseconds() >= it.x && delta.asMilliseconds() <= it.y)
			return ((ACCURACY)accuracy);
		accuracy--;
	}
	return (ACCURACY::ACC_MISS);
}

const sf::Time&	GameEvent::getSkipFreeze() const
{
	return (this->_skip_freeze);
}


//SETTERS
void	IEvent::setToggleOptions(const std::vector<bool>& toggle_options)
{
	this->_toggle_options = toggle_options;
}


//METHODS
int		IEvent::changeScreen(GAMESTATE gamestate, IScreen* screen)
{
	std::string	type[3] = { "menu", "options", "game" };
	GAMESTATE	newState;

	if (gamestate == GAMESTATE::EXIT)
		return (int)gamestate;

	newState = this->createScreen(gamestate, screen);
	std::cout << "Switching screen to " << type[(int)gamestate] << " at index " << (int)newState << "." << std::endl;

	return (int)newState;
}

GAMESTATE	IEvent::createScreen(GAMESTATE gamestate, IScreen* screen)
{
	std::string	type[3] = { "menu", "options", "game" };
	std::shared_ptr<IScreen> new_screen = NULL;

	std::cout << std::endl << "Creating new " << type[(int)gamestate] << "." << std::endl;

	switch (gamestate) {
	case GAMESTATE::MENU:
		new_screen = std::make_shared<MenuScreen>(screen->getWindow());
		break;
	case GAMESTATE::OPTIONS:
		new_screen = std::make_shared<GameScreen>(screen->getWindow());
		break;
	case GAMESTATE::GAME:
		new_screen = std::make_shared<GameScreen>(screen->getWindow());
		break;
	}
	
	if (new_screen != NULL) {
		new_screen->getEvents()[1]->setToggleOptions(this->_toggle_options);
		all_screens[(int)gamestate] = new_screen;

		return new_screen->getState();
	}

	return screen->getState();
}

int		IEvent::toggleBoundingBoxes(int index)
{
	this->_toggle_options[0] = !this->_toggle_options[0];
	if (this->_toggle_options[0])
		std::cout << "Showing bounding boxes" << std::endl;
	else
		std::cout << "Hiding bounding boxes" << std::endl;
	return (index);
}


void	GameEvent::restart()
{
	this->_next_notes.clear();
	this->_game_clock.restart();
	this->_skip_freeze = sf::Time::Zero;
}


//MISCELLANEOUS
std::string	setPrecision(float value, unsigned int precision)
{
	std::ostringstream	oss;

	oss << std::fixed << std::setfill('0') << std::setprecision(precision) << value;

	return (oss.str());
}