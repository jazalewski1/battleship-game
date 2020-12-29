#ifndef PLAYERS_HPP
#define PLAYERS_HPP

#include "SFML/Graphics.hpp"
#include "common/Common.hpp"
#include "game/Grid.hpp"
#include "game/Marker.hpp"
#include "util/Random.hpp"
#include "game/Ship.hpp"
#include <algorithm>
#include <iostream>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>




namespace game
{
using random_static = effolkronium::random_static;

class Player : public sf::Drawable
{

	protected:
		std::vector<Ship> m_ships;

		std::unordered_map<sf::Vector2i, Marker> m_markers;
		std::unordered_set<sf::Vector2i> m_possibleShots;

		const Grid* m_attackGrid;
		const Grid* m_defenseGrid;
		const Grid* m_placeGrid;
		int m_points;

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
		virtual void fillShips() = 0;

	public:
		Player(const Grid* attackGrid, const Grid* defenseGrid, const Grid* placeGrid) :
			m_attackGrid{attackGrid}, m_defenseGrid{defenseGrid}, m_placeGrid{placeGrid},
			m_points{0}
		{
			sf::IntRect bounds {m_attackGrid->get_bounds()};
			for(int y = 0; y < bounds.height; ++y)
			{
				for(int x = 0; x < bounds.width; ++x)
				{
					m_possibleShots.emplace(x + bounds.left, y + bounds.top);
				}
			}
		}

		bool isShip(sf::Vector2i index) const
		{
			for(const auto& ship : m_ships)
			{
				if(ship.contains(index))
					return true;
			}
			return false;
		}

		bool placeable(const Ship* test) const
		{
			sf::IntRect rect {test->getGhostBounds()};
			for(int y = 0; y < rect.height; ++y)
			{
				for(int x = 0; x < rect.width; ++x)
				{
					sf::Vector2i index {rect.left + x, rect.top + y};
					if(!m_defenseGrid->contains(index))
						return false;
				}
			}
			for(int y = 0; y < rect.height + 2; ++y)
			{
				for(int x = 0; x < rect.width + 2; ++x)
				{
					sf::Vector2i index {rect.left - 1 + x, rect.top - 1 + y};
					if(test == getShip(index))
						continue;

					if(isShip(index))
						return false;
				}
			}
			return true;
		}
		bool shootable(sf::Vector2i index) const { return m_possibleShots.find(index) != m_possibleShots.end(); }
		bool shootable(sf::Vector2f pos) const { return shootable(common::screen_position_to_index(pos)); }

		virtual void markShot(sf::Vector2i index, bool isHit)
		{
			Marker::Type type {(isHit ? Marker::Type::HIT : Marker::Type::MISS)};
			auto search {m_markers.find(index)};
			if(search != m_markers.end())
				search->second.set_type(type);
			else
				m_markers.emplace(index, Marker{index, type});

			if(m_possibleShots.find(index) != m_possibleShots.end())
				m_possibleShots.erase(index);

			if(isHit)
				++m_points;
		}


		Ship* getShip(sf::Vector2i index)
		{
			for(auto& ship : m_ships)
			{
				if(ship.contains(index))
					return &ship;
			}
			return nullptr;
		}
		const Ship* getShip(sf::Vector2i index) const
		{
			for(auto& ship : m_ships)
			{
				if(ship.contains(index))
					return &ship;
			}
			return nullptr;
		}
		int getPoints() const { return m_points; }

};

class ComputerPlayer : public Player
{
	private:
		struct Shot
		{
			sf::Vector2i index;
			bool hit;
			Shot() : index{0, 0}, hit{false} {}
		};
		enum class Dir {U, R, D, L};

	private:
		Shot m_lastShot;
		sf::Vector2i m_origin;
		Dir m_direction;
		sf::Vector2i m_diff;
		bool m_foundOrigin;
		bool m_foundPlane;
		bool m_foundEnd1;
		bool m_foundEnd2;
		sf::IntRect m_foundShip;

		int m_delayLimit;
		int m_delayCounter;

	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override
		{
			// for(const auto& ship : m_ships)
			// 	target.draw(ship, states);
			for(const auto& marker : m_markers)
				target.draw(marker.second, states);
		}


		void fillShips() override
		{
			sf::Vector2i start {m_placeGrid->get_bounds().left, m_placeGrid->get_bounds().top};
			sf::IntRect bounds {m_defenseGrid->get_bounds()};
			for(int i = 1; i <= 5; ++i)
			{
				int shipSize {(i < 3 ? i + 1 : i)};
				m_ships.push_back(Ship{start, shipSize});
				do
				{
					if(random_static::get(0, 100) < 50)
						m_ships.back().rotate();
					sf::Vector2i offset {random_static::get(bounds.left, bounds.left + bounds.width), random_static::get(bounds.top, bounds.top + bounds.height)};
					m_ships.back().setOffset(offset);
				} while(!placeable(&m_ships.back()));
			}
		}

		void updateFoundShip()
		{
			sf::Vector2i index {m_lastShot.index};
			if(!m_foundOrigin)
			{
				m_foundShip = sf::IntRect{index.x, index.y, 1, 1};
			}
			else
			{
				m_foundShip.left = (index.x < m_foundShip.left ? index.x : m_foundShip.left);
				m_foundShip.top = (index.y < m_foundShip.top ? index.y : m_foundShip.top);
				if(m_direction == Dir::R || m_direction == Dir::L)
					++m_foundShip.width;
				if(m_direction == Dir::U || m_direction == Dir::D)
					++m_foundShip.height;
			}
		}
		void updatePossibleShots()
		{
			sf::IntRect rect {m_foundShip};
			for(int y = 0; y < rect.height + 2; ++y)
			{
				for(int x = 0; x < rect.width + 2; ++x)
				{
					sf::Vector2i index {rect.left - 1 + x, rect.top - 1 + y};
					if(m_possibleShots.find(index) != m_possibleShots.end())
						m_possibleShots.erase(index);
				}
			}
		}

		void setDirection(Dir d)
		{
			m_direction = d;
			switch(d)
			{
				case Dir::U: m_diff = sf::Vector2i{0, -1}; break;
				case Dir::R: m_diff = sf::Vector2i{1, 0}; break;
				case Dir::D: m_diff = sf::Vector2i{0, 1}; break;
				case Dir::L: m_diff = sf::Vector2i{-1, 0}; break;
				default: break;
			}
		}
		void rotateDirection()
		{
			switch(m_direction)
			{
				case Dir::U: setDirection(Dir::R); break;
				case Dir::R: setDirection(Dir::D); break;
				case Dir::D: setDirection(Dir::L); break;
				case Dir::L: setDirection(Dir::U); break;
				default: break;
			}
		}
		void reverseDirection()
		{
			switch(m_direction)
			{
				case Dir::U: setDirection(Dir::D); break;
				case Dir::R: setDirection(Dir::L); break;
				case Dir::D: setDirection(Dir::U); break;
				case Dir::L: setDirection(Dir::R); break;
				default: break;
			}
		}
		bool isHit(sf::Vector2i index) const { return m_markers.at(index).is_hit(); }

	public:
		ComputerPlayer(const Grid* attackGrid, const Grid* defenseGrid, const Grid* placeGrid) :
			Player{attackGrid, defenseGrid, placeGrid},
			m_foundOrigin{false}, m_foundPlane{false}, m_foundEnd1{false}, m_foundEnd2{false},
			m_delayLimit{60}, m_delayCounter{0}
		{
			fillShips();
			setDirection(static_cast<Dir>(random_static::get(0, 3)));
		}

		sf::Vector2i makeShot()
		{
			if(!m_foundOrigin && !m_foundPlane)
			{
				if(m_lastShot.hit)
				{
					m_foundOrigin = true;
					m_origin = m_lastShot.index;

					sf::Vector2i index {m_origin + m_diff};
					while(!shootable(index))
					{
						rotateDirection();
						index = m_origin + m_diff;
					}
					return index;
				}
				else
				{
					return *random_static::get(m_possibleShots);
				}
			}

			if(m_foundOrigin && !m_foundPlane)
			{
				if(!m_lastShot.hit)
				{
					rotateDirection();
					sf::Vector2i index {m_origin + m_diff};
					while(!shootable(index))
					{
						rotateDirection();
						index = m_origin + m_diff;
					}
					return index;
				}
				else
				{
					m_foundPlane = true;

					sf::Vector2i index {m_lastShot.index + m_diff};
					bool test {true};
					while(test)
					{
						if(!m_attackGrid->contains(index))
						{
							m_foundEnd1 = true;
							reverseDirection();
							test = false;
						}
						else
						{
							auto markerItr {m_markers.find(index)};
							if(markerItr != m_markers.end())
							{
								if(markerItr->second.is_hit())
								{
									index = index + m_diff;
								}
								else
								{
									m_foundEnd1 = true;
									reverseDirection();
									test = false;
								}
							}
							else
							{
								return index;
							}
						}
					}
				}
			}
			if(m_foundOrigin && m_foundPlane)
			{
				if(!m_foundEnd1 && !m_foundEnd2)
				{
					if(m_lastShot.hit)
					{
						sf::Vector2i index {m_lastShot.index + m_diff};
						bool test {true};
						while(test)
						{
							if(!m_attackGrid->contains(index))
							{
								m_foundEnd1 = true;
								reverseDirection();
								test = false;
							}
							else
							{
								auto markerItr {m_markers.find(index)};
								if(markerItr != m_markers.end())
								{
									if(markerItr->second.is_hit())
									{
										index = index + m_diff;
									}
									else
									{
										m_foundEnd1 = true;
										reverseDirection();
										test = false;
									}
								}
								else
								{
									return index;
								}
							}
						}
					}
					else
					{
						m_foundEnd1 = true;
						reverseDirection();

						sf::Vector2i index {m_lastShot.index + m_diff};
						bool test {true};
						while(test)
						{
							if(!m_attackGrid->contains(index))
							{
								m_foundEnd2 = true;
								test = false;
							}
							else
							{
								auto markerItr {m_markers.find(index)};
								if(markerItr != m_markers.end())
								{
									if(markerItr->second.is_hit())
									{
										index = index + m_diff;
									}
									else
									{
										m_foundEnd2 = true;
										test = false;
									}
								}
								else
								{
									return index;
								}
							}
						}
					}
				}
				if(m_foundEnd1 && !m_foundEnd2)
				{
					if(m_lastShot.hit)
					{
						sf::Vector2i index {m_lastShot.index + m_diff};
						bool test {true};
						while(test)
						{
							if(!m_attackGrid->contains(index))
							{
								m_foundEnd2 = true;
								test = false;
							}
							else
							{
								auto markerItr {m_markers.find(index)};
								if(markerItr != m_markers.end())
								{
									if(markerItr->second.is_hit())
									{
										index = index + m_diff;
									}
									else
									{
										m_foundEnd2 = true;
										test = false;
									}
								}
								else
								{
									return index;
								}
							}
						}
					}
					else
					{
						m_foundEnd2 = true;
					}
				}
				if(m_foundEnd1 && m_foundEnd2)
				{
					m_foundOrigin = false;
					m_foundPlane = false;
					m_foundEnd1 = false;
					m_foundEnd2 = false;

					setDirection(static_cast<Dir>(random_static::get(0, 3)));

					updatePossibleShots();
					m_foundShip = sf::IntRect{0, 0, 0, 0};

					return *random_static::get(m_possibleShots);
				}
			}
			return *random_static::get(m_possibleShots);
		}
		void markShot(sf::Vector2i index, bool isHit) override
		{
			Player::markShot(index, isHit);

			m_lastShot.index = index;
			m_lastShot.hit = isHit;

			if(isHit && !m_foundEnd2)
				updateFoundShip();
		}

		void startThinking()
		{
			m_delayLimit = random_static::get(20, 60);
			m_delayCounter = 0;
		}
		void think() { ++m_delayCounter; }
		bool isThinking() const { return m_delayCounter < m_delayLimit; }
};



}


#endif