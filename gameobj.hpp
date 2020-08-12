#ifndef _GAMEOBJ
#define _GAMEOBJ

#include <SFML/Graphics.hpp>


class Player {
public:
	Player(const sf::Texture& t_player, unsigned int x, unsigned int y);
	void move(sf::Vector2i mpos, unsigned int hor_width, unsigned int ver_height, sf::Vector2u windowSize);
	inline void moveTo(sf::Vector2f pos)
	{
		sprite.setPosition(pos);
	}
	void draw(sf::RenderWindow& window);
	inline sf::Sprite& getSprite() { return sprite; }
	bool died;
private:
	sf::Sprite sprite;
	
};

class Wall {
public:
	Wall(sf::Sprite& spr);
	void drawWall(sf::RenderWindow& window);
	inline void setPosition(unsigned int x, unsigned int y) {
		sprite.setPosition(x, y);
	}
protected:
	sf::Sprite sprite;
};

class LaserWall : public Wall {
public:
	enum LaserDirection {
		Vertical,
		Horizontal
	};
	LaserWall(sf::Sprite& spr, LaserDirection direction, const sf::Texture& _off, const sf::Texture& _next, const sf::Texture& _activated);

	virtual void drawWall(sf::RenderWindow& window, float thick);
	void ready();
	void off();
	void active();
	bool laser(sf::Sprite& player_spr, sf::RenderWindow& window, float thick);
private:
	bool activating;
	LaserDirection direction;
	const sf::Texture& t_off;
	const sf::Texture& t_next;
	const sf::Texture& t_activated;
};

#endif // _GAMEOBJ
