#include <Aurora/System/Timer.h>

#include <time.h>


#ifdef AURORA_UNIX_MAC
    #include <sys/time.h>
#elif  AURORA_PC
    #include <windows.h>
#else
    #include <sys/time.h>
#endif

namespace Aurora
{
	namespace System
	{
		Timer::Timer()
		{
			lastTime = getTime();
		}

		void Timer::Reset()
		{
			lastTime = getTime();
		}

		double Timer::getTime()
		{

#ifdef AURORA_UNIX_MAC
            
			timeval t;
			gettimeofday(&t, NULL);
			return (t.tv_sec) + (t.tv_usec/1000000.0f);
            
#elif AURORA_PC

			static LARGE_INTEGER Frequency;
			static BOOL UseHighPerformanceTimer = QueryPerformanceFrequency(&Frequency);

			if (UseHighPerformanceTimer)
			{
				// High performance counter available : use it
				LARGE_INTEGER CurrentTime;
				QueryPerformanceCounter(&CurrentTime);

				return static_cast<double>(CurrentTime.QuadPart) / Frequency.QuadPart;
			}
			else
			{
				// High performance counter not available : use GetTickCount (less accurate)
				return GetTickCount() * 0.001;
			}
#endif
		}

		double Timer::getDt()
		{
			double currentTime = getTime();
			double dt = currentTime - lastTime;
			lastTime = currentTime;

			return static_cast<float>(dt);
		}
	}
}
