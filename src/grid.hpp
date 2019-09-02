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
		sf::IntRect m_bounds;

	protected:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			for(const auto& elem : m_cells)
				target.draw(*elem.second, states);
		}


		template <typename CellType>
		void addCell(sf::Vector2i index) { m_cells.insert(std::make_pair(index, std::make_unique<CellType>(index))); }
		template <typename CellType>
		void addCell(int indexX, int indexY) { addCell<CellType>(sf::Vector2i{indexX, indexY}); }

	public:
		Grid(sf::Vector2i offset, sf::Vector2i size) :
			m_offset{offset}, m_size{size}, m_bounds{offset, size}
		{
		}

		virtual bool contains(sf::Vector2i index) const { return m_bounds.contains(index); }
		virtual bool contains(sf::Vector2f pos) const { return contains(ftoi(pos)); }

		sf::IntRect getBounds() const { return m_bounds; }
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
	private:
		void changeKey(sf::Vector2i old, sf::Vector2i update)
		{
			if(m_cells.find(old) != m_cells.end())
			{
				auto node {m_cells.extract(old)};
				node.key() = update;
				m_cells.insert(std::move(node));
			}
		}

	public:
		ShipGrid(sf::Vector2i offset, int length) :
			Grid{offset, sf::Vector2i{length, 1}}
		{
			for(int x = 0; x < length; ++x)
				addCell<ShipCell>(offset.x + x, offset.y);
		}
		ShipGrid(int offsetX, int offsetY, int length) :
			ShipGrid{sf::Vector2i{offsetX, offsetY}, length}
		{
		}

		void setPosition(sf::Vector2f pos)
		{
			for(int x = 0; x < m_size.x; ++x)
			{
				sf::Vector2i index {m_offset.x + x, m_offset.y};
				auto search {m_cells.find(index)};
				if(search != m_cells.end())
				{
					sf::Vector2f center {pos.x + (x * g_cellsize), pos.y};
					search->second->setCenter(center);
				}
			}
		}

		void adjust(sf::Vector2f pos)
		{
			sf::Vector2i newOffset {ftoi(pos)};

			for(int x = 0; x < m_size.x; ++x)
			{
				sf::Vector2i oldIndex {m_offset.x + x, m_offset.y};
				auto search {m_cells.find(oldIndex)};
				if(search != m_cells.end())
				{
					sf::Vector2i newIndex {newOffset.x + x, newOffset.y};
					search->second->setIndex(newIndex);
					changeKey(oldIndex, newIndex);
				}
			}
			m_offset = newOffset;
			m_bounds = sf::IntRect{m_offset, m_size};
		}
};

}




#endif