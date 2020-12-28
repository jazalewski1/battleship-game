#include "common/Common.hpp"
#include "game/Grid.hpp"
#include "game/Players.hpp"
#include "gui/Button.hpp"
#include "gui/Endscreen.hpp"
#include "gui/Particles.hpp"
#include "gui/Sfline.hpp"
#include "util/Sfutils.hpp"
#include "SFML/Graphics.hpp"
#include <iostream>
#include <memory>

const sf::Vector2i g_cellcount{22, 28};
const float g_cellsize{32};
const sf::Vector2u g_winsize{static_cast<unsigned int>(g_cellcount.x * g_cellsize), static_cast<unsigned int>(g_cellcount.y *g_cellsize)};
const int g_boardcount{10};
sf::Font g_font;

sf::Vector2i screen_position_to_index(sf::Vector2f pos)
{
	return sf::Vector2i{static_cast<int>(pos.x / g_cellsize), static_cast<int>(pos.y / g_cellsize)};
}
sf::Vector2f index_to_screen_position(sf::Vector2i index)
{
	return sf::Vector2f{index.x * g_cellsize, index.y * g_cellsize};
}
sf::Vector2f index_to_center_position(sf::Vector2i index)
{
	return sf::Vector2f{(index.x * g_cellsize) + (g_cellsize * 0.5f), (index.y * g_cellsize) + (g_cellsize * 0.5f)};
}

namespace Game
{
class Simulation : public sf::Drawable
{
public:
	Simulation() : attack_grid{2, 4, g_boardcount, g_boardcount},
					defense_grid{2, 16, g_boardcount, g_boardcount},
					place_grid{14, 16, 6, 10},
					human{&attack_grid, &defense_grid, &place_grid},
					opponent{&defense_grid, &attack_grid, &place_grid},
					turn{Turn::NONE}, mode{Mode::PLACE},
					selected_ship{nullptr},
					cell_on_hover{nullptr}, selected_cell{nullptr},
					hit_emitter{0.0f, 0.0f, 360.0f}, miss_emitter{0.0f, 0.0f, 360.0f},
					confirm_button{14, 10, 6, 3},
					human_points_text{index_to_screen_position(sf::Vector2i{14, 5})},
					opponent_points_text{index_to_screen_position(sf::Vector2i{14, 6})},
					message_text{index_to_screen_position(sf::Vector2i{2, 1})},
					end_screen{}, do_reset{false}
	{
		hit_emitter.setEmitSpan(360.0f);
		hit_emitter.setEmitCount(100);
		hit_emitter.setSpeed(0.4f, 1.0f);
		hit_emitter.setRange(10.0f, 30.0f);
		hit_emitter.setStartRadius(0.0f, 0.5f);
		hit_emitter.setEndRadius(1.0f, 10.0f);
		hit_emitter.addColor(sf::Color::White, 0.0f);
		hit_emitter.addColor(sf::Color::Yellow, 0.2f);
		hit_emitter.addColor(sf::Color::Red, 0.4f);
		hit_emitter.addColor(sf::Color{20, 20, 20}, 0.6f);
		hit_emitter.addColor(sf::Color{150, 150, 150}, 1.0f);

		miss_emitter.setEmitSpan(360.0f);
		miss_emitter.setEmitCount(100);
		miss_emitter.setSpeed(0.5f, 1.5f);
		miss_emitter.setRange(10.0f, 30.0f);
		miss_emitter.setStartRadius(0.0f, 1.0f);
		miss_emitter.setEndRadius(1.0f, 10.0f);
		miss_emitter.setFading(true);
		miss_emitter.addColor(sf::Color::White, 0.0f);
		miss_emitter.addColor(sf::Color{200, 200, 209}, 1.0f);

		human_points_text.setFont(g_font);
		opponent_points_text.setFont(g_font);
		message_text.setFont(g_font);

		human_points_text.setString("Human: ", human.getPoints(), "/", 17);
		opponent_points_text.setString("Computer: ", opponent.getPoints(), "/", 17);
		message_text.setString("Place ships on the board.");

		human_points_text.setFillColor(sf::Color::White);
		opponent_points_text.setFillColor(sf::Color::White);
		message_text.setFillColor(sf::Color::White);
	}

	void update()
	{
		hit_emitter.update();
		miss_emitter.update();

		if (mode == Mode::PLACE)
		{
			confirm_button.setActive(human.isReady() && !selected_ship);
		}
		if (mode == Mode::ATTACK)
		{
			if (turn == Turn::HUMAN)
			{
				confirm_button.setActive(selected_cell != nullptr);
			}
			if (turn == Turn::OPPONENT)
			{
				confirm_button.setActive(false);

				opponent.think();
				if (!opponent.isThinking())
				{
					take_opponent_shot();
				}
			}
		}
		if (mode == Mode::FINISH)
		{
			confirm_button.setActive(false);

			if (cell_on_hover)
			{
				cell_on_hover->defaultColor();
			}
			cell_on_hover = nullptr;
			if (selected_cell)
			{
				selected_cell->defaultColor();
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
				selected_ship->setCenter(mouse);
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
			if (end_screen.buttonPressed(mouse))
			{
				do_reset = true;
			}
		}
	}

	void rmb_press(sf::Vector2f mouse)
	{
		if (mode == Mode::ATTACK)
		{
			human.markGuess(screen_position_to_index(mouse));
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

	bool reset() const { return do_reset; }

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

	Ship *selected_ship;
	Cell *cell_on_hover;
	Cell *selected_cell;

	ParticleSystem::Emitter hit_emitter;
	ParticleSystem::Emitter miss_emitter;

	Button confirm_button;

	Gui::Text human_points_text;
	Gui::Text opponent_points_text;
	Gui::Text message_text;
	EndScreen end_screen;
	bool do_reset;

	void draw(sf::RenderTarget &target, sf::RenderStates states) const
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

		target.draw(hit_emitter, states);
		target.draw(miss_emitter, states);

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
			selected_ship = human.getShip(screen_position_to_index(mouse));
		}
	}

	void hover_on_cell(sf::Vector2f mouse)
	{
		if (attack_grid.contains(mouse))
		{
			if (cell_on_hover && cell_on_hover != selected_cell)
			{
				cell_on_hover->defaultColor();
			}

			cell_on_hover = attack_grid.getCell(mouse);
			if (cell_on_hover && cell_on_hover != selected_cell)
			{
				cell_on_hover->hoverColor();
			}
		}
		else
		{
			if (cell_on_hover && cell_on_hover != selected_cell)
			{
				cell_on_hover->defaultColor();
			}
			cell_on_hover = nullptr;
		}
	}

	void select_cell(sf::Vector2f mouse)
	{
		if (selected_cell)
		{
			selected_cell->defaultColor();
			selected_cell = nullptr;
		}
		if (human.shootable(mouse))
		{
			selected_cell = attack_grid.getCell(mouse);
			if (selected_cell)
				selected_cell->selectColor();
		}
	}

	void take_human_shot()
	{
		if (selected_cell && confirm_button.isActive())
		{
			sf::Vector2i index{selected_cell->getIndex()};
			bool isHit{opponent.isShip(index)};
			human.markShot(index, isHit);

			selected_cell->defaultColor();
			selected_cell = nullptr;

			human_points_text.setString("  Human: ", human.getPoints(), "/", 17);
			message_text.append((isHit ? " It's a hit!" : " It's a miss..."));

			if (isHit)
			{
				hit_emitter.setPosition(index_to_center_position(index));
				hit_emitter.emit();
			}
			else
			{
				miss_emitter.setPosition(index_to_center_position(index));
				miss_emitter.emit();
			}

			if (human.getPoints() >= 17)
			{
				mode = Mode::FINISH;	
			}

			opponent.startThinking();
			turn = Turn::OPPONENT;
		}
	}

	void take_opponent_shot()
	{
		sf::Vector2i index{opponent.makeShot()};
		bool isHit{human.isShip(index)};
		opponent.markShot(index, isHit);

		message_text.append((isHit ? " It's a hit!" : " It's a miss."));
		opponent_points_text.setString("Computer: ", opponent.getPoints(), "/", 17);

		if (isHit)
		{
			hit_emitter.setPosition(index_to_center_position(index));
			hit_emitter.emit();
		}
		else
		{
			miss_emitter.setPosition(index_to_center_position(index));
			miss_emitter.emit();
		}

		if (opponent.getPoints() >= 17)
		{
			mode = Mode::FINISH;
		}

		turn = Turn::HUMAN;
	}

	void choose_message()
	{
		if (mode == Mode::PLACE)
		{
			message_text.setString("Place ships on the board.");
		}
		if (mode == Mode::ATTACK)
		{
			if (turn == Turn::HUMAN)
			{
				if (selected_cell)
				{
					std::pair<char, char> label{attack_grid.getSymbols(selected_cell->getIndex())};
					message_text.setString(" You selected: ", label.first, label.second);
				}
				else
				{
					message_text.setString("Choose a spot to shoot!");
				}
			}
			if (turn == Turn::OPPONENT)
			{
				message_text.setString("Opponent's turn.");
			}
		}
		if (mode == Mode::FINISH)
		{
			if (human.getPoints() > opponent.getPoints())
			{
				message_text.setString("Finish! You win!");
				end_screen.setScreen(true);
			}
			else
			{
				message_text.setString("Finish! You lose...");
				end_screen.setScreen(false);
			}
		}
	}
};
} // namespace Game

int main()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window{sf::VideoMode{g_winsize.x, g_winsize.y}, "Battleship Game", sf::Style::Default, settings};
	window.setFramerateLimit(60);
	sf::Color background{5, 20, 43};

	if (!g_font.loadFromFile("../fonts/consola.ttf"))
	{
		return EXIT_FAILURE;
	}

	std::unique_ptr<Game::Simulation> simulation{std::make_unique<Game::Simulation>()};

	while (window.isOpen())
	{
		sf::Vector2i mousei{sf::Mouse::getPosition(window)};
		sf::Vector2f mouse{mousei.x * 1.0f, mousei.y * 1.0f};

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}

			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					simulation->lmb_press(mouse);

					if (simulation->reset())
					{
						simulation = std::make_unique<Game::Simulation>();
					}
				}
				if (event.mouseButton.button == sf::Mouse::Right)
				{
					simulation->rmb_press(mouse);
				}
			}

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Space)
				{
					simulation->spacebar_press();
				}
				if (event.key.code == sf::Keyboard::Escape)
				{
					window.close();
				}
			}
		}

		simulation->hover(mouse);
		simulation->update();

		window.clear(background);
		window.draw(*simulation);
		window.display();
	}

	return 0;
}
