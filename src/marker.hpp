#ifndef MARKER_HPP
#define MARKER_HPP

#include "SFML/Graphics.hpp"
#include "common.hpp"


class Marker : public sf::Drawable
{
	public:
		enum class Type {HIT, MISS, GUESS};

	private:
		sf::CircleShape m_shape;
		sf::Vector2i m_index;
		sf::Vector2f m_pos;
		Type m_type;

	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			target.draw(m_shape, states);
		}
		void updateColor()
		{
			if(m_type == Type::HIT)
				m_shape.setFillColor(sf::Color::Red);
			if(m_type == Type::MISS)
				m_shape.setFillColor(sf::Color::White);
			if(m_type == Type::GUESS)
				m_shape.setFillColor(sf::Color{255, 255, 255, 40});
		}

	public:
		Marker(sf::Vector2i index, Type type) :
			m_shape{g_cellsize * 0.3f}, m_index{index}, m_type{type}
		{
			m_shape.setPointCount(16);
			m_shape.setOrigin(m_shape.getRadius(), m_shape.getRadius());
			m_pos = itoc(index);
			m_shape.setPosition(m_pos);
			updateColor();
		}

		void setType(Type type)
		{
			m_type = type;
			updateColor();
		}

		bool isHit() const { return m_type == Type::HIT; }
		bool isGuess() const { return m_type == Type::GUESS; }
};



#endif