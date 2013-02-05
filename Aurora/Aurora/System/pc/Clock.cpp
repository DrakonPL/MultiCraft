#include <Aurora/System/Clock.h>
#include <SFML/System/Clock.hpp>

namespace Aurora
{
	namespace System
	{
		sf::Clock _clock;

		Clock::Clock()
		{
			Reset();
		}

		void Clock::Reset()
		{
			_clock.Reset();
		}

		float Clock::getTime()
		{
			return _clock.GetElapsedTime();
		}
	}
}
