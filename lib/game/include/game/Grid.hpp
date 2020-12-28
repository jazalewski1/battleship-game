#ifndef GRID_HPP
#define GRID_HPP


#include "SFML/Graphics.hpp"
#include "common/Common.hpp"
#include "game/Cell.hpp"
#include "util/Sfutils.hpp"
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
		virtual bool contains(sf::Vector2f pos) const { return contains(screen_position_to_index(pos)); }

		sf::IntRect getBounds() const { return m_bounds; }
		Cell* getCell(sf::Vector2i index)
		{
			auto search {m_cells.find(index)};
			if(search != m_cells.end())
				return &search->second;
			return nullptr;
		}
		Cell* getCell(sf::Vector2f pos) { return getCell(screen_position_to_index(pos)); }
};

class GridLabeled : public Grid
{
	private:
		std::unordered_map<sf::Vector2i, LabelCell> m_labels;

	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override
		{
			Grid::draw(target, states);
			for(const auto& cell : m_labels)
				target.draw(cell.second, states);
		}

	public:
		GridLabeled(sf::Vector2i offset, sf::Vector2i size) :
			Grid{offset, size}
		{
			for(int x = 0; x < size.x; ++x)
			{
				sf::Vector2i index {offset.x + x, offset.y - 1};
				char symbol {static_cast<char>(48 + x)};
				m_labels.emplace(index, LabelCell{index, symbol});
			}
			for(int y = 0; y < size.y; ++y)
			{
				sf::Vector2i index {offset.x - 1, offset.y + y};
				char symbol {static_cast<char>(65 + y)};
				m_labels.emplace(index, LabelCell{index, symbol});
			}
		}
		GridLabeled(int offsetX, int offsetY, int sizeX, int sizeY) :
			GridLabeled{sf::Vector2i{offsetX, offsetY}, sf::Vector2i{sizeX, sizeY}}
		{
		}

		std::pair<char, char> getSymbols(sf::Vector2i index) const
		{
			auto searchX {m_labels.find(sf::Vector2i{index.x, m_offset.y - 1})};
			auto searchY {m_labels.find(sf::Vector2i{m_offset.x - 1, index.y})};
			if(searchX != m_labels.end() && searchY != m_labels.end())
				return std::make_pair(searchX->second.getSymbol(), searchY->second.getSymbol());
			else
				return std::make_pair('?', '?');
		}

};

}




#endif