#ifndef SFUTILS_HPP_INCLUDED
#define SFUTILS_HPP_INCLUDED

#include <functional>

namespace std
{
template<>
struct hash<sf::Vector2i>
{
    size_t operator()(const sf::Vector2i& vec) const
    {
        return ((hash<float>()(vec.x) ^ (hash<float>()(vec.y) << 1)) >> 1);
    }
};
} // namespace std

#endif // SFUTILS_HPP_INCLUDED
