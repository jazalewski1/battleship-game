#ifndef MARKER_HPP
#define MARKER_HPP

#include "SFML/Graphics.hpp"
#include "common/Common.hpp"

class Marker : public sf::Drawable
{
public:
	enum class Type
	{
		HIT,
		MISS,
		GUESS
	};

	Marker(sf::Vector2i index, Type type) :
		shape{common::cell_size * 0.3f}, index{index}, pos{common::index_to_center_position(index)},
		type{type}
	{
		shape.setPointCount(16);
		shape.setOrigin(shape.getRadius(), shape.getRadius());
		shape.setPosition(pos);
		update_color();
	}

	void set_type(Type type)
	{
		type = type;
		update_color();
	}

	bool is_hit() const
	{
		return type == Type::HIT;
	}

	bool is_guess() const
	{
		return type == Type::GUESS;
	}

private:
	sf::CircleShape shape;
	const sf::Vector2i index;
	const sf::Vector2f pos;
	Type type;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(shape, states);
	}

	void update_color()
	{
		if (type == Type::HIT)
		{
			shape.setFillColor(sf::Color::Red);
		}
		if (type == Type::MISS)
		{
			shape.setFillColor(sf::Color::White);
		}
		if (type == Type::GUESS)
		{
			shape.setFillColor(sf::Color{255, 255, 255, 40});
		}
	}
};

#endif