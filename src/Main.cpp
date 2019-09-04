#include "SFML/Graphics.hpp"
#include "common.hpp"
#include "sfline.hpp"
#include "sfutils.hpp"

#include "button.hpp"
#include "grid.hpp"
#include "players.hpp"

#include <iostream>



const sf::Vector2i g_cellcount {22, 26};
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

		Turn m_turn;
		Mode m_mode;

		HumanPlayer m_human;
		ComputerPlayer m_opponent;

		Ship* m_selectShip;
		Cell* m_hoverCell;
		Cell* m_selectCell;

		Button m_confirmButton;


	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			target.draw(m_attackGrid, states);
			target.draw(m_defenseGrid, states);
			target.draw(m_placeGrid, states);

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
				m_selectCell->defaultColor();
			if(m_attackGrid.contains(mouse) && m_human.shootable(mouse))
			{
				m_selectCell = m_attackGrid.getCell(mouse);
				if(m_selectCell)
					m_selectCell->selectColor();
			}
			else
			{
				m_selectCell = nullptr;
			}
		}
		void humanShoot()
		{
			if(m_selectCell)
			{
				sf::Vector2i index {m_selectCell->getIndex()};
				bool isHit {m_opponent.isShip(index)};
				m_human.markShot(index, isHit);
				m_turn = Turn::OPPONENT;

				m_selectCell->defaultColor();
			}
			m_selectCell = nullptr;
		}
		void opponentShoot()
		{
			sf::Vector2i shot {m_opponent.makeShot()};
			bool isHit {m_human.isShip(shot)};
			m_opponent.markShot(shot, isHit);

			m_turn = Turn::HUMAN;
		}


	public:
		Simulation() :
			m_attackGrid{2, 2, g_boardcount, g_boardcount},
			m_defenseGrid{2, 14, g_boardcount, g_boardcount},
			m_placeGrid{14, 14, 5, 10},
			m_turn{Turn::NONE}, m_mode{Mode::PLACE},
			m_human{&m_attackGrid, &m_defenseGrid, &m_placeGrid},
			m_opponent{&m_defenseGrid, &m_attackGrid, &m_placeGrid},
			m_selectShip{nullptr},
			m_hoverCell{nullptr}, m_selectCell{nullptr},
			m_confirmButton{14, 6, 6, 3}
		{
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

					// here may be the function to think, delaying the shot

					opponentShoot();
				}
			}
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
				selectCell(mouse);

				if(m_confirmButton.pressed(mouse))
				{
					humanShoot();
				}
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
	sf::RenderWindow window {sf::VideoMode{g_winsize.x, g_winsize.y}, "Battleship Game"};
	window.setFramerateLimit(60);

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

		window.clear();
		window.draw(sim);
		window.display();
	}

	return 0;
}
