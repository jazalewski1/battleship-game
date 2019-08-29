#ifndef GRID_HPP
#define GRID_HPP


#include "SFML/Graphics.hpp"
#include "common.hpp"
#include "cell.hpp"
#include "sfutils.hpp"
#include <unordered_map>


namespace Game
{

class Grid : public sf::Drawable
{
	private:
		std::unordered_map<sf::Vector2i, Cell> m_cells;

	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			for(const auto& elem : m_cells)
				target.draw(elem.second, states);
		}

	public:
		Grid(sf::Vector2i offset, sf::Vector2i size)
		{
			for(int y = 0; y < size.y; ++y)
			{
				for(int x = 0; x < size.x; ++x)
				{
					sf::Vector2i newIndex {offset.x + x, offset.y + y};
					m_cells.insert(std::make_pair(newIndex, Cell{newIndex}));
				}
			}
		}
		Grid(int offX, int offY, int sizeX, int sizeY) :
			Grid(sf::Vector2i{offX, offY}, sf::Vector2i{sizeX, sizeY})
		{
		}


		void hover(sf::Vector2f mouse)
		{

		}
};

}




#endif