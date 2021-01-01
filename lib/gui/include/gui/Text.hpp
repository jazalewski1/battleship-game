#ifndef GUITEXT_HPP
#define GUITEXT_HPP

#include "SFML/Graphics.hpp"
#include <sstream>
#include <string>

namespace gui
{
class Text : public sf::Drawable
{
public:
	Text(sf::Vector2f pos, std::string str = "") :
		position{pos}, size{0.0f, 0.0f}, origin_offset{0.0f, 0.0f}, align_offset{0.0f, 0.0f}, align_factor{0.0f, 0.0f}, sum_offset{0.0f, 0.0f}
	{
		text.setCharacterSize(24);
		text.setFillColor(sf::Color::Red);
		text.setString(str);

		adjust();
		text.setPosition(position);
	}

	Text() : Text{{0.0f, 0.0f}, ""}
	{
	}

	void set_string(const std::string& string)
	{
		text.setString(string);
		adjust();
	}

	void set_position(sf::Vector2f pos)
	{
		position = pos;
		text.setPosition(position);
	}

	void set_position(float pos_x, float pos_y)
	{
		set_position(sf::Vector2f{pos_x, pos_y});
	}

	void set_fill_color(sf::Color color)
	{
		text.setFillColor(color);
	}

	void set_outline_color(sf::Color color)
	{
		text.setOutlineColor(color);
	}

	void set_outline_thickness(float value)
	{
		text.setOutlineThickness(value);
	}

	void set_font(const sf::Font& font)
	{
		text.setFont(font);
	}

	void set_character_size(unsigned int size)
	{
		text.setCharacterSize(size);
		adjust();
	}

	void set_line_spacing(float value)
	{
		text.setLineSpacing(value);
		adjust();
	}

	void set_letter_spacing(float value)
	{
		text.setLetterSpacing(value);
		adjust();
	}

	void align_to_center_x()
	{
		sum_offset = sf::Vector2f{0.0f, 0.0f};
		align_factor.x = 0.5f;
		adjust();
	}

	void align_to_center_y()
	{
		sum_offset = sf::Vector2f{0.0f, 0.0f};
		align_factor.y = 0.5f;
		adjust();
	}

	void align_to_center()
	{
		sum_offset = sf::Vector2f{0.0f, 0.0f};
		align_factor = sf::Vector2f{0.5f, 0.5f};
		adjust();
	}

	void align_to_right()
	{
		sum_offset = sf::Vector2f{0.0f, 0.0f};
		align_factor.x = 1.0f;
		adjust();
	}

	void align_to_left()
	{
		sum_offset = sf::Vector2f{0.0f, 0.0f};
		align_factor.x = 0.0f;
		adjust();
	}

	void align_to_top()
	{
		sum_offset = sf::Vector2f{0.0f, 0.0f};
		align_factor.y = 0.0f;
		adjust();
	}

	void align_to_bottom()
	{
		sum_offset = sf::Vector2f{0.0f, 0.0f};
		align_factor.y = 1.0f;
		adjust();
	}

	void setAlignement(sf::Vector2f offset)
	{
		align_factor = sf::Vector2f{0.0f, 0.0f};
		sum_offset = offset;
		adjust();
	}

	sf::Vector2f get_alignement() const
	{
		return align_offset;
	}

	sf::Vector2f get_position() const
	{
		return position;
	}

	sf::Vector2f get_size() const
	{
		return size;
	}

private:
	sf::Text text;
	sf::Vector2f position;
	sf::Vector2f size;
	sf::Vector2f origin_offset;
	sf::Vector2f align_offset;
	sf::Vector2f align_factor;
	sf::Vector2f sum_offset;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(text, states);
	}

	void adjust()
	{
		size = sf::Vector2f{text.getLocalBounds().width, text.getLocalBounds().height};
		align_offset = sf::Vector2f{size.x * align_factor.x, size.y * align_factor.y};
		origin_offset = sf::Vector2f{text.getLocalBounds().left, text.getLocalBounds().top};
		text.setOrigin(sum_offset + align_offset + origin_offset);
	}
};
} // namespace gui

#endif