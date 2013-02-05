#include <Aurora/Threads/win/SleepImpl.hpp>
#include <windows.h>


namespace Aurora
{
	namespace Threads
	{
		namespace priv
		{
			////////////////////////////////////////////////////////////
			void sleepImpl(int time)
			{
				::Sleep(time);
			}

		} // namespace priv

	} // namespace sf
}
