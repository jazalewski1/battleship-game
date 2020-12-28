#ifndef CELL_HPP
#define CELL_HPP

#include "SFML/Graphics.hpp"
#include "common/Common.hpp"
#include "gui/Guitext.hpp"

namespace Game
{

class Cell : public sf::Drawable
{
	protected:
		sf::RectangleShape m_shape;
		sf::Vector2f m_size;
		sf::Vector2i m_index;
		sf::Vector2f m_pos;
		sf::Vector2f m_center;

		sf::Color m_defaultColor;
		sf::Color m_hoverColor;
		sf::Color m_selectColor;


	protected:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			target.draw(m_shape, states);
		}


	public:
		Cell(sf::Vector2i index) :
			m_shape{sf::Vector2f{g_cellsize, g_cellsize}}, m_size{g_cellsize, g_cellsize},
			m_index{index}, m_pos{index.x * g_cellsize, index.y * g_cellsize}, m_center{itoc(index)},
			m_defaultColor{sf::Color::Transparent}, m_hoverColor{245, 242, 201, 100}, m_selectColor{245, 242, 201, 180}
		{
			m_shape.setPosition(m_pos);
			m_shape.setFillColor(sf::Color::Transparent);
			m_shape.setOutlineColor(sf::Color{245, 242, 201});
			m_shape.setOutlineThickness(0.5f);
		}
		Cell(int indexX, int indexY) :
			Cell{sf::Vector2i{indexX, indexY}}
		{
		}
		Cell() :
			Cell{sf::Vector2i{0, 0}}
		{
		}

		virtual bool contains(sf::Vector2f test) { return m_shape.getLocalBounds().contains(test); }
		virtual bool contains(float posX, float posY) { return contains(sf::Vector2f{posX, posY}); }

		virtual sf::Vector2i getIndex() const { return m_index; }
		virtual sf::Vector2f getPosition() const { return m_pos; }
		virtual sf::Vector2f getCenter() const { return m_center; }

		void defaultColor() { m_shape.setFillColor(m_defaultColor); }
		void hoverColor() { m_shape.setFillColor(m_hoverColor); }
		void selectColor() { m_shape.setFillColor(m_selectColor); }
};


class LabelCell : public Cell
{
	private:
		Gui::Text m_text;
		char m_symbol;

	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override
		{
			Cell::draw(target, states);
			target.draw(m_text);
		}

	public:
		LabelCell(sf::Vector2i index, char symbol) :
			Cell{index}, m_text{m_center}, m_symbol{symbol}
		{
			m_shape.setOutlineThickness(0.0f);

			m_text.setFont(g_font);
			m_text.setString(m_symbol);
			m_text.alignToCenter();
			m_text.setFillColor(m_selectColor);
		}
		LabelCell(int indexX, int indexY, char symbol) :
			LabelCell{sf::Vector2i{indexX, indexY} ,symbol}
		{
		}

		char getSymbol() const { return m_symbol; }
};

}

#endif