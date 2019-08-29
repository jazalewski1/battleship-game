#ifndef CELL_HPP
#define CELL_HPP

#include "SFML/Graphics.hpp"
#include "common.hpp"

namespace Game
{

class Cell : public sf::Drawable
{
	private:
		sf::RectangleShape m_shape;
		sf::Vector2f m_size;
		sf::Vector2i m_index;
		sf::Vector2f m_pos;
		sf::Vector2f m_center;

		sf::Color m_defaultFillColor;
		sf::Color m_hoverFillColor;


	protected:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			target.draw(m_shape, states);
		}


	public:
		Cell(sf::Vector2i index) :
			m_shape{sf::Vector2f{g_cellsize, g_cellsize}}, m_size{g_cellsize, g_cellsize},
			m_index{index}, m_pos{index.x * g_cellsize, index.y * g_cellsize},
			m_defaultFillColor{sf::Color::Transparent}, m_hoverFillColor{sf::Color::Red}
		{
			setPosition(m_pos);
			m_shape.setFillColor(m_defaultFillColor);
			m_shape.setOutlineColor(sf::Color::White);
			m_shape.setOutlineThickness(1.0f);
		}
		Cell() :
			Cell{sf::Vector2i{0, 0}}
		{
		}



		void setPosition(sf::Vector2f pos)
		{
			m_pos = pos;
			m_center.x = m_pos.x + g_cellsize / 2.0f;
			m_center.y = m_pos.y + g_cellsize / 2.0f;
			m_shape.setPosition(m_pos);
		}
		void setIndex(sf::Vector2i index)
		{
			m_index = index;
			sf::Vector2f newPos {m_index.x * g_cellsize, m_index.y * g_cellsize};
			setPosition(newPos);
		}


		void setFillDefault(sf::Color color)
		{
			m_defaultFillColor = color;
			m_shape.setFillColor(m_defaultFillColor);
		}
		void setFillHover(sf::Color color)
		{
			m_defaultFillColor = color;
			m_shape.setFillColor(m_defaultFillColor);
		}

		void defaultColor() { m_shape.setFillColor(m_defaultFillColor); }
		void hoverColor() { m_shape.setFillColor(m_hoverFillColor); }
};

}

#endif