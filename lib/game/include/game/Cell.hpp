#ifndef CELL_HPP
#define CELL_HPP

#include "SFML/Graphics.hpp"
#include "common/Common.hpp"
#include "gui/Guitext.hpp"

namespace game
{
class Cell : public sf::Drawable
{
public:
	Cell(sf::Vector2i index) :
		shape{sf::Vector2f{common::cell_size, common::cell_size}}, size{common::cell_size, common::cell_size}, index{index},
		position{index.x * common::cell_size, index.y * common::cell_size}, center{common::index_to_center_position(index)},
		default_color{sf::Color::Transparent}, color_on_hover{245, 242, 201, 100}, color_on_select{245, 242, 201, 180}
	{
		shape.setPosition(position);
		shape.setFillColor(sf::Color::Transparent);
		shape.setOutlineColor(sf::Color{245, 242, 201});
		shape.setOutlineThickness(0.5f);
	}

	Cell(int indexX, int indexY) : Cell{sf::Vector2i{indexX, indexY}}
	{
	}

	Cell() : Cell{sf::Vector2i{0, 0}}
	{
	}

	virtual bool contains(sf::Vector2f vector)
	{
		return shape.getLocalBounds().contains(vector);
	}

	virtual bool contains(float position_x, float position_y)
	{
		return contains(sf::Vector2f{position_x, position_y});
	}

	virtual sf::Vector2i get_index() const
	{
		return index;
	}

	void put_default_color()
	{
		shape.setFillColor(default_color);
	}

	void put_hover_color()
	{
		shape.setFillColor(color_on_hover);
	}

	void put_select_color()
	{
		shape.setFillColor(color_on_select);
	}

protected:
	sf::RectangleShape shape;
	const sf::Vector2f size;
	const sf::Vector2i index;
	const sf::Vector2f position;
	const sf::Vector2f center;
	sf::Color default_color;
	sf::Color color_on_hover;
	sf::Color color_on_select;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(shape, states);
	}
};

class LabelCell : public Cell
{
public:
	LabelCell(sf::Vector2i index, char symbol) :
		Cell{index}, text{center}, symbol{symbol}
	{
		shape.setOutlineThickness(0.0f);

		text.setFont(common::font);
		text.setString(symbol);
		text.alignToCenter();
		text.setFillColor(color_on_select);
	}
	LabelCell(int indexX, int indexY, char symbol) :
		LabelCell{sf::Vector2i{indexX, indexY}, symbol}
	{
	}

	char get_symbol() const
	{
		return symbol;
	}

private:
	Gui::Text text;
	char symbol;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		Cell::draw(target, states);
		target.draw(text);
	}
};
} // namespace game

#endif