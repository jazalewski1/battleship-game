#ifndef ENDSCREEN_HPP
#define ENDSCREEN_HPP

#include "SFML/Graphics.hpp"
#include "common/Common.hpp"
#include "gui/Button.hpp"
#include "gui/Text.hpp"

namespace game
{
class EndScreen : public sf::Drawable
{
public:
	EndScreen() :
		background{sf::Vector2f{common::index_to_screen_position(sf::Vector2i{common::grid_size.x, 10})}}, text{},
		button{{7, 14}, {8, 3}, true}
	{
		background.setFillColor(sf::Color{240, 235, 189, 220});
		background.setPosition(common::index_to_screen_position(sf::Vector2i{0, 9}));

		text.set_font(common::font);
		text.set_position(common::window_size.x * 0.5f, 360.0f);
		text.set_fill_color(sf::Color::Black);
		text.set_character_size(72);
		text.align_to_center_x();

		button.set_string("RESTART");
	}

	void set_human_won()
	{
		text.set_string("YOU WIN!");
	}

	void set_opponent_won()
	{
		text.set_string("YOU LOSE...");
	}

	bool button_pressed(sf::Vector2f mouse)
	{
		return button.pressed(mouse);
	}

private:
	sf::RectangleShape background;
	gui::Text text;
	game::Button button;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(background, states);
		target.draw(text, states);
		target.draw(button, states);
	}
};
} // namespace game

#endif