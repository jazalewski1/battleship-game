#ifndef ENDSCREEN_HPP
#define ENDSCREEN_HPP

#include "SFML/Graphics.hpp"
#include "common/Common.hpp"
#include "gui/Button.hpp"
#include "gui/Guitext.hpp"

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

		text.setFont(common::font);
		text.setPosition(common::window_size.x * 0.5f, 360.0f);
		text.setFillColor(sf::Color::Black);
		text.setCharacterSize(72);
		text.alignToCenterX();

		button.set_string("RESTART");
	}

	void set_human_won()
	{
		text.setString("YOU WIN!");
	}

	void set_opponent_won()
	{
		text.setString("YOU LOSE...");
	}

	bool button_pressed(sf::Vector2f mouse)
	{
		return button.pressed(mouse);
	}

private:
	sf::RectangleShape background;
	Gui::Text text;
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