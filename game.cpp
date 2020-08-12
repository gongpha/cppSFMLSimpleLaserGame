#include <SFML/Graphics.hpp>
#include <random>
#include <vector>
#include <numeric>
#include <iostream>

// Hello, Objects
#include "gameobj.hpp"

// Standard values for the game
#define LEVEL_ROUND 3
#define LASER_COUNT 20
#define MAX_LEVEL 30

// DON'T CHANGE IF YOU'RE JUST RUNNING ON THE ROAD
#define FORMULA (level % MAX_LEVEL) * ((LASER_COUNT - 3) / (float)MAX_LEVEL) + 1

// Randoms lasers with above formula
std::vector<unsigned int> randomLaser(std::vector<LaserWall>& laserWalls, unsigned int level)
{
	std::vector<unsigned int> list(LASER_COUNT);
	std::mt19937 rng(std::random_device{}());
	std::iota(begin(list), end(list), 0);
	std::shuffle(begin(list), end(list), rng);

	return std::vector<unsigned int>(list.begin(), list.begin() + FORMULA);
}

// Shortcut for loading assets
#define LOAD_FROM_FILE(c,d) if(!c.loadFromFile(d)){std::cout<< "Error ! Cannot load \""<<d<<"\" !"<<std::endl;return-1;}

unsigned int level;
unsigned int round_;
float time_;
unsigned int evt;
float laserThickness;
sf::Clock game_clock;
sf::Text levelLabel;

// For fun. if added on the game start. Like nextLevelBy(99999)
void nextLevelBy(int lvl)
{
	round_ = 1;
	level += lvl;
	int l = level / MAX_LEVEL;
	time_ = 0.5f * l;
	laserThickness = std::min(2 * (l+1), 24);
	levelLabel.setString(std::to_string(level));
	sf::FloatRect rect = levelLabel.getLocalBounds();
	levelLabel.setOrigin(rect.left + rect.width / 2.0f, rect.top + rect.height / 2.0f);
}

int main()
{
	// Makes window
	sf::RenderWindow window(sf::VideoMode((LASER_COUNT + 1) * 32, (LASER_COUNT + 1) * 32), "cppLaserGame - Laser Rush", sf::Style::Titlebar | sf::Style::Close);

	// Limits FPS to 60. if i wasn't set here. It costs GPU usage over 50% with 2000fps+
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);

	// Prepares for textures and font
	sf::Texture t_Wall;
	LOAD_FROM_FILE(t_Wall, "Assets/wall.png")sf::Sprite s_Wall;
	s_Wall.setTexture(t_Wall, true);

	sf::Texture t_laserWall;
	LOAD_FROM_FILE(t_laserWall, "Assets/wall_indicator.png")sf::Sprite s_laserWall;
	s_laserWall.setTexture(t_laserWall, true);

	sf::Texture t_laserWallNext;
	LOAD_FROM_FILE(t_laserWallNext, "Assets/wall_indicator_next.png")sf::Sprite s_laserWallNext;
	s_laserWallNext.setTexture(t_laserWallNext, true);

	sf::Texture t_laserWallActivated;
	LOAD_FROM_FILE(t_laserWallActivated, "Assets/wall_indicator_activated.png")sf::Sprite s_laserWallActivated;
	s_laserWallActivated.setTexture(t_laserWallActivated, true);

	sf::Texture t_player;
	LOAD_FROM_FILE(t_player, "Assets/player.png")

		sf::Font g_font;
	LOAD_FROM_FILE(g_font, "Assets/SourceCodePro-Light.ttf");

	/*
	EVT (Event type) Definition
		0 = READY
		1 = FIRE LESER
		2 = RESET
		3 = GAME OVER
	*/

	// Creates Laser walls on horizontal and vertical
	std::vector<LaserWall> laserwalls_hor(LASER_COUNT, LaserWall(s_laserWall, LaserWall::Horizontal, t_laserWall, t_laserWallNext, t_laserWallActivated));
	std::vector<LaserWall> laserwalls_ver(LASER_COUNT, LaserWall(s_laserWall, LaserWall::Vertical, t_laserWall, t_laserWallNext, t_laserWallActivated));
	std::vector<LaserWall*> laserwalls_tof;
	Player player(t_player, window.getSize().x / 2, window.getSize().y / 2);
	const unsigned int spr_w = t_laserWall.getSize().x;
	const unsigned int spr_h = t_laserWall.getSize().y;
	unsigned int curr = 1;
	// Place each to the room
	for (auto& l : laserwalls_hor)
	{
		l.setPosition(curr * spr_w, 0);
		curr++;
	}
	curr = 1;
	for (auto& l : laserwalls_ver)
	{
		l.setPosition(0, curr * spr_h);
		curr++;
	}
	sf::FloatRect rect;

	// The jumper for restarting a game
reset:
	player.moveTo(sf::Vector2f(window.getSize().x / 2, window.getSize().y / 2));
	player.died = false;
	game_clock.restart();
	level = 1;
	round_ = 1;
	evt = 0;
	time_ = 0;
	laserThickness = 2;
	levelLabel.setFont(g_font);
	levelLabel.setString(std::to_string(level));
	levelLabel.setCharacterSize(144);
	levelLabel.setFillColor(sf::Color(200, 200, 200));
	rect = levelLabel.getLocalBounds();
	levelLabel.setOrigin(rect.left + rect.width / 2.0f, rect.top + rect.height / 2.0f);
	levelLabel.setPosition(window.getSize().x / 2, window.getSize().y / 2);
	while (window.isOpen())
	{
		switch (evt)
		{
		case 0:
			// Prepares lasers for activating
			if (game_clock.getElapsedTime().asSeconds() >= std::max((2.0f - (float)round_ / LEVEL_ROUND) - time_, 1.0f))
			{
				for (auto i : randomLaser(laserwalls_hor, level))
				{
					laserwalls_hor[i].ready();
					laserwalls_tof.push_back(&laserwalls_hor[i]);
				}
				for (auto i : randomLaser(laserwalls_ver, level))
				{
					laserwalls_ver[i].ready();
					laserwalls_tof.push_back(&laserwalls_ver[i]);
				}
				game_clock.restart();
				evt++;
			}
			break;
		case 1:
			// Activates lasers
			if (game_clock.getElapsedTime().asSeconds() >= std::max((2.0f - (float)round_ / LEVEL_ROUND) - time_, 1.0f))
			{
				for (auto l : laserwalls_tof)
				{
					l->active();
				}
				game_clock.restart();
				evt++;
			}
			break;
		case 2:
			// Check if player touches one of lasers
			for (auto l : laserwalls_tof)
			{
				
				if (l->laser(player.getSprite(), window, laserThickness))
				{
					// Yes, You are lose. Waiting for respecting at case 3
					player.died = true;
					levelLabel.setString("GAME OVER");
					levelLabel.setCharacterSize(72);
					sf::FloatRect rect = levelLabel.getLocalBounds();
					levelLabel.setOrigin(rect.left + rect.width / 2.0f, rect.top + rect.height / 2.0f);
					evt = 3;
					game_clock.restart();
					break;
				}
			}

			// Wait for 2 seconds
			if (game_clock.getElapsedTime().asSeconds() >= 2.0f)
			{
				// Inactivates lasers
				for (auto l : laserwalls_tof)
				{
					l->off();
				}
				laserwalls_tof.clear();

				// If round counter is touched LEVEL_ROUND, Go to the next level and reset a round counter
				if (round_ < LEVEL_ROUND)
					round_++;
				else
				{
					nextLevelBy(1);
				}
				game_clock.restart();
				evt = 0;
			}
		case 3:
			// After showing GAME OVER text for 3 seconds
			if (game_clock.getElapsedTime().asSeconds() >= 3.0f)
			{
				// Restart a game
				for (auto l : laserwalls_tof)
				{
					l->off();
				}
				laserwalls_tof.clear();
				goto reset;
			}
		}

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// Moving a player to the mouse position
		player.move(sf::Mouse::getPosition(window), t_laserWall.getSize().x, t_laserWall.getSize().y, window.getSize());

		window.clear();
		// Draw some floor. Made with TEXT
		window.draw(levelLabel);

		// Draw walls
		for (auto& l : laserwalls_hor)
		{
			l.drawWall(window, laserThickness);
		}
		for (auto& l : laserwalls_ver)
		{
			l.drawWall(window, laserThickness);
		}

		// Draw useless wall on the corner
		window.draw(s_Wall);
		// Draw me
		player.draw(window);
		window.display();
	}

	return 0;
}