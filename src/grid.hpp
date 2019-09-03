#ifndef GRID_HPP
#define GRID_HPP


#include "SFML/Graphics.hpp"
#include "common.hpp"
#include "cell.hpp"
#include "sfutils.hpp"
#include <iostream>
#include <unordered_map>


namespace Game
{

class Grid : public sf::Drawable
{
	protected:
		std::unordered_map<sf::Vector2i, Cell> m_cells;
		sf::Vector2i m_offset;
		sf::Vector2i m_size;
		sf::IntRect m_bounds;

	protected:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			for(const auto& elem : m_cells)
				target.draw(elem.second, states);
		}


	public:
		Grid(sf::Vector2i offset, sf::Vector2i size) :
			m_offset{offset}, m_size{size}, m_bounds{offset, size}
		{
			for(int y = 0; y < m_size.y; ++y)
			{
				for(int x = 0; x < m_size.x; ++x)
				{
					sf::Vector2i index {m_offset.x + x, m_offset.y + y};
					m_cells.emplace(index, index);
				}
			}
		}
		Grid(int offsetX, int offsetY, int sizeX, int sizeY) :
			Grid{sf::Vector2i{offsetX, offsetY}, sf::Vector2i{sizeX, sizeY}}
		{
		}

		virtual bool contains(sf::Vector2i index) const { return m_bounds.contains(index); }
		virtual bool contains(sf::Vector2f pos) const { return contains(ftoi(pos)); }

		sf::IntRect getBounds() const { return m_bounds; }
		Cell* getCell(sf::Vector2i index)
		{
			auto search {m_cells.find(index)};
			if(search != m_cells.end())
				return &search->second;
			return nullptr;
		}
		Cell* getCell(sf::Vector2f pos) { return getCell(ftoi(pos)); }
};

}




#endif