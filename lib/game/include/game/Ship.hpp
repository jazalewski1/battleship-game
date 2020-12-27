#ifndef SHIP_HPP
#define SHIP_HPP

#include "SFML/Graphics.hpp"
#include "Common.hpp"

class Ship : public sf::Drawable
{
	private:
		enum Rotation {HOR, VER};

	private:
		sf::RectangleShape m_shape;
		sf::Vector2i m_offset;
		sf::Vector2i m_size;
		sf::IntRect m_bounds;
		sf::Vector2f m_center;
		Rotation m_rotation;

	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			target.draw(m_shape, states);
		}


	public:
		Ship(sf::Vector2i offset, int length) :
			m_shape{sf::Vector2f{length * g_cellsize, g_cellsize}},
			m_offset{offset}, m_size{length, 1}, m_bounds{offset.x, offset.y, length, 1},
			m_center{itof(offset) + (m_shape.getSize() * 0.5f)}, m_rotation{HOR}
		{
			m_shape.setOrigin(m_shape.getSize() * 0.5f);

			m_shape.setPosition(m_center);
			m_shape.setFillColor(sf::Color{200, 200, 200});
			m_shape.setOutlineColor(sf::Color{100, 100, 100});
			m_shape.setOutlineThickness(3.0f);
		}
		Ship(int offsetX, int offsetY, int length) :
			Ship{sf::Vector2i{offsetX, offsetY}, length}
		{
		}


		void adjust(sf::Vector2f center)
		{
			sf::Vector2f diff {(m_shape.getSize().x * 0.5f) - (g_cellsize * 0.5f), (m_shape.getSize().y * 0.5f) - (g_cellsize * 0.5f)};
			m_offset = ftoi(center - diff);
			m_bounds = sf::IntRect{m_offset, m_size};
			m_center = itof(m_offset) + (m_shape.getSize() * 0.5f);
			m_shape.setPosition(m_center);
		}
		void adjust(float posX, float posY) { adjust(sf::Vector2f{posX, posY}); }

		bool contains(sf::Vector2i index) const { return m_bounds.contains(index); }
		bool contains(int indexX, int indexY) const { return m_bounds.contains(sf::Vector2i{indexX, indexY}); }

		void rotate()
		{
			if(m_rotation == HOR)
			{
				m_rotation = VER;

				m_size.y = m_size.x;
				m_size.x = 1;
			}
			else
			{
				m_rotation = HOR;

				m_size.x = m_size.y;
				m_size.y = 1;
			}
			m_shape.setOrigin(m_shape.getSize() * -0.5f);
			m_shape.setSize(sf::Vector2f{m_size.x * g_cellsize, m_size.y * g_cellsize});
			m_shape.setOrigin(m_shape.getSize() * 0.5f);
		}


		sf::IntRect getBounds() const { return m_bounds; }
		sf::IntRect getGhostBounds() const
		{
			sf::Vector2f diff {(m_shape.getSize().x * 0.5f) - (g_cellsize * 0.5f), (m_shape.getSize().y * 0.5f) - (g_cellsize * 0.5f)};
			sf::Vector2i offset {ftoi(m_center - diff)};
			return sf::IntRect{offset, m_size};
		}


		void setCenter(sf::Vector2f center)
		{
			m_center = center;
			m_shape.setPosition(m_center);
		}
		void setCenter(float posX, float posY) { setCenter(sf::Vector2f{posX, posY}); }

		void setOffset(sf::Vector2i offset)
		{
			m_offset = offset;
			m_bounds = sf::IntRect{m_offset, m_size};
			m_center = itof(m_offset) + (m_shape.getSize() * 0.5f);
			m_shape.setPosition(m_center);
		}

};



#endif