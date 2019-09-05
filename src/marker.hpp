#ifndef MARKER_HPP
#define MARKER_HPP

#include "SFML/Graphics.hpp"
#include "common.hpp"


class Marker : public sf::Drawable
{
	private:
		sf::CircleShape m_shape;
		sf::Vector2i m_index;
		sf::Vector2f m_pos;
		bool m_hit;

	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			target.draw(m_shape, states);
		}

	public:
		Marker(sf::Vector2i index, bool isHit) :
			m_shape{g_cellsize * 0.3f}, m_index{index}, m_hit{isHit}
		{
			m_shape.setOrigin(m_shape.getRadius(), m_shape.getRadius());
			m_pos = itoc(index);
			m_shape.setPosition(m_pos);
			sf::Color color {m_hit ? sf::Color::Red : sf::Color::White};
			m_shape.setFillColor(color);
		}
};



#endif