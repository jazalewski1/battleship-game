#pragma once

#include <random>
#include <type_traits>
#include <iostream>

namespace util
{
namespace detail
{
template<typename T>
struct is_iterator
{
private:
    static char test(...);

    template <
        typename U,
        typename = typename std::iterator_traits<U>::difference_type,
        typename = typename std::iterator_traits<U>::pointer,
        typename = typename std::iterator_traits<U>::reference,
        typename = typename std::iterator_traits<U>::value_type,
        typename = typename std::iterator_traits<U>::iterator_category
    >
    static long test(U&&);

public:
    static constexpr bool value = std::is_same<decltype(test(std::declval<T>())), long>::value;
};

template <typename T>
using is_iterator_v = typename is_iterator<T>::value;

template <typename T>
using IsIterator = std::enable_if_t<is_iterator<T>::value, bool>;

template <typename T>
using IsInteger = std::enable_if_t<std::is_integral_v<T>, bool>;

template <typename T>
using IsFloatingPoint = std::enable_if_t<std::is_floating_point_v<T>, bool>;
} // namespace detail

class RandomGenerator
{
public:
	RandomGenerator() :
		device{}, engine{device()}
	{
	}

	template <typename T, detail::IsInteger<T> = true>
	T next(T min, T max)
	{
		std::uniform_int_distribution distribution{min, max};
		return distribution(engine);
	}

	template <typename T, detail::IsFloatingPoint<T> = true>
	T next(T min, T max)
	{
		std::uniform_real_distribution distribution{min, max};
		return distribution(engine);
	}

	template <typename T, detail::IsIterator<T> = true>
	T next(T begin, T end)
	{
		if(begin == end)
		{
			return begin;
		}
        const auto valid_distance = std::distance(begin, end);
		std::uniform_int_distribution distribution{0l, valid_distance};
        const auto new_distance = distribution(engine);
		return std::next(begin, new_distance);
	}

private:
	std::random_device device;
	std::mt19937 engine;
};
} // namespace util