#pragma once

#include "game/Player.hpp"
#include "util/Random.hpp"

namespace game
{
using random_static = effolkronium::random_static;

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
		Shot lastShot;
		sf::Vector2i origin;
		Dir direction;
		sf::Vector2i diff;
		bool foundOrigin;
		bool foundPlane;
		bool foundEnd1;
		bool foundEnd2;
		sf::IntRect foundShip;

		int delayLimit;
		int delayCounter;

	private:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override
		{
			// for(const auto& ship : ships)
			// 	target.draw(ship, states);
			for(const auto& marker : markers)
				target.draw(marker.second, states);
		}


		void fillShips() override
		{
			sf::Vector2i start {place_grid.get_bounds().left, place_grid.get_bounds().top};
			sf::IntRect bounds {defense_grid.get_bounds()};
			for(int i = 1; i <= 5; ++i)
			{
				int shipSize {(i < 3 ? i + 1 : i)};
				ships.push_back(Ship{start, shipSize});
				do
				{
					if(random_static::get(0, 100) < 50)
						ships.back().rotate();
					sf::Vector2i offset {random_static::get(bounds.left, bounds.left + bounds.width), random_static::get(bounds.top, bounds.top + bounds.height)};
					ships.back().setOffset(offset);
				} while(!placeable(&ships.back()));
			}
		}

		void updateFoundShip()
		{
			sf::Vector2i index {lastShot.index};
			if(!foundOrigin)
			{
				foundShip = sf::IntRect{index.x, index.y, 1, 1};
			}
			else
			{
				foundShip.left = (index.x < foundShip.left ? index.x : foundShip.left);
				foundShip.top = (index.y < foundShip.top ? index.y : foundShip.top);
				if(direction == Dir::R || direction == Dir::L)
					++foundShip.width;
				if(direction == Dir::U || direction == Dir::D)
					++foundShip.height;
			}
		}
		void updatePossibleShots()
		{
			sf::IntRect rect {foundShip};
			for(int y = 0; y < rect.height + 2; ++y)
			{
				for(int x = 0; x < rect.width + 2; ++x)
				{
					sf::Vector2i index {rect.left - 1 + x, rect.top - 1 + y};
					if(possible_shots.find(index) != possible_shots.end())
						possible_shots.erase(index);
				}
			}
		}

		void setDirection(Dir d)
		{
			direction = d;
			switch(d)
			{
				case Dir::U: diff = sf::Vector2i{0, -1}; break;
				case Dir::R: diff = sf::Vector2i{1, 0}; break;
				case Dir::D: diff = sf::Vector2i{0, 1}; break;
				case Dir::L: diff = sf::Vector2i{-1, 0}; break;
				default: break;
			}
		}
		void rotateDirection()
		{
			switch(direction)
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
			switch(direction)
			{
				case Dir::U: setDirection(Dir::D); break;
				case Dir::R: setDirection(Dir::L); break;
				case Dir::D: setDirection(Dir::U); break;
				case Dir::L: setDirection(Dir::R); break;
				default: break;
			}
		}
		bool isHit(sf::Vector2i index) const { return markers.at(index).is_hit(); }

	public:
		ComputerPlayer(Grid& attack_grid, Grid& defense_grid, Grid& place_grid) :
			Player{attack_grid, defense_grid, place_grid},
			foundOrigin{false}, foundPlane{false}, foundEnd1{false}, foundEnd2{false},
			delayLimit{60}, delayCounter{0}
		{
			fillShips();
			setDirection(static_cast<Dir>(random_static::get(0, 3)));
		}

		sf::Vector2i makeShot()
		{
			if(!foundOrigin && !foundPlane)
			{
				if(lastShot.hit)
				{
					foundOrigin = true;
					origin = lastShot.index;

					sf::Vector2i index {origin + diff};
					while(!shootable(index))
					{
						rotateDirection();
						index = origin + diff;
					}
					return index;
				}
				else
				{
					return *random_static::get(possible_shots);
				}
			}

			if(foundOrigin && !foundPlane)
			{
				if(!lastShot.hit)
				{
					rotateDirection();
					sf::Vector2i index {origin + diff};
					while(!shootable(index))
					{
						rotateDirection();
						index = origin + diff;
					}
					return index;
				}
				else
				{
					foundPlane = true;

					sf::Vector2i index {lastShot.index + diff};
					bool test {true};
					while(test)
					{
						if(!attack_grid.contains(index))
						{
							foundEnd1 = true;
							reverseDirection();
							test = false;
						}
						else
						{
							auto markerItr {markers.find(index)};
							if(markerItr != markers.end())
							{
								if(markerItr->second.is_hit())
								{
									index = index + diff;
								}
								else
								{
									foundEnd1 = true;
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
			if(foundOrigin && foundPlane)
			{
				if(!foundEnd1 && !foundEnd2)
				{
					if(lastShot.hit)
					{
						sf::Vector2i index {lastShot.index + diff};
						bool test {true};
						while(test)
						{
							if(!attack_grid.contains(index))
							{
								foundEnd1 = true;
								reverseDirection();
								test = false;
							}
							else
							{
								auto markerItr {markers.find(index)};
								if(markerItr != markers.end())
								{
									if(markerItr->second.is_hit())
									{
										index = index + diff;
									}
									else
									{
										foundEnd1 = true;
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
						foundEnd1 = true;
						reverseDirection();

						sf::Vector2i index {lastShot.index + diff};
						bool test {true};
						while(test)
						{
							if(!attack_grid.contains(index))
							{
								foundEnd2 = true;
								test = false;
							}
							else
							{
								auto markerItr {markers.find(index)};
								if(markerItr != markers.end())
								{
									if(markerItr->second.is_hit())
									{
										index = index + diff;
									}
									else
									{
										foundEnd2 = true;
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
				if(foundEnd1 && !foundEnd2)
				{
					if(lastShot.hit)
					{
						sf::Vector2i index {lastShot.index + diff};
						bool test {true};
						while(test)
						{
							if(!attack_grid.contains(index))
							{
								foundEnd2 = true;
								test = false;
							}
							else
							{
								auto markerItr {markers.find(index)};
								if(markerItr != markers.end())
								{
									if(markerItr->second.is_hit())
									{
										index = index + diff;
									}
									else
									{
										foundEnd2 = true;
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
						foundEnd2 = true;
					}
				}
				if(foundEnd1 && foundEnd2)
				{
					foundOrigin = false;
					foundPlane = false;
					foundEnd1 = false;
					foundEnd2 = false;

					setDirection(static_cast<Dir>(random_static::get(0, 3)));

					updatePossibleShots();
					foundShip = sf::IntRect{0, 0, 0, 0};

					return *random_static::get(possible_shots);
				}
			}
			return *random_static::get(possible_shots);
		}
		void mark_shot(sf::Vector2i index, bool isHit) override
		{
			Player::mark_shot(index, isHit);

			lastShot.index = index;
			lastShot.hit = isHit;

			if(isHit && !foundEnd2)
				updateFoundShip();
		}

		void startThinking()
		{
			delayLimit = random_static::get(20, 60);
			delayCounter = 0;
		}
		void think() { ++delayCounter; }
		bool isThinking() const { return delayCounter < delayLimit; }
};
}