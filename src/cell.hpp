#ifndef CELL_HPP
#define CELL_HPP

#include "SFML/Graphics.hpp"
#include "common.hpp"

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


	protected:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			target.draw(m_shape, states);
		}


	public:
		Cell(sf::Vector2i index) :
			m_shape{sf::Vector2f{g_cellsize, g_cellsize}}, m_size{g_cellsize, g_cellsize},
			m_index{index}, m_pos{index.x * g_cellsize, index.y * g_cellsize}
		{
			setPosition(m_pos);
			m_shape.setFillColor(sf::Color::Transparent);
			m_shape.setOutlineColor(sf::Color::White);
			m_shape.setOutlineThickness(1.0f);
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

		virtual void setPosition(sf::Vector2f pos)
		{
			m_pos = pos;
			m_center.x = m_pos.x + g_cellsize / 2.0f;
			m_center.y = m_pos.y + g_cellsize / 2.0f;
			m_shape.setPosition(m_pos);
			m_index = ftoi(m_pos);
		}
		virtual void setIndex(sf::Vector2i index)
		{
			m_index = index;
			sf::Vector2f newPos {m_index.x * g_cellsize, m_index.y * g_cellsize};
			setPosition(newPos);
		}
		virtual void setCenter(sf::Vector2f pos)
		{
			m_center = pos;
			sf::Vector2f newPos {m_center.x - g_cellsize / 2.0f, m_center.y - g_cellsize / 2.0f};
			setPosition(newPos);
		}
		virtual void setFillColor(sf::Color color) { m_shape.setFillColor(color); }

		virtual sf::Vector2i getIndex() const { return m_index; }
		virtual sf::Vector2f getPosition() const { return m_pos; }
		virtual sf::Vector2f getCenter() const { return m_center; }

		virtual bool operator==(sf::Vector2i index) { return m_index == index; }
		virtual bool operator!=(sf::Vector2i index) { return m_index != index; }
};



class BoardCell : public Cell
{
	private:
		sf::Color m_defaultColor;
		sf::Color m_hoverColor;

	public:
		BoardCell(sf::Vector2i index) :
			Cell{index}, m_defaultColor{sf::Color::Transparent}, m_hoverColor{sf::Color::Green}
		{
		}
		BoardCell(int indexX, int indexY) :
			BoardCell{sf::Vector2i{indexX, indexY}}
		{
		}

		void defaultColor() { m_shape.setFillColor(m_defaultColor); }
		void hoverColor() { m_shape.setFillColor(m_hoverColor); }
};

}

#endif