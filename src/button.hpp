#ifndef BUTTON_HPP
#define BUTTON_HPP


#include "SFML/Graphics.hpp"
#include "common.hpp"

namespace Game
{

class Button : public sf::Drawable
{
	private:
		sf::RectangleShape m_shape;
		sf::Vector2i m_offset;
		sf::Vector2i m_size;
		sf::Vector2f m_center;

		bool m_isActive;
		sf::Color m_activeColor;
		sf::Color m_unactiveColor;

	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			target.draw(m_shape, states);
		}

		void changeColor()
		{
			if(m_isActive)
				m_shape.setFillColor(m_activeColor);
			else
				m_shape.setFillColor(m_unactiveColor);
		}

	public:
		Button(sf::Vector2i offset, sf::Vector2i size, bool active = false) :
			m_shape{itof(size)}, m_offset{offset}, m_size{size},
			m_isActive{active}, m_activeColor{83, 158, 24}, m_unactiveColor{196, 196, 196}
		{
			m_shape.setPosition(itof(m_offset));
			changeColor();

			m_center.x = m_shape.getLocalBounds().left + (m_shape.getLocalBounds().width * 0.5f);
			m_center.x = m_shape.getLocalBounds().top + (m_shape.getLocalBounds().height * 0.5f);
		}
		Button(int offsetX, int offsetY, int sizeX, int sizeY, bool active = false) :
			Button{sf::Vector2i{offsetX, offsetY}, sf::Vector2i{sizeX, sizeY}, active}
		{
		}


		bool pressed(sf::Vector2f mouse) { return m_isActive && m_shape.getGlobalBounds().contains(mouse); }

		void setActive(bool active)
		{
			m_isActive = active;
			changeColor();
		}
};

}


#endif