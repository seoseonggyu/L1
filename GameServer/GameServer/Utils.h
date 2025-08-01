#pragma once
#include <random>
#include <fstream>

class Utils
{
public:
	template<typename T>
	static T GetRandom(T min, T max)
	{
		random_device randomDevice;
		default_random_engine dre(randomDevice());

		if constexpr (std::is_integral_v<T>)
		{
			std::uniform_int_distribution<T> uid(min, max);
			return uid(dre);
		}
		else
		{
			std::uniform_real_distribution<T> urd(min, max);
			return urd(dre);
		}
	}
};

