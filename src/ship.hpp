#ifndef SHIP_HPP
#define SHIP_HPP

#include "SFML/Graphics.hpp"
#include "common.hpp"

class Ship : public sf::Drawable
{
	private:
		enum class Rot {HOR, VER};

	private:
		sf::RectangleShape m_shape;
		sf::Vector2i m_offset;
		sf::Vector2i m_size;
		sf::IntRect m_bounds;
		sf::Vector2f m_pos;
		sf::Vector2f m_center;

	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			target.draw(m_shape, states);
		}


	public:
		Ship(sf::Vector2i offset, int length) :
			m_shape{sf::Vector2f{length * g_cellsize, g_cellsize}},
			m_offset{offset}, m_size{length, 1}, m_bounds{offset.x, offset.y, length, 1},
			m_pos{itof(offset)}
		{
			m_center.x = m_pos.x + (m_shape.getSize().x * 0.5f);
			m_center.y = m_pos.y + (m_shape.getSize().y * 0.5f);

			m_shape.setPosition(m_pos);
			m_shape.setFillColor(sf::Color{120, 120, 120});
			m_shape.setOutlineColor(sf::Color{160, 160, 160});
			m_shape.setOutlineThickness(3.0f);
		}
		Ship(int offsetX, int offsetY, int length) :
			Ship{sf::Vector2i{offsetX, offsetY}, length}
		{
		}


		void setCenter(sf::Vector2f center)
		{
			m_center = center;
			m_pos.x = m_center.x - (m_shape.getSize().x * 0.5f);
			m_pos.y = m_center.y - (m_shape.getSize().y * 0.5f);
			m_shape.setPosition(m_pos);
		}
		void setCenter(float posX, float posY) { setCenter(sf::Vector2f{posX, posY}); }

		void adjust(sf::Vector2f center)
		{
			setCenter(center);
			m_offset = ftoi(sf::Vector2f{m_pos.x + (g_cellsize * 0.5f), m_center.y});
			m_pos = itof(m_offset);
			m_shape.setPosition(m_pos);
			m_bounds = sf::IntRect{m_offset, m_size};
		}
		void adjust(float posX, float posY) { adjust(sf::Vector2f{posX, posY}); }

		bool contains(sf::Vector2i index) { return m_bounds.contains(index); }
};



#endif