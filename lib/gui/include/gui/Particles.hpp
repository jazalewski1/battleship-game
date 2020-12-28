#ifndef PARTICLES_HPP
#define PARTICLES_HPP

#include "common/Common.hpp"
#include "util/Random.hpp"
#include "util/Sfutils.hpp"
#include "SFML/Graphics.hpp"
#include <cmath>
#include <iostream>
#include <list>
#include <vector>

namespace ParticleSystem
{

template <typename T>
class Range
{
	private:
		using random_generator = effolkronium::random_static;

	private:
		T m_min;
		T m_max;

	public:
		Range(T min, T max) :
			m_min{min}, m_max{max}
		{
			assert(m_min <= m_max && "Min value must be smaller than max value!");
		}
		Range(T max) :
			m_min{0}, m_max{max}
		{
		}
		Range() :
			m_min{0}, m_max{0}
		{
		}

		T min() const { return m_min; }
		T max() const { return m_max; }
		void min(T value)
		{
			assert(m_min <= m_max && "Min value must be smaller than max value!");
			m_min = value;
		}
		void max(T value)
		{
			assert(m_min <= m_max && "Min value must be smaller than max value!");
			m_max = value;
		}
		T random() const { return random_generator::get(m_min, m_max); }
};


class Particle : public sf::Drawable
{
	private:
		struct FloatColor
		{
			float r;
			float g;
			float b;

			FloatColor(float r_, float g_, float b_) :
				r{r_}, g{g_}, b{b_}
			{
			}
			FloatColor(int r_, int g_, int b_) :
				FloatColor{static_cast<float>(r_), static_cast<float>(g_), static_cast<float>(b_)}
			{
			}
			FloatColor() :
				FloatColor{0.0f, 0.0f, 0.0f}
			{
			}
			FloatColor(sf::Color color) :
				FloatColor{static_cast<float>(color.r), static_cast<float>(color.g), static_cast<float>(color.b)}
			{
			}

			friend FloatColor operator+(const FloatColor& c1, const FloatColor& c2) { return FloatColor{c1.r + c2.r, c1.g + c2.g, c1.b + c2.b}; }
			friend FloatColor operator-(const FloatColor& c1, const FloatColor& c2) { return FloatColor{c1.r - c2.r, c1.g - c2.g, c1.b - c2.b}; }
		};
		struct ColorModule
		{
			sf::Color color;
			int start;
			int length;
			float ratio;
		};

	private:
		sf::CircleShape m_shape;
		sf::Vector2f m_pos;
		float m_heading;
		float m_speed;
		sf::Vector2f m_vel;
		float m_range;

		int m_lifetime;
		int m_lifetimeLimit;

		float m_startRadius;
		float m_endRadius;

		std::vector<ColorModule> m_colorModules;
		std::vector<ColorModule>::iterator m_currModule;
		sf::Color m_color;
		FloatColor m_colorf;

		float m_startOpacity;
		float m_opacity;
		bool m_isFading;

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			target.draw(m_shape, states);
		}

		void setFillColor(sf::Color color)
		{
			m_colorf = color;
			m_color = color;
			m_shape.setFillColor(m_color);
		}
		void setFillColor(FloatColor color)
		{
			m_colorf = color;
			m_color = sf::Color{static_cast<sf::Uint8>(m_colorf.r), static_cast<sf::Uint8>(m_colorf.g), static_cast<sf::Uint8>(m_colorf.b), static_cast<sf::Uint8>(m_opacity)};
			m_shape.setFillColor(m_color);
		}
		void calculateModules()
		{
			if(!m_colorModules.empty())
			{
				for(auto curr = std::next(m_colorModules.begin()); curr != m_colorModules.end(); curr = std::next(curr))
				{
					auto prev {std::prev(curr)};
					curr->start = static_cast<int>(m_lifetimeLimit * curr->ratio);
					prev->length = curr->start - prev->start;

					if(curr == std::prev(m_colorModules.end()))
						curr->length = m_lifetimeLimit - curr->start;
				}
			}
		}

	public:
		Particle(sf::Vector2f pos, float heading) :
			m_shape{0.0f},
			m_pos{pos}, m_heading{heading}, m_speed{1}, m_range{360.0f},
			m_lifetime{0}, m_lifetimeLimit{360},
			m_startRadius{0.0f}, m_endRadius{100.0f},
			m_currModule{nullptr},
			m_color{sf::Color::White}, m_colorf{sf::Color::White},
			m_startOpacity{255.0f}, m_opacity{255.0f}, m_isFading{false}
		{
			m_shape.setFillColor(m_color);
			m_shape.setOrigin(m_shape.getRadius(), m_shape.getRadius());
			m_shape.setPosition(m_pos);

			m_vel.x = cos(radians(m_heading)) * m_speed;
			m_vel.y = sin(radians(m_heading)) * m_speed;
		}
		Particle(float posX, float posY, float heading) :
			Particle{sf::Vector2f{posX, posY}, heading}
		{
		}
		Particle(const Particle& other) :
			m_lifetime{0}, m_currModule{nullptr}
		{
			m_shape = other.m_shape;
			m_pos = other.m_pos;
			m_heading = other.m_heading;
			m_speed = other.m_speed;
			m_vel = other.m_vel;
			m_range = other.m_range;

			m_lifetimeLimit = other.m_lifetimeLimit;

			m_startRadius = other.m_startRadius;
			m_endRadius = other.m_endRadius;

			m_colorModules = std::vector<ColorModule>{};
			for(auto& module : other.m_colorModules)
			{
				addColor(module.color, module.ratio);
			}
			m_colorf = FloatColor{m_color};

			m_startOpacity = other.m_startOpacity;
			m_opacity = other.m_opacity;
			m_isFading = other.m_isFading;
		}

		void update()
		{
			m_pos += m_vel;
			m_shape.setPosition(m_pos);

			float ratio {(m_lifetime * 1.0f) / (m_lifetimeLimit * 1.0f)};
			float newRadius {m_startRadius + ((m_endRadius - m_startRadius) * ratio)};
			m_shape.setRadius(newRadius);
			m_shape.setOrigin(m_shape.getRadius(), m_shape.getRadius());

			if(m_colorModules.size() > 1)
			{
				if(m_lifetime >= std::next(m_currModule)->start)
				{
					m_currModule = std::next(m_currModule);
				}

				FloatColor currf {m_currModule->color};
				FloatColor nextf {std::next(m_currModule)->color};
				FloatColor difff {nextf - currf};
				difff.r /= m_currModule->length;
				difff.g /= m_currModule->length;
				difff.b /= m_currModule->length;

				FloatColor newColor {m_colorf + difff};
				setFillColor(newColor);
			}
			if(m_isFading)
			{
				m_opacity = m_startOpacity - (m_startOpacity * ratio);
			}

			++m_lifetime;
		}

		void addColor(sf::Color color, float timeRatio)
		{
			int start {static_cast<int>(m_lifetimeLimit * timeRatio)};

			ColorModule module;
			module.color = color;
			module.start = start;
			module.length = m_lifetimeLimit - start;
			module.ratio = timeRatio;

			if(!m_colorModules.empty())
				m_colorModules.back().length = start - m_colorModules.back().start;

			m_colorModules.push_back(module);
			m_currModule = m_colorModules.begin();
			setFillColor(m_colorModules.begin()->color);
		}

		bool isDead() const { return m_lifetime >= m_lifetimeLimit; }


		void setSpeed(float speed)
		{
			m_speed = speed;
			m_vel.x = cos(radians(m_heading)) * m_speed;
			m_vel.y = sin(radians(m_heading)) * m_speed;
		}
		void setRange(float range)
		{
			m_range = range;
			m_lifetimeLimit = static_cast<int>(m_range / m_speed);
			calculateModules();
		}
		void setLifetimeLimit(int limit)
		{
			m_lifetimeLimit = limit;
			m_range = m_speed * m_lifetimeLimit;
			calculateModules();
		}
		void setStartRadius(float radius) { m_startRadius = radius; }
		void setEndRadius(float radius) { m_endRadius = radius; }
		void setHeading(float angle)
		{
			m_heading = angle;
			m_vel.x = cos(radians(m_heading)) * m_speed;
			m_vel.y = sin(radians(m_heading)) * m_speed;
		}
		void setFading(bool test) { m_isFading = test; }
		void setStartOpacity(float opacity) { m_startOpacity = opacity; }

		void setPosition(sf::Vector2f pos) { m_pos = pos; }
		void setPosition(float posX, float posY) { setPosition(sf::Vector2f{posX, posY}); }
};


class Emitter : public sf::Drawable
{
	private:
		using random_generator = effolkronium::random_static;

	private:
		std::list<Particle> m_particles;
		sf::Vector2f m_pos;
		float m_rotation;
		Range<float> m_span;

		int m_frequency;
		int m_emitCount;
		int m_lifetime;

		Range<int> m_particleLifetimeLimit;
		Range<float> m_particleSpeed;
		Range<float> m_particleRange;
		Range<float> m_particleStartRadius;
		Range<float> m_particleEndRadius;
		Range<float> m_particleStartOpacity;
		Particle m_particleTemplate;

		bool m_rangeIsSet;

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			for(const auto& particle : m_particles)
				target.draw(particle, states);
		}

	public:
		Emitter(sf::Vector2f center, float span) :
			m_pos{center}, m_rotation(0.0f), m_span{span},
			m_frequency{0}, m_emitCount{1}, m_lifetime{0},
			m_particleLifetimeLimit{360},
			m_particleSpeed{1.0f}, m_particleRange{360.0f},
			m_particleStartRadius{0.0f}, m_particleEndRadius{100.0f},
			m_particleStartOpacity{255.0f},
			m_particleTemplate{center, 0.0f},
			m_rangeIsSet{true}
		{
		}
		Emitter(float posX, float posY, float span) :
			Emitter{sf::Vector2f(posX, posY), span}
		{
		}
		Emitter(const Emitter& other) :
			m_lifetime{0}, m_particleTemplate{other.m_particleTemplate}
		{
			m_particles = std::list<Particle>{};
			m_pos = other.m_pos;
			m_rotation = other.m_rotation;
			m_span = other.m_span;

			m_frequency = other.m_frequency;
			m_emitCount = other.m_emitCount;
		}

		void update()
		{
			if(m_frequency > 0)
			{
				if(m_lifetime % m_frequency == 0)
					emit();
			}

			for(auto curr = m_particles.begin(); curr != m_particles.end(); curr = std::next(curr))
			{
				if(curr->isDead())
				{
					auto prev {std::prev(curr)};
					m_particles.erase(curr);
					curr = prev;
				}
				else
				{
					curr->update();
				}
			}
			++m_lifetime;
		}

		void emit()
		{
			m_particleTemplate.setPosition(m_pos);
			for(int i = 0; i < m_emitCount; ++i)
			{
				float startAngle {m_rotation - (m_span.max() * 0.5f)};
				float diff {m_span.random()};
				float angle {startAngle + diff};

				m_particleTemplate.setHeading(angle);
				m_particleTemplate.setSpeed(m_particleSpeed.random());
				if(m_rangeIsSet)
					m_particleTemplate.setRange(m_particleRange.random());
				else
					m_particleTemplate.setLifetimeLimit(m_particleLifetimeLimit.random());
				m_particleTemplate.setStartRadius(m_particleStartRadius.random());
				m_particleTemplate.setEndRadius(m_particleEndRadius.random());
				m_particleTemplate.setEndRadius(m_particleEndRadius.random());
				m_particleTemplate.setStartOpacity(m_particleStartOpacity.random());

				m_particles.push_back(m_particleTemplate);
			}
		}


		void setPosition(sf::Vector2f pos) { m_pos = pos; }
		void setPosition(float posX, float posY) { setPosition(sf::Vector2f{posX, posY}); }
		void setRotation(float angle) { m_rotation = angle; }
		void setEmitFrequency(int frequency) { m_frequency = frequency; }
		void setEmitCount(int count) { m_emitCount = count; }
		void setEmitSpan(float angle) { m_span = Range<float>{angle}; }

		void setTemplate(Particle temp) { m_particleTemplate = temp; }
		void setSpeed(float minSpeed, float maxSpeed) { m_particleSpeed = Range<float>{minSpeed, maxSpeed}; }
		void setLifetimeLimit(int minLimit, int maxLimit) { m_particleLifetimeLimit = Range<int>{minLimit, maxLimit}; m_rangeIsSet = false; }
		void setRange(float minRange, float maxRange) { m_particleRange = Range<float>{minRange, maxRange}; m_rangeIsSet = true; }
		void setStartRadius(float minRadius, float maxRadius) { m_particleStartRadius = Range<float>{minRadius, maxRadius}; }
		void setEndRadius(float minRadius, float maxRadius) { m_particleEndRadius = Range<float>{minRadius, maxRadius}; }
		void setStartOpacity(float minOpacity, float maxOpacity) { m_particleStartOpacity = Range<float>{minOpacity, maxOpacity}; }

		void setFading(bool test) { m_particleTemplate.setFading(test); }
		void addColor(sf::Color color, float timeRatio) { m_particleTemplate.addColor(color, timeRatio); }


		Particle& getTemplate() { return m_particleTemplate; }
};

}


#endif