#include <Aurora/Threads/Sleep.h>

#ifdef AURORA_PC
	#include <Aurora/Threads/win/SleepImpl.hpp>
#endif

#ifdef AURORA_PSP
	#include <Aurora/Threads/psp/SleepImpl.hpp>
#endif

#ifdef AURORA_ANDROID
    #include <Aurora/Threads/unix/SleepImpl.hpp>
#endif

#ifdef AURORA_IOS
    #include <Aurora/Threads/unix/SleepImpl.hpp>
#endif

#ifdef AURORA_UNIX_MAC
	#include <Aurora/Threads/unix/SleepImpl.hpp>
#endif

namespace Aurora
{
	namespace Threads
	{
		////////////////////////////////////////////////////////////
		void sleep(int duration)
		{
			if (duration >= 0)
				priv::sleepImpl(duration);
		}

	}
}
