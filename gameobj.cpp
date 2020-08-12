#include "gameobj.hpp"

LaserWall::LaserWall(sf::Sprite& spr, LaserDirection direction, const sf::Texture& _off, const sf::Texture& _next, const sf::Texture& _activated) :
	Wall(spr), direction(direction),
	t_off(_off),
	t_next(_next),
	t_activated(_activated),
	activating(false)
{
	off();
}

void LaserWall::drawWall(sf::RenderWindow& window, float thick)
{
	window.draw(sprite);
	if (activating)
	{
		sf::RectangleShape line;
		if (direction == Horizontal)
		{
			line = sf::RectangleShape(sf::Vector2f(thick, window.getSize().y - sprite.getTexture()->getSize().y));
			line.setPosition(sprite.getPosition().x + sprite.getTexture()->getSize().x / 2 - (thick / 2), sprite.getPosition().y + sprite.getTexture()->getSize().y);
		}
		else
		{
			line = sf::RectangleShape(sf::Vector2f(window.getSize().x - sprite.getTexture()->getSize().x, thick));
			line.setPosition(sprite.getPosition().x + sprite.getTexture()->getSize().x, sprite.getPosition().y + sprite.getTexture()->getSize().y / 2 - (thick / 2));
		}
		line.setFillColor(sf::Color::Red);
		window.draw(line);
	}
}

void LaserWall::ready()
{
	sprite.setTexture(t_next);
}

void LaserWall::off()
{
	sprite.setTexture(t_off);
	activating = false;
}

bool LaserWall::laser(sf::Sprite& player_spr, sf::RenderWindow& window, float thick)
{
	sf::RectangleShape line;

	sf::FloatRect player_rect = player_spr.getGlobalBounds();
	if (direction == Horizontal)
	{
		line = sf::RectangleShape(sf::Vector2f(thick, window.getSize().y - sprite.getTexture()->getSize().y));
		line.setPosition(sprite.getPosition().x + sprite.getTexture()->getSize().x / 2, sprite.getPosition().y + sprite.getTexture()->getSize().y);
	}
	else
	{
		line = sf::RectangleShape(sf::Vector2f(window.getSize().x - sprite.getTexture()->getSize().x, thick));
		line.setPosition(sprite.getPosition().x + sprite.getTexture()->getSize().x, sprite.getPosition().y + sprite.getTexture()->getSize().y / 2);
	}
	if (player_rect.intersects(line.getGlobalBounds())) return true;
	else return false;
}

void LaserWall::active()
{
	sprite.setTexture(t_activated);
	activating = true;
}

Wall::Wall(sf::Sprite& spr) : sprite(spr)
{
}

void Wall::drawWall(sf::RenderWindow& window)
{
	window.draw(sprite);
}

Player::Player(const sf::Texture& t_player, unsigned int x, unsigned int y) : sprite(sf::Sprite(t_player)), died(false)
{
	sprite.setPosition(sf::Vector2f(x,y));
}

void Player::move(sf::Vector2i mpos, unsigned int hor_width, unsigned int ver_height, sf::Vector2u windowSize)
{
	if (died) return;
	sf::Vector2f curr = sprite.getPosition();
	sf::Vector2f cent;
	cent.x += sprite.getTexture()->getSize().x / 2;
	cent.y += sprite.getTexture()->getSize().y / 2;
	sf::Vector2f v;
	v.x = mpos.x - curr.x - cent.x;
	v.y = mpos.y - curr.y - cent.y;



	float dist = sqrt((curr.x + cent.x - mpos.x) * (curr.x + cent.x - mpos.x) + (curr.y + cent.y - mpos.y) * (curr.y + cent.y - mpos.y));
	if (!dist) dist = 0.01;
	v /= dist;
	float w = std::min(dist, std::max(dist/ 30, 2.5f));

	v *= w;

	sprite.setPosition(curr.x + v.x, curr.y + v.y);

	v = sprite.getPosition();
	v += cent;
	if (v.x < cent.x + 32) {
		v.x = cent.x + 32;
	}
	if (v.y < cent.y + 32) {
		v.y = cent.y + 32;
	}

	if (v.x > windowSize.x - 1 - cent.x) {
		v.x = windowSize.x - 1 - cent.x;
	}
	if (v.y > windowSize.y - 1 - cent.y) {
		v.y = windowSize.y - 1 - cent.y;
	}


	sprite.setPosition(v - cent);

}

void Player::draw(sf::RenderWindow& window)
{
	if (!died)
		window.draw(sprite);
}