#ifndef GRID_HPP
#define GRID_HPP


#include "SFML/Graphics.hpp"
#include "sfutils.hpp"
#include "common.hpp"
#include "cell.hpp"
#include <unordered_map>


namespace Game
{

class Grid : public sf::Drawable
{
	protected:
		std::unordered_map<sf::Vector2i, Cell> m_cells;
		sf::Vector2f m_offset;

	protected:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			for(const auto& cell : m_cells)
				target.draw(cell.second, states);
		}

	public:
		Grid(sf::Vector2i count, sf::Vector2f offset) :
			m_offset{offset}
		{
			for(int y = 0; y < count.y; ++y)
			{
				for(int x = 0; x < count.x; ++x)
				{
					sf::Vector2f pos {x * g_cellsize + m_offset.x, y * g_cellsize + m_offset.y};
					sf::Vector2i index {x, y};
					m_cells.insert(std::make_pair(index, Cell{pos}));
				}
			}
		}
};

}




#endif