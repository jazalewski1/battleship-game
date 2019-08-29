#include "SFML/Graphics.hpp"
#include "common.hpp"
#include "grid.hpp"
#include "sfline.hpp"
#include "sfutils.hpp"


const sf::Vector2u g_winsize {800, 900};
const float g_boardsize {380.0f};
const int g_cellcount {10};
const float g_cellsize {g_boardsize / static_cast<float>(g_cellcount)};



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
			m_attackGrid{sf::Vector2i{g_cellcount, g_cellcount}, sf::Vector2f{40.0f, 40.0f}},
			m_defenseGrid{sf::Vector2i{g_cellcount, g_cellcount}, sf::Vector2f{40.0f, 480.0f}},
			m_placeGrid{sf::Vector2i{5, 5}, sf::Vector2f{480.0f, 480.0f}},
			m_turn{Turn::NONE}, m_mode{Mode::PLACE}
		{
		}
};

}



int main()
{
	sf::RenderWindow window {sf::VideoMode{g_winsize.x, g_winsize.y}, "Battleship Game"};
	window.setFramerateLimit(60);


	Game::Simulation test;


	while(window.isOpen())
	{
		sf::Event event;
		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(test);
		window.display();
	}

	return 0;
}
