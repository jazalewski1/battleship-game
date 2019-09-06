#include "SFML/Graphics.hpp"
#include "common.hpp"
#include "sfline.hpp"
#include "sfutils.hpp"

#include "button.hpp"
#include "grid.hpp"
#include "players.hpp"

#include <iostream>



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
		enum class Mode {PLACE, ATTACK, NONE};

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

		Button m_confirmButton;

		Gui::Text m_humanPointsText;
		Gui::Text m_opponentPointsText;
		Gui::Text m_messageText;

	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			target.draw(m_attackGrid, states);
			target.draw(m_defenseGrid, states);
			if(m_mode == Mode::PLACE)
			{
				target.draw(m_placeGrid, states);
			}
			if(m_mode == Mode::ATTACK)
			{
				target.draw(m_humanPointsText);
				target.draw(m_opponentPointsText);
			}
			target.draw(m_messageText);

			target.draw(m_confirmButton);

			target.draw(m_human);
			target.draw(m_opponent);
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
			if(m_attackGrid.contains(mouse) && m_human.shootable(mouse))
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

				m_humanPointsText.setString("Human: ", m_human.getPoints(), "/", 17);

				m_selectCell->defaultColor();

				m_opponent.startThinking();

				if(m_selectCell)
					m_selectCell->defaultColor();

				m_selectCell = nullptr;

				m_messageText.append((isHit ? " It's a hit!" : "It's a miss..."));
				m_turn = Turn::OPPONENT;
			}
		}
		void opponentShoot()
		{
			sf::Vector2i shot {m_opponent.makeShot()};
			bool isHit {m_human.isShip(shot)};
			m_opponent.markShot(shot, isHit);

			m_messageText.append((isHit ? " It's a hit!" : "It's a miss."));
			m_opponentPointsText.setString("Computer: ", m_opponent.getPoints(), "/", 17);

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
			m_confirmButton{14, 10, 6, 3},
			m_humanPointsText{itof(sf::Vector2i{14, 5})},
			m_opponentPointsText{itof(sf::Vector2i{14, 6})},
			m_messageText{itof(sf::Vector2i{2, 1})}
		{
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

		void press(sf::Vector2f mouse)
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


	Game::Simulation sim;

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
					sim.press(mouse);
				}
			}

			if(event.type == sf::Event::KeyPressed)
			{
				if(event.key.code == sf::Keyboard::Space)
				{
					sim.spacebar();
				}
				if(event.key.code == sf::Keyboard::Escape)
				{
					window.close();
				}

			}
		}

		sim.hover(mouse);

		sim.update();

		window.clear(background);
		window.draw(sim);
		window.display();
	}

	return 0;
}
