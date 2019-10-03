#include "SFML/Graphics.hpp"
#include "common.hpp"
#include "sfline.hpp"
#include "sfutils.hpp"

#include "button.hpp"
#include "endscreen.hpp"
#include "grid.hpp"
#include "players.hpp"
#include "particles.hpp"

#include <iostream>
#include <memory>



const sf::Vector2i g_cellcount {22, 28};
const float g_cellsize {32};
const sf::Vector2u g_winsize {static_cast<unsigned int>(g_cellcount.x * g_cellsize), static_cast<unsigned int>(g_cellcount.y * g_cellsize)};
const int g_boardcount {10};
sf::Font g_font;


sf::Vector2i ftoi(sf::Vector2f pos) { return sf::Vector2i{static_cast<int>(pos.x / g_cellsize), static_cast<int>(pos.y / g_cellsize)}; }
sf::Vector2f itof(sf::Vector2i index) { return sf::Vector2f{index.x * g_cellsize, index.y * g_cellsize}; }
sf::Vector2f itoc(sf::Vector2i index) { return sf::Vector2f{(index.x * g_cellsize) + (g_cellsize * 0.5f), (index.y * g_cellsize) + (g_cellsize * 0.5f)}; }


namespace Game
{

class Simulation : public sf::Drawable
{
	private:
		enum class Turn {HUMAN, OPPONENT, NONE};
		enum class Mode {PLACE, ATTACK, FINISH, NONE};

	private:
		GridLabeled m_attackGrid;
		GridLabeled m_defenseGrid;
		Grid m_placeGrid;

		HumanPlayer m_human;
		ComputerPlayer m_opponent;

		Turn m_turn;
		Mode m_mode;

		Ship* m_selectShip;
		Cell* m_hoverCell;
		Cell* m_selectCell;

		ParticleSystem::Emitter m_hitEmitter;
		ParticleSystem::Emitter m_missEmitter;

		Button m_confirmButton;

		Gui::Text m_humanPointsText;
		Gui::Text m_opponentPointsText;
		Gui::Text m_messageText;
		EndScreen m_endscreen;
		bool m_doReset;

	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			target.draw(m_attackGrid, states);
			target.draw(m_defenseGrid, states);
			if(m_mode == Mode::PLACE)
			{
				target.draw(m_placeGrid, states);
			}
			if(m_mode == Mode::ATTACK || m_mode == Mode::FINISH)
			{
				target.draw(m_humanPointsText, states);
				target.draw(m_opponentPointsText, states);
			}
			target.draw(m_messageText, states);

			target.draw(m_confirmButton, states);

			target.draw(m_human, states);
			target.draw(m_opponent, states);

			target.draw(m_hitEmitter, states);
			target.draw(m_missEmitter, states);

			if(m_mode == Mode::FINISH)
			{
				target.draw(m_endscreen, states);
			}
		}

		void selectShip(sf::Vector2f mouse)
		{
			if(m_selectShip)
			{
				if(m_human.placeable(m_selectShip))
				{
					m_selectShip->adjust(mouse);
					m_selectShip = nullptr;
				}
			}
			else
			{
				m_selectShip = m_human.getShip(ftoi(mouse));
			}
		}
		void hoverCell(sf::Vector2f mouse)
		{
			if(m_attackGrid.contains(mouse))
			{
				if(m_hoverCell && m_hoverCell != m_selectCell)
					m_hoverCell->defaultColor();

				m_hoverCell = m_attackGrid.getCell(mouse);
				if(m_hoverCell && m_hoverCell != m_selectCell)
					m_hoverCell->hoverColor();
			}
			else
			{
				if(m_hoverCell && m_hoverCell != m_selectCell)
					m_hoverCell->defaultColor();
				m_hoverCell = nullptr;
			}
		}
		void selectCell(sf::Vector2f mouse)
		{
			if(m_selectCell)
			{
				m_selectCell->defaultColor();
				m_selectCell = nullptr;
			}
			if(m_human.shootable(mouse))
			{
				m_selectCell = m_attackGrid.getCell(mouse);
				if(m_selectCell)
					m_selectCell->selectColor();
			}
		}
		void humanShoot()
		{
			if(m_selectCell && m_confirmButton.isActive())
			{
				sf::Vector2i index {m_selectCell->getIndex()};
				bool isHit {m_opponent.isShip(index)};
				m_human.markShot(index, isHit);

				m_selectCell->defaultColor();
				m_selectCell = nullptr;

				m_humanPointsText.setString("  Human: ", m_human.getPoints(), "/", 17);
				m_messageText.append((isHit ? " It's a hit!" : " It's a miss..."));

				if(isHit)
				{
					m_hitEmitter.setPosition(itoc(index));
					m_hitEmitter.emit();
				}
				else
				{
					m_missEmitter.setPosition(itoc(index));
					m_missEmitter.emit();
				}

				if(m_human.getPoints() >= 17)
					m_mode = Mode::FINISH;

				m_opponent.startThinking();
				m_turn = Turn::OPPONENT;
			}
		}
		void opponentShoot()
		{
			sf::Vector2i index {m_opponent.makeShot()};
			bool isHit {m_human.isShip(index)};
			m_opponent.markShot(index, isHit);

			m_messageText.append((isHit ? " It's a hit!" : " It's a miss."));
			m_opponentPointsText.setString("Computer: ", m_opponent.getPoints(), "/", 17);

			if(isHit)
			{
				m_hitEmitter.setPosition(itoc(index));
				m_hitEmitter.emit();
			}
			else
			{
				m_missEmitter.setPosition(itoc(index));
				m_missEmitter.emit();
			}

			if(m_opponent.getPoints() >= 17)
				m_mode = Mode::FINISH;

			m_turn = Turn::HUMAN;
		}
		void chooseMessage()
		{
			if(m_mode == Mode::PLACE)
			{
				m_messageText.setString("Place ships on the board.");
			}
			if(m_mode == Mode::ATTACK)
			{
				if(m_turn == Turn::HUMAN)
				{
					if(m_selectCell)
					{
						std::pair<char, char> label {m_attackGrid.getSymbols(m_selectCell->getIndex())};
						m_messageText.setString(" You selected: ", label.first, label.second);
					}
					else
					{
						m_messageText.setString("Choose a spot to shoot!");
					}
				}
				if(m_turn == Turn::OPPONENT)
				{
					m_messageText.setString("Opponent's turn.");
				}
			}
			if(m_mode == Mode::FINISH)
			{
				if(m_human.getPoints() > m_opponent.getPoints())
				{
					m_messageText.setString("Finish! You win!");
					m_endscreen.setScreen(true);
				}
				else
				{
					m_messageText.setString("Finish! You lose...");
					m_endscreen.setScreen(false);
				}
			}
		}


	public:
		Simulation() :
			m_attackGrid{2, 4, g_boardcount, g_boardcount},
			m_defenseGrid{2, 16, g_boardcount, g_boardcount},
			m_placeGrid{14, 16, 6, 10},
			m_human{&m_attackGrid, &m_defenseGrid, &m_placeGrid},
			m_opponent{&m_defenseGrid, &m_attackGrid, &m_placeGrid},
			m_turn{Turn::NONE}, m_mode{Mode::PLACE},
			m_selectShip{nullptr},
			m_hoverCell{nullptr}, m_selectCell{nullptr},
			m_hitEmitter{0.0f, 0.0f, 360.0f}, m_missEmitter{0.0f, 0.0f, 360.0f},
			m_confirmButton{14, 10, 6, 3},
			m_humanPointsText{itof(sf::Vector2i{14, 5})},
			m_opponentPointsText{itof(sf::Vector2i{14, 6})},
			m_messageText{itof(sf::Vector2i{2, 1})},
			m_endscreen{}, m_doReset{false}
		{
			m_hitEmitter.setEmitSpan(360.0f);
			m_hitEmitter.setEmitCount(100);
			m_hitEmitter.setSpeed(0.4f, 1.0f);
			m_hitEmitter.setRange(10.0f, 30.0f);
			m_hitEmitter.setStartRadius(0.0f, 0.5f);
			m_hitEmitter.setEndRadius(1.0f, 10.0f);
			m_hitEmitter.addColor(sf::Color::White, 0.0f);
			m_hitEmitter.addColor(sf::Color::Yellow, 0.2f);
			m_hitEmitter.addColor(sf::Color::Red, 0.4f);
			m_hitEmitter.addColor(sf::Color{20, 20, 20}, 0.6f);
			m_hitEmitter.addColor(sf::Color{150, 150, 150}, 1.0f);

			m_missEmitter.setEmitSpan(360.0f);
			m_missEmitter.setEmitCount(100);
			m_missEmitter.setSpeed(0.5f, 1.5f);
			m_missEmitter.setRange(10.0f, 30.0f);
			m_missEmitter.setStartRadius(0.0f, 1.0f);
			m_missEmitter.setEndRadius(1.0f, 10.0f);
			m_missEmitter.setFading(true);
			m_missEmitter.addColor(sf::Color::White, 0.0f);
			m_missEmitter.addColor(sf::Color{200, 200, 209}, 1.0f);



			m_humanPointsText.setFont(g_font);
			m_opponentPointsText.setFont(g_font);
			m_messageText.setFont(g_font);

			m_humanPointsText.setString("Human: ", m_human.getPoints(), "/", 17);
			m_opponentPointsText.setString("Computer: ", m_opponent.getPoints(), "/", 17);
			m_messageText.setString("Place ships on the board.");

			m_humanPointsText.setFillColor(sf::Color::White);
			m_opponentPointsText.setFillColor(sf::Color::White);
			m_messageText.setFillColor(sf::Color::White);
		}


		void update()
		{
			m_hitEmitter.update();
			m_missEmitter.update();

			if(m_mode == Mode::PLACE)
			{
				m_confirmButton.setActive(m_human.isReady() && !m_selectShip);
			}
			if(m_mode == Mode::ATTACK)
			{
				if(m_turn == Turn::HUMAN)
				{
					m_confirmButton.setActive(m_selectCell != nullptr);
				}
				if(m_turn == Turn::OPPONENT)
				{
					m_confirmButton.setActive(false);

					m_opponent.think();
					if(!m_opponent.isThinking())
						opponentShoot();
				}
			}
			if(m_mode == Mode::FINISH)
			{
				m_confirmButton.setActive(false);

				if(m_hoverCell)
					m_hoverCell->defaultColor();
				m_hoverCell = nullptr;
				if(m_selectCell)
					m_selectCell->defaultColor();
				m_selectCell = nullptr;
			}
			chooseMessage();
		}

		void hover(sf::Vector2f mouse)
		{
			if(m_mode == Mode::PLACE)
			{
				if(m_selectShip)
					m_selectShip->setCenter(mouse);
			}

			if(m_mode == Mode::ATTACK)
			{
				hoverCell(mouse);
			}
		}

		void lmbPress(sf::Vector2f mouse)
		{
			if(m_mode == Mode::PLACE)
			{
				selectShip(mouse);

				if(m_confirmButton.pressed(mouse))
				{
					m_mode = Mode::ATTACK;
					m_turn = Turn::HUMAN;
					return;
				}
			}

			if(m_mode == Mode::ATTACK)
			{
				if(m_turn == Turn::HUMAN)
				{
					if(m_confirmButton.pressed(mouse))
					{
						humanShoot();
					}
				}
				selectCell(mouse);
			}
			if(m_mode == Mode::FINISH)
			{
				if(m_endscreen.buttonPressed(mouse))
					m_doReset = true;
			}
		}
		void rmbPress(sf::Vector2f mouse)
		{
			if(m_mode == Mode::ATTACK)
			{
				m_human.markGuess(ftoi(mouse));
			}
		}

		void spacebar()
		{
			if(m_mode == Mode::PLACE)
			{
				if(m_selectShip)
					m_selectShip->rotate();
			}
			if(m_mode == Mode::ATTACK)
			{
				humanShoot();
			}
		}

		bool doReset() const { return m_doReset; }
};

}



int main()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window {sf::VideoMode{g_winsize.x, g_winsize.y}, "Battleship Game", sf::Style::Default, settings};
	window.setFramerateLimit(60);
	sf::Color background {5, 20, 43};

	if(!g_font.loadFromFile("content/consola.ttf"))
		return EXIT_FAILURE;


	std::unique_ptr<Game::Simulation> simulation {std::make_unique<Game::Simulation>()};

	while(window.isOpen())
	{
		sf::Vector2i mousei {sf::Mouse::getPosition(window)};
		sf::Vector2f mouse {mousei.x * 1.0f, mousei.y * 1.0f};


		sf::Event event;
		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				window.close();
			}

			if(event.type == sf::Event::MouseButtonPressed)
			{
				if(event.mouseButton.button == sf::Mouse::Left)
				{
					simulation->lmbPress(mouse);

					if(simulation->doReset())
						simulation = std::make_unique<Game::Simulation>();
				}
				if(event.mouseButton.button == sf::Mouse::Right)
				{
					simulation->rmbPress(mouse);
				}
			}

			if(event.type == sf::Event::KeyPressed)
			{
				if(event.key.code == sf::Keyboard::Space)
				{
					simulation->spacebar();
				}
				if(event.key.code == sf::Keyboard::Escape)
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
