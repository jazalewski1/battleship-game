#ifndef SHIP_HPP
#define SHIP_HPP

#include "SFML/Graphics.hpp"
#include "common/Common.hpp"

namespace game
{
class Ship : public sf::Drawable
{
public:
	Ship(sf::Vector2i offset, int length) :
		shape{sf::Vector2f{length * common::cell_size, common::cell_size}}, offset{offset}, size{length, 1},
		bounds{offset.x, offset.y, length, 1}, center{common::index_to_screen_position(offset) + (shape.getSize() * 0.5f)}, rotation{HORIZONTAL}
	{
		shape.setOrigin(shape.getSize() * 0.5f);

		shape.setPosition(center);
		shape.setFillColor(sf::Color{200, 200, 200});
		shape.setOutlineColor(sf::Color{100, 100, 100});
		shape.setOutlineThickness(3.0f);
	}

	Ship(int offset_x, int offset_y, int length) : Ship{sf::Vector2i{offset_x, offset_y}, length}
	{
	}

	void adjust(sf::Vector2f center)
	{
		const auto diff = sf::Vector2f{(shape.getSize().x * 0.5f) - (common::cell_size * 0.5f), (shape.getSize().y * 0.5f) - (common::cell_size * 0.5f)};
		offset = common::screen_position_to_index(center - diff);
		bounds = sf::IntRect{offset, size};
		center = common::index_to_screen_position(offset) + (shape.getSize() * 0.5f);
		shape.setPosition(center);
	}

	void adjust(float pos_x, float pos_y)
	{
		adjust(sf::Vector2f{pos_x, pos_y});
	}

	bool contains(sf::Vector2i index) const
	{
		return bounds.contains(index);
	}

	bool contains(int index_x, int index_y) const
	{
		return bounds.contains(sf::Vector2i{index_x, index_y});
	}

	void rotate()
	{
		if (rotation == HORIZONTAL)
		{
			rotation = VERTICAL;

			size.y = size.x;
			size.x = 1;
		}
		else
		{
			rotation = HORIZONTAL;

			size.x = size.y;
			size.y = 1;
		}
		shape.setOrigin(shape.getSize() * -0.5f);
		shape.setSize(sf::Vector2f{size.x * common::cell_size, size.y * common::cell_size});
		shape.setOrigin(shape.getSize() * 0.5f);
	}

	sf::IntRect get_bounds() const
	{
		return bounds;
	}

	sf::IntRect get_ghost_bounds() const
	{
		const auto diff = sf::Vector2f{(shape.getSize().x * 0.5f) - (common::cell_size * 0.5f), (shape.getSize().y * 0.5f) - (common::cell_size * 0.5f)};
		const auto offset = sf::Vector2i{common::screen_position_to_index(center - diff)};
		return sf::IntRect{offset, size};
	}

	void set_center(sf::Vector2f center)
	{
		center = center;
		shape.setPosition(center);
	}

	void set_offset(sf::Vector2i offset)
	{
		offset = offset;
		bounds = sf::IntRect{offset, size};
		center = common::index_to_screen_position(offset) + (shape.getSize() * 0.5f);
		shape.setPosition(center);
	}

private:
	enum Rotation
	{
		HORIZONTAL,
		VERTICAL
	};

	sf::RectangleShape shape;
	sf::Vector2i offset;
	sf::Vector2i size;
	sf::IntRect bounds;
	sf::Vector2f center;
	Rotation rotation;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(shape, states);
	}
};
} // namespace game
#endif