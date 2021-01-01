#pragma once

#include "SFML/Graphics.hpp"
#include "common/Common.hpp"
#include "game/ComputerPlayer.hpp"
#include "game/Game.hpp"
#include "game/Grid.hpp"
#include "game/HumanPlayer.hpp"
#include "game/Player.hpp"
#include "gui/Button.hpp"
#include "gui/Endscreen.hpp"
#include "util/Sfutils.hpp"
#include <iostream>
#include <memory>

namespace game
{
class Game : public sf::Drawable
{
public:
	Game() :
		attack_grid{2, 4, common::single_board_size, common::single_board_size},
		defense_grid{2, 16, common::single_board_size, common::single_board_size},
		place_grid{14, 16, 6, 10}, human{attack_grid, defense_grid, place_grid}, opponent{defense_grid, attack_grid, place_grid},
		turn{Turn::NONE}, mode{Mode::PLACE}, selected_ship{nullptr}, cell_on_hover{nullptr}, selected_cell{nullptr}, confirm_button{{14, 10}, {6, 3}},
		human_points_text{common::index_to_screen_position(sf::Vector2i{14, 5})},
		opponent_points_text{common::index_to_screen_position(sf::Vector2i{14, 6})},
		message_text{common::index_to_screen_position(sf::Vector2i{2, 1})}, end_screen{}, do_reset{false}
	{
		human_points_text.set_font(common::font);
		opponent_points_text.set_font(common::font);
		message_text.set_font(common::font);

		const auto human_points_string = std::string{"Human: 0/17"};
		human_points_text.set_string(human_points_string);
		const auto opponent_points_string = std::string{"Opponent: 0/17"};
		opponent_points_text.set_string(opponent_points_string);
		message_text.set_string("Place ships on the board.");

		human_points_text.set_fill_color(sf::Color::White);
		opponent_points_text.set_fill_color(sf::Color::White);
		message_text.set_fill_color(sf::Color::White);
	}

	void update()
	{
		if (mode == Mode::PLACE)
		{
			if(human.is_ready() && !selected_ship)
			{
				confirm_button.activate();
			}
			else
			{
				confirm_button.deactivate();
			}
		}
		if (mode == Mode::ATTACK)
		{
			if (turn == Turn::HUMAN)
			{
				if(selected_cell != nullptr)
				{
					confirm_button.activate();
				}
				else
				{
					confirm_button.deactivate();
				}
			}
			if (turn == Turn::OPPONENT)
			{
				confirm_button.deactivate();

				opponent.think();
				if (!opponent.is_thinking())
				{
					take_opponent_shot();
				}
			}
		}
		if (mode == Mode::FINISH)
		{
			confirm_button.deactivate();

			if (cell_on_hover)
			{
				cell_on_hover->put_default_color();
			}
			cell_on_hover = nullptr;
			if (selected_cell)
			{
				selected_cell->put_default_color();
			}
			selected_cell = nullptr;
		}
		choose_message();
	}

	void hover(sf::Vector2f mouse)
	{
		if (mode == Mode::PLACE)
		{
			if (selected_ship)
			{
				selected_ship->set_center(mouse);
			}
		}

		if (mode == Mode::ATTACK)
		{
			hover_on_cell(mouse);
		}
	}

	void lmb_press(sf::Vector2f mouse)
	{
		if (mode == Mode::PLACE)
		{
			select_ship(mouse);

			if (confirm_button.pressed(mouse))
			{
				mode = Mode::ATTACK;
				turn = Turn::HUMAN;
				return;
			}
		}

		if (mode == Mode::ATTACK)
		{
			if (turn == Turn::HUMAN)
			{
				if (confirm_button.pressed(mouse))
				{
					take_human_shot();
				}
			}
			select_cell(mouse);
		}
		if (mode == Mode::FINISH)
		{
			if (end_screen.button_pressed(mouse))
			{
				do_reset = true;
			}
		}
	}

	void rmb_press(sf::Vector2f mouse)
	{
		if (mode == Mode::ATTACK)
		{
			human.mark_guess(common::screen_position_to_index(mouse));
		}
	}

	void spacebar_press()
	{
		if (mode == Mode::PLACE)
		{
			if (selected_ship)
			{
				selected_ship->rotate();
			}
		}
		if (mode == Mode::ATTACK)
		{
			take_human_shot();
		}
	}

	bool reset() const
	{
		return do_reset;
	}

private:
	enum class Turn
	{
		HUMAN,
		OPPONENT,
		NONE
	};
	enum class Mode
	{
		PLACE,
		ATTACK,
		FINISH,
		NONE
	};

	GridLabeled attack_grid;
	GridLabeled defense_grid;
	Grid place_grid;

	HumanPlayer human;
	ComputerPlayer opponent;

	Turn turn;
	Mode mode;

	Ship* selected_ship;
	Cell* cell_on_hover;
	Cell* selected_cell;

	Button confirm_button;

	gui::Text human_points_text;
	gui::Text opponent_points_text;
	gui::Text message_text;
	EndScreen end_screen;
	bool do_reset;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(attack_grid, states);
		target.draw(defense_grid, states);
		if (mode == Mode::PLACE)
		{
			target.draw(place_grid, states);
		}
		if (mode == Mode::ATTACK || mode == Mode::FINISH)
		{
			target.draw(human_points_text, states);
			target.draw(opponent_points_text, states);
		}
		target.draw(message_text, states);

		target.draw(confirm_button, states);

		target.draw(human, states);
		target.draw(opponent, states);

		if (mode == Mode::FINISH)
		{
			target.draw(end_screen, states);
		}
	}

	void select_ship(sf::Vector2f mouse)
	{
		if (selected_ship)
		{
			if (human.placeable(selected_ship))
			{
				selected_ship->adjust(mouse);
				selected_ship = nullptr;
			}
		}
		else
		{
			selected_ship = human.get_ship(common::screen_position_to_index(mouse));
		}
	}

	void hover_on_cell(sf::Vector2f mouse)
	{
		if (attack_grid.contains(mouse))
		{
			if (cell_on_hover && cell_on_hover != selected_cell)
			{
				cell_on_hover->put_default_color();
			}

			cell_on_hover = attack_grid.get_cell(mouse);
			if (cell_on_hover && cell_on_hover != selected_cell)
			{
				cell_on_hover->put_hover_color();
			}
		}
		else
		{
			if (cell_on_hover && cell_on_hover != selected_cell)
			{
				cell_on_hover->put_default_color();
			}
			cell_on_hover = nullptr;
		}
	}

	void select_cell(sf::Vector2f mouse)
	{
		if (selected_cell)
		{
			selected_cell->put_default_color();
			selected_cell = nullptr;
		}
		if (human.shootable(mouse))
		{
			selected_cell = attack_grid.get_cell(mouse);
			if (selected_cell)
				selected_cell->put_select_color();
		}
	}

	void take_human_shot()
	{
		if (selected_cell && confirm_button.is_active())
		{
			const auto index = sf::Vector2i{selected_cell->get_index()};
			const auto is_hit = opponent.contains_ship(index);
			human.mark_shot(index, is_hit);

			selected_cell->put_default_color();
			selected_cell = nullptr;

			const auto human_points_string = std::string{"Human: " + std::to_string(human.get_points()) + "/" + std::to_string(17)};
			human_points_text.set_string(human_points_string);

			if (human.get_points() >= 17)
			{
				mode = Mode::FINISH;
			}

			opponent.start_thinking();
			turn = Turn::OPPONENT;
		}
	}

	void take_opponent_shot()
	{
		const auto index = sf::Vector2i{opponent.make_shot()};
		const auto is_hit = human.contains_ship(index);
		opponent.mark_shot(index, is_hit);

		const auto opponent_points_string = std::string{"Opponent: " + std::to_string(opponent.get_points()) + "/" + std::to_string(17)};
		opponent_points_text.set_string(opponent_points_string);

		if (opponent.get_points() >= 17)
		{
			mode = Mode::FINISH;
		}

		turn = Turn::HUMAN;
	}

	void choose_message()
	{
		if (mode == Mode::PLACE)
		{
			message_text.set_string("Place ships on the board.");
		}
		if (mode == Mode::ATTACK)
		{
			if (turn == Turn::HUMAN)
			{
				if (selected_cell)
				{
					const auto label = attack_grid.get_symbols(selected_cell->get_index());
					const auto message_string = std::string{" You selected: " + label.first + label.second};
					message_text.set_string(message_string);
				}
				else
				{
					message_text.set_string("Choose a spot to shoot!");
				}
			}
			if (turn == Turn::OPPONENT)
			{
				message_text.set_string("Opponent's turn.");
			}
		}
		if (mode == Mode::FINISH)
		{
			if (human.get_points() > opponent.get_points())
			{
				message_text.set_string("Finish! You win!");
				end_screen.set_human_won();
			}
			else
			{
				message_text.set_string("Finish! You lose...");
				end_screen.set_opponent_won();
			}
		}
	}
};
} // namespace game