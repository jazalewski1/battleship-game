#ifndef GRID_HPP
#define GRID_HPP


#include "SFML/Graphics.hpp"
#include "common.hpp"
#include "cell.hpp"
#include "sfutils.hpp"
#include <memory>
#include <unordered_map>


namespace Game
{

class Grid : public sf::Drawable
{
	protected:
		std::unordered_map<sf::Vector2i, std::unique_ptr<Cell> > m_cells;
		sf::Vector2i m_offset;
		sf::Vector2i m_size;

	protected:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			for(const auto& elem : m_cells)
				target.draw(*elem.second, states);
		}

		bool contains(sf::Vector2i index) { return m_cells.find(index) != m_cells.end(); }

		template <typename CellType>
		void addCell(sf::Vector2i index) { m_cells.insert(std::make_pair(index, std::make_unique<CellType>(index))); }
		template <typename CellType>
		void addCell(int indexX, int indexY) { addCell<CellType>(sf::Vector2i{indexX, indexY}); }

	public:
		Grid(sf::Vector2i offset, sf::Vector2i size) :
			m_offset{offset}, m_size{size}
		{
		}
};


class BoardGrid : public Grid
{
	private:
		BoardCell* m_hoverCell;

	public:
		BoardGrid(sf::Vector2i offset, sf::Vector2i size) :
			Grid{offset, size}, m_hoverCell{nullptr}
		{
			for(int y = 0; y < m_size.y; ++y)
			{
				for(int x = 0; x < m_size.x; ++x)
				{
					addCell<BoardCell>(m_offset.x + x, m_offset.y + y);
				}
			}
		}
		BoardGrid(int offX, int offY, int sizeX, int sizeY) :
			BoardGrid{sf::Vector2i{offX, offY}, sf::Vector2i{sizeX, sizeY}}
		{
		}


		void hover(sf::Vector2f mouse)
		{
			sf::Vector2i index {ftoi(mouse)};
			auto search {m_cells.find(index)};

			if(m_hoverCell != nullptr)
				m_hoverCell->defaultColor();

			if(search != m_cells.end())
				m_hoverCell = dynamic_cast<BoardCell*>(search->second.get());
			else
				m_hoverCell = nullptr;

			if(m_hoverCell != nullptr)
				m_hoverCell->hoverColor();
		}
};


class ShipGrid : public Grid
{

};

}




#endif