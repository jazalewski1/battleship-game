#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "SFML/Graphics.hpp"
#include "common/Common.hpp"
#include "gui/Guitext.hpp"

namespace game
{
class Button : public sf::Drawable
{
public:
	Button(sf::Vector2i offset, sf::Vector2i size, bool active = false) : // TODO: don't pass bool active; pass string for text
		shape{common::index_to_screen_position(size)}, offset{offset}, size{size}, is_activated{active}, active_color{83, 158, 24},
		active_color_detail{37, 71, 9}, unactive_color{196, 196, 196},
		unactive_color_detail{94, 94, 94}, text{common::index_to_center_position(offset), "CONFIRM"}
	{
		shape.setPosition(common::index_to_screen_position(offset));
		shape.setOutlineThickness(3.0f);
		change_color();

		center = common::index_to_screen_position(offset) + (shape.getSize() * 0.5f);

		text.setFont(common::font);
		text.setCharacterSize(32);
		text.alignToCenter();
		text.setPosition(center);
	}

	bool pressed(sf::Vector2f mouse)
	{
		return is_activated && shape.getGlobalBounds().contains(mouse);
	}

	void activate()
	{
		is_activated = true;
		change_color();
	}

	void deactivate()
	{
		is_activated = false;
		change_color();
	}

	void set_string(const std::string& string)
	{
		text.setString(string);
	}

	bool is_active() const
	{
		return is_activated;
	}

private:
	sf::RectangleShape shape;
	sf::Vector2i offset;
	sf::Vector2i size;
	sf::Vector2f center;
	bool is_activated;
	sf::Color active_color;
	sf::Color active_color_detail;
	sf::Color unactive_color;
	sf::Color unactive_color_detail;
	Gui::Text text;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(shape, states);
		target.draw(text, states);
	}

	void change_color()
	{
		if (is_activated)
		{
			shape.setFillColor(active_color);
			shape.setOutlineColor(active_color_detail);
			text.setFillColor(active_color_detail);
		}
		else
		{
			shape.setFillColor(unactive_color);
			shape.setOutlineColor(unactive_color_detail);
			text.setFillColor(unactive_color_detail);
		}
	}
};
} // namespace game

#endif