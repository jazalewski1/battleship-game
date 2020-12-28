#ifndef SFLINE_HPP
#define SFLINE_HPP

#include "SFML/Graphics.hpp"
#include "util/Sfutils.hpp"
#include <cmath>

const float LinePi {3.14159265359};


namespace sf
{
	class Line : public sf::Drawable, public sf::Transformable
	{
		private:
			sf::RectangleShape m_rect;
			sf::Vector2f m_point[2];

		private:
			void draw(sf::RenderTarget& target, sf::RenderStates states) const
			{
				target.draw(m_rect, states);
			}

			void calculateRotation()
			{
				float length (dist(m_point[0], m_point[1]));
				float angle (atan2(m_point[1].y - m_point[0].y, m_point[1].x - m_point[0].x) * 180/LinePi);
				m_rect.setSize({length, m_rect.getSize().y});
				m_rect.setPosition(m_point[0]);
				m_rect.setRotation(angle);
			}

		public:
			Line(float x0, float y0, float x1, float y1)
			{
				m_point[0] = sf::Vector2f(x0, y0);
				m_point[1] = sf::Vector2f(x1, y1);

				m_rect.setSize({0, 1});
				m_rect.setOrigin(0, 0.5);

				calculateRotation();

				setThickness(1);
				setFillColor(sf::Color::Black);
			}
			Line(sf::Vector2f p0, sf::Vector2f p1) :
				Line(p0.x, p0.y, p1.x, p1.y)
			{
			}
			Line() :
				Line(0, 0, 0, 0)
			{
			}

			void setFillColor(sf::Color color) { m_rect.setFillColor(color); }
			void setThickness(float thickness)
			{
				m_rect.setSize({m_rect.getSize().x, thickness});
				m_rect.setOrigin(0, thickness/2);
			}
			void setPosition(size_t index, sf::Vector2f pos)
			{
				m_point[index] = pos;
				calculateRotation();
			}
			void setPosition(size_t index, float x, float y)
			{
				setPosition(index, {x, y});
			}
	};
}

#endif