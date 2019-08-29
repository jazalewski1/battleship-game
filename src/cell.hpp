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
		sf::Vector2f m_center;

	protected:
		sf::Vector2f m_pos;


	protected:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			target.draw(m_shape, states);
		}


	public:
		Cell(sf::Vector2f pos) :
			m_shape{sf::Vector2f{g_cellsize, g_cellsize}}, m_size{g_cellsize, g_cellsize},
			m_center{pos.x + g_cellsize / 2.0f, pos.y + g_cellsize / 2.0f}, m_pos{pos}
		{
			m_shape.setPosition(pos);
			m_shape.setFillColor(sf::Color::Transparent);
			m_shape.setOutlineColor(sf::Color::White);
			m_shape.setOutlineThickness(1.0f);
		}

		void setPosition(sf::Vector2f pos)
		{
			m_pos = pos;
			m_center.x = m_pos.x + g_cellsize / 2.0f;
			m_center.y = m_pos.y + g_cellsize / 2.0f;
		}

		void setFillColor(sf::Color color) { m_shape.setFillColor(color); }
		void setOutlineColor(sf::Color color) { m_shape.setOutlineColor(color); }
};

}

#endif