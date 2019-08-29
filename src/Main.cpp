#include "SFML/Graphics.hpp"
#include "common.hpp"
#include "grid.hpp"
#include "sfline.hpp"
#include "sfutils.hpp"
#include <iostream>


const sf::Vector2i g_cellcount {22, 26};
const float g_cellsize {32};
const sf::Vector2u g_winsize {g_cellcount.x * g_cellsize, g_cellcount.y * g_cellsize};
const int g_boardcount {10};

sf::Vector2i ftoi(sf::Vector2f pos) { return sf::Vector2i{pos.x / g_cellsize, pos.y / g_cellsize}; }


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


	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			target.draw(m_attackGrid, states);
			target.draw(m_defenseGrid, states);
			target.draw(m_placeGrid, states);
		}


	public:
		Simulation() :
			m_attackGrid{2, 2, g_boardcount, g_boardcount},
			m_defenseGrid{2, 14, g_boardcount, g_boardcount},
			m_placeGrid{14, 14, 5, 5},
			m_turn{Turn::NONE}, m_mode{Mode::PLACE}
		{
		}


		void hover(sf::Vector2f mouse)
		{
			if(m_mode == Mode::ATTACK)
				m_attackGrid.hover(mouse);
		}

		void press(sf::Vector2f mouse)
		{

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
		sf::Vector2f mouse {sf::Mouse::getPosition(window).x * 1.0f, sf::Mouse::getPosition(window).y * 1.0f};


		sf::Event event;
		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
				window.close();

			if(event.type == sf::Event::MouseButtonPressed)
			{
				if(event.mouseButton.button == sf::Mouse::Left)
				{
					sim.press(mouse);
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
