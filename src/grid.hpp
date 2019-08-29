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

		Cell* m_hoverCell;

	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			for(const auto& elem : m_cells)
				target.draw(elem.second, states);
		}



	public:
		Grid(sf::Vector2i offset, sf::Vector2i size) :
			m_hoverCell{nullptr}
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
			sf::Vector2i index {ftoi(mouse)};
			auto search {m_cells.find(index)};

			if(m_hoverCell != nullptr)
				m_hoverCell->defaultColor();

			if(search != m_cells.end())
				m_hoverCell = &(search->second);
			else
				m_hoverCell = nullptr;

			if(m_hoverCell != nullptr)
				m_hoverCell->hoverColor();
		}
};

}




#endif