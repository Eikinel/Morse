#include "Event.h"
#include "Screen.h"
#include "Button.h"
#include "Collider.h"
#include "Skin.h"
#include "Note.h"
#include "Phase.h"
#include "Bezier.h"
#include <iomanip>
#include <sstream>

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
	this->_game_clock.restart();

	//Convert second to milliseconds for proper timing gaps, then apply calcul
	float	max_timing_view_ms = MAX_TIMING_VIEW * 1000.f;
	this->_timing_gaps.push_back(sf::Vector2f(std::roundf(-max_timing_view_ms / 400.f), std::roundf(max_timing_view_ms / 400.f))); //GREAT
	this->_timing_gaps.push_back(sf::Vector2f(std::roundf(-max_timing_view_ms / 133.4f), std::roundf(max_timing_view_ms / 133.4f))); //GOOD
	this->_timing_gaps.push_back(sf::Vector2f(std::roundf(-max_timing_view_ms / 100.f), std::roundf(max_timing_view_ms / 100.f))); //BAD
	this->_timing_gaps.push_back(sf::Vector2f(std::roundf(-max_timing_view_ms / 66.7f), std::roundf(max_timing_view_ms / 66.7f))); //MISS

	this->_phases_events.push_back(new AttackEvent(*this)); // Subevent for the attack phase
	this->_phases_events.push_back(new DefenseEvent(*this)); // Subevent for the defense phase
}

AttackEvent::AttackEvent(GameEvent& gevent) : _gevent(gevent)
{
	std::cout << "Creating AttackEvent" << std::endl;

	this->_power = 1.f;
	this->_accelerator = 5.f;
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
int		WindowDefaultEvent::update(IScreen& screen, sf::Event& event)
{
	switch (event.type)
	{
	case sf::Event::Closed:
		return (EXIT);
		break;
	default:
		break;
	}

	return (screen.getIndex());
}

int		MenuEvent::update(IScreen& screen, sf::Event& event)
{
	MenuScreen*	mscreen = static_cast<MenuScreen *>(&screen);

	switch (event.type)
	{
	case sf::Event::KeyPressed:
		switch (event.key.code)
		{
		case sf::Keyboard::Escape:
			return (EXIT);
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
			int status = mscreen->getIndex();
				
			if (it->getText().getGlobalBounds().contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y)))
				if ((status = it->triggerEvent()) != screen.getIndex())
					return (status);
		}
		break;
	default:
		break;
	}

	return (screen.getIndex());
}

void		MenuEvent::draw(IScreen& screen)
{
	MenuScreen*	mscreen = static_cast<MenuScreen *>(&screen);

	for (auto it : mscreen->getButtons())
		mscreen->draw(it->getText());
	mscreen->draw(mscreen->getFPSText());
}

int		GameEvent::update(IScreen& screen, sf::Event& event)
{
	GameScreen*	gscreen = static_cast<GameScreen *>(&screen);
	int			status = screen.getIndex();
	
	this->_current_phase = gscreen->getPhaseByTime(this->_game_clock.getElapsedTime());
	this->_next_notes = gscreen->getNextNotes(this->_game_clock.getElapsedTime());

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

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			return (this->changeScreen(eGamestate::MENU, gscreen));

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
			std::cout << "T1 : " << this->_game_clock.getElapsedTime().asSeconds() << std::endl;

		break;
	}

	if (this->_current_phase != NULL)
	{
		gscreen->setPhaseText(this->_current_phase->getName());
		status = this->_phases_events[this->_current_phase->getType()]->update(screen, event);
		this->_phases_events[this->_current_phase->getType()]->draw(screen);
	}

	return (status);
}

void	GameEvent::draw(IScreen& screen)
{
	GameScreen*	gscreen = static_cast<GameScreen *>(&screen);

	/*for (auto it : this->_next_notes)
		for (auto it2 : it->getSprites())
			gscreen->draw(*it2);*/

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


int		AttackEvent::update(IScreen& screen, sf::Event& event)
{
	GameScreen*	gscreen = static_cast<GameScreen *>(&screen);
	sf::Sprite	arrow = gscreen->getArrow();
	const std::vector<Note *>&	next_notes = this->_gevent.getNextNotes();
	const sf::Time& game_elapsed = this->_gevent.getGameClock().getElapsedTime();

	// Give notes a position
	for (auto it : next_notes)
	{
		const std::vector<sf::Sprite *>	tmp = it->getSprites();
	}

	const std::vector<Bezier *> curves = gscreen->getBezierCurves();

	for (auto it : curves)
	{
		const sf::VertexArray curve = it->getBezierCurve();

			//gscreen->getCursor().getPosition().x + ((game_elapsed.asSeconds()) * gscreen->getSpeed() * (gscreen->getWindow().getSize().x / (2.f * MAX_TIMING_VIEW))) * curve[1].position.x - curve[0].position.x,
			//gscreen->getCursor().getPosition().x + ((game_elapsed.asSeconds()) * gscreen->getSpeed() * (gscreen->getWindow().getSize().x / (2.f * MAX_TIMING_VIEW))) * curve[1].position.y - curve[0].position.y));
	}


	//Check user action
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		this->_is_accelerated = true;
	else
		this->_is_accelerated = false;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		float	arrow_direction = 0.f;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			arrow_direction = -1.f;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			arrow_direction = 1.f;

		this->_rot_arrow.rotate(
			arrow_direction * this->_power * (this->_is_accelerated ? this->_accelerator : 1),
			gscreen->getCursor().getPosition());
	}

	return (screen.getIndex());
}

void	AttackEvent::draw(IScreen& screen)
{
	GameScreen*	gscreen = static_cast<GameScreen *>(&screen);

	sf::CircleShape circle(5.f);
	circle.setFillColor(sf::Color(255, 0, 0));

	for (auto curve : gscreen->getBezierCurves())
		gscreen->draw(curve->getBezierCurve(), this->_pos_curve);

	gscreen->draw(gscreen->getArrow(), this->_rot_arrow);
}


int		DefenseEvent::update(IScreen& screen, sf::Event& event)
{
	GameScreen*	gscreen = static_cast<GameScreen *>(&screen);
	const std::vector<Note *>&	next_notes = this->_gevent.getNextNotes();
	const sf::Time& game_elapsed = this->_gevent.getGameClock().getElapsedTime();

	// Give notes a position
	for (auto it : next_notes)
	{
		const std::vector<sf::Sprite *>	tmp = it->getSprites();

		tmp[0]->setPosition(sf::Vector2f(
			gscreen->getCursor().getPosition().x + ((it->getTime().asSeconds() - game_elapsed.asSeconds()) * gscreen->getSpeed() * (gscreen->getWindow().getSize().x / (2.f * MAX_TIMING_VIEW))) * it->getDirection().x,
			gscreen->getCursor().getPosition().y + ((it->getTime().asSeconds() - game_elapsed.asSeconds()) * gscreen->getSpeed() * (gscreen->getWindow().getSize().y / (2.f * MAX_TIMING_VIEW))) * it->getDirection().y));
		tmp[1]->setPosition(tmp[0]->getPosition());

		if (it->getDuration() > 0.f)
		{
			sf::Color	playing(it->getBaseColor());

			//If the long note reach the cursor
			if (it->getTime().asSeconds() - game_elapsed.asSeconds() <= 0.f)
			{
				it->scaleDuration(game_elapsed);
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
			std::vector<Note *>	same_timing = gscreen->getNotesWithSameTiming(next_notes[0]->getTime(), next_notes[0]->getLength());

			for (auto it : same_timing)
			{
				sf::Vector2i	direction = it->getDirection();
				sf::Time		note_time = it->getTime();
				sf::Time		delta_accuracy = sf::seconds(this->_gevent.getTimingGaps()[this->_gevent.getTimingGaps().size() - 1].y);
				sf::Clock		delay;

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && direction.y == -1)
					delta_accuracy = note_time - game_elapsed;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && direction.x == -1)
					delta_accuracy = note_time - game_elapsed;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && direction.y == 1)
					delta_accuracy = note_time - game_elapsed;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && direction.x == 1)
					delta_accuracy = note_time - game_elapsed;

				if (delta_accuracy > sf::milliseconds(this->_gevent.getTimingGaps()[this->_gevent.getTimingGaps().size() - 1].x) &&
					delta_accuracy < sf::milliseconds(this->_gevent.getTimingGaps()[this->_gevent.getTimingGaps().size() - 1].y))
				{
					eAccuracy	accuracy = this->_gevent.getAccuracy(delta_accuracy - delay.getElapsedTime());

					std::cout << "Delta accuracy : " << delta_accuracy.asMilliseconds() - delay.getElapsedTime().asMilliseconds() << std::endl;

					if (it->getDuration() == 0.f)
					{
						gscreen->removeNote(*it);
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
				eAccuracy		accuracy;

				if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && direction.y == -1)
					delta_accuracy = note_length - game_elapsed;
				if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && direction.x == -1)
					delta_accuracy = note_length - game_elapsed;
				if (!sf::Keyboard::isKeyPressed(sf::Keyboard::S) && direction.y == 1)
					delta_accuracy = note_length - game_elapsed;
				if (!sf::Keyboard::isKeyPressed(sf::Keyboard::D) && direction.x == 1)
					delta_accuracy = note_length - game_elapsed;

				if (delta_accuracy < sf::seconds(this->_gevent.getTimingGaps()[this->_gevent.getTimingGaps().size() - 1].y))
				{
					std::cout << "Note length = " << note_length.asSeconds() << std::endl;
					std::cout << "Time elapsed = " << game_elapsed.asSeconds() << std::endl;
					std::cout << "Delta accuracy released : " << delta_accuracy.asMilliseconds() - delay.getElapsedTime().asMilliseconds() << std::endl;

					if (!it->hasBeenHeld())
					{
						if ((accuracy = this->_gevent.getAccuracy(delta_accuracy - delay.getElapsedTime())) == eAccuracy::ACC_MISS)
							accuracy = eAccuracy::ACC_BAD;
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

	//If the note hit the death bar and hasn't been played, delete it and set accuracy depending on short or long note (held or not)
	for (auto it = next_notes.begin(); it != next_notes.end();)
	{
		if ((*it)->getTime().asMilliseconds() - game_elapsed.asMilliseconds() < this->_gevent.getTimingGaps()[this->_gevent.getTimingGaps().size() - 1].x)
		{
			//A long not that reach the death bar always has its duration set to 0.
			if ((*it)->getDuration() == 0.f)
			{
				//hasBeenHeld is set to "true" if the user played a long note and released the key.
				//That means if the long note reached the death bar but the user didn't released the key, he will have a "bad".
				//It also means that short note (that cannot be hold) will always display a "miss" if it reach the death bar.
				if (!(*it)->hasBeenHeld())
				{
					if ((*it)->isHeld())
						gscreen->addAccuracy(eAccuracy::ACC_BAD);
					else
						gscreen->addAccuracy(eAccuracy::ACC_MISS);
				}
				gscreen->removeNote(**it);
				it = this->_gevent.removeNote(**it);
			}
			else
				++it;
		}
		else
			++it;
	}

	return (screen.getIndex());
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

const sf::Clock&	GameEvent::getGameClock() const
{
	return (this->_game_clock);
}

const std::vector<Note *>&	GameEvent::getNextNotes() const
{
	return (this->_next_notes);
}

const std::vector<sf::Vector2f>	GameEvent::getTimingGaps() const
{
	return (this->_timing_gaps);
}

const eAccuracy	GameEvent::getAccuracy(const sf::Time& delta) const
{
	int	accuracy = eAccuracy::ACC_GREAT;

	for (auto it : this->_timing_gaps)
	{
		if (delta.asMilliseconds() >= it.x && delta.asMilliseconds() <= it.y)
			return ((eAccuracy)accuracy);
		accuracy--;
	}
	return (eAccuracy::ACC_MISS);
}


//SETTERS
void	IEvent::setToggleOptions(const std::vector<bool>& toggle_options)
{
	this->_toggle_options = toggle_options;
}


//METHODS
int	IEvent::changeScreen(eGamestate gamestate, IScreen* screen)
{
	std::string	type[3] = { "menu", "options", "game" };
	int	index;

	if (gamestate == EXIT)
		return (gamestate);
	
	// Check if a screen with the gamestate passed as parameter already exist.
	// If true, copy options and return the gamestate.
	// Else, create the screen before performing operations.
	for (std::vector<IScreen *>::const_iterator it = all_screens.begin(); it != all_screens.end(); ++it)
	{
		if ((*it)->getState() == gamestate)
		{
			if ((*it)->getEvents()[1]->getToggleOptions() != this->_toggle_options)
			{
				std::cout << "Replacing old options" << std::endl;
				(*it)->getEvents()[1]->setToggleOptions(this->_toggle_options);
			}
			return ((*it)->getIndex());
		}
	}
	index = this->createScreen(gamestate, screen);
	std::cout << "Switching screen to " << type[(int)gamestate] << " at index " << index << "." << std::endl;
	return (index);
}

int	IEvent::createScreen(eGamestate gamestate, IScreen* screen)
{
	std::string	type[3] = { "menu", "options", "game" };
	IScreen*	new_screen = NULL;

	std::cout << std::endl << "Creating new " << type[(int)gamestate] << "." << std::endl;

	if (gamestate == MENU)
		new_screen = new MenuScreen(screen->getWindow());
	else if (gamestate == OPTIONS)
		new_screen = NULL;//new OptionsScreen(*window);
	else
		new_screen = new GameScreen(screen->getWindow());
	new_screen->getEvents()[1]->setToggleOptions(this->_toggle_options);
	all_screens.push_back(new_screen);
	return (new_screen->getIndex());
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
}


//MISCELLANEOUS
std::string	setPrecision(float value, unsigned int precision)
{
	std::ostringstream	oss;

	oss << std::fixed << std::setfill('0') << std::setprecision(precision) << value;

	return (oss.str());
}