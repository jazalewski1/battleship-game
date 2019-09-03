#include "SFML/Graphics.hpp"
#include "common.hpp"
#include "grid.hpp"
#include "players.hpp"
#include "sfline.hpp"
#include "sfutils.hpp"
#include <iostream>


const sf::Vector2i g_cellcount {22, 26};
const float g_cellsize {32};
const sf::Vector2u g_winsize {static_cast<unsigned int>(g_cellcount.x * g_cellsize), static_cast<unsigned int>(g_cellcount.y * g_cellsize)};
const int g_boardcount {10};


sf::Vector2i ftoi(sf::Vector2f pos) { return sf::Vector2i{static_cast<int>(pos.x / g_cellsize), static_cast<int>(pos.y / g_cellsize)}; }
sf::Vector2f itof(sf::Vector2i index) { return sf::Vector2f{index.x * g_cellsize, index.y * g_cellsize}; }


namespace Game
{

class Simulation : public sf::Drawable
{
	private:
		enum class Turn {HUMAN, COMPUTER, NONE};
		enum class Mode {PLACE, ATTACK, WAIT, NONE};

	private:
		Game::Grid m_attackGrid;
		Game::Grid m_defenseGrid;
		Game::Grid m_placeGrid;

		Turn m_turn;
		Mode m_mode;

		HumanPlayer m_human;
		ComputerPlayer m_opponent;

		Ship* m_activeShip;


	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			target.draw(m_attackGrid, states);
			target.draw(m_defenseGrid, states);
			target.draw(m_placeGrid, states);
			target.draw(m_human);
			target.draw(m_opponent);
		}


	public:
		Simulation() :
			m_attackGrid{2, 2, g_boardcount, g_boardcount},
			m_defenseGrid{2, 14, g_boardcount, g_boardcount},
			m_placeGrid{14, 14, 5, 5},
			m_turn{Turn::NONE}, m_mode{Mode::PLACE},
			m_human{&m_attackGrid, &m_defenseGrid, &m_placeGrid},
			m_opponent{&m_defenseGrid, &m_attackGrid, &m_placeGrid},
			m_activeShip{nullptr}
		{
		}


		void hover(sf::Vector2f mouse)
		{
			if(m_mode == Mode::PLACE)
			{
				if(m_activeShip)
					m_activeShip->setCenter(mouse);
			}
			if(m_mode == Mode::ATTACK)
			{
				m_attackGrid.hover(mouse);
			}
		}

		void press(sf::Vector2f mouse)
		{
			if(m_mode == Mode::PLACE)
			{
				if(m_activeShip)
				{
					if(m_human.placeable(m_activeShip))
					{
						m_activeShip->adjust(mouse);
						m_activeShip = nullptr;
					}
				}
				else
				{
					sf::Vector2i index {ftoi(mouse)};
					m_activeShip = m_human.getShip(index);
				}
			}
		}

		void spacebar()
		{
			if(m_activeShip)
			{
				m_activeShip->rotate();
			}
		}
};

}



int main()
{
	sf::RenderWindow window {sf::VideoMode{g_winsize.x, g_winsize.y}, "Battleship Game"};
	window.setFramerateLimit(60);


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

		window.clear();
		window.draw(sim);
		window.display();
	}

	return 0;
}
