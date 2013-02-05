#include <Aurora/System/Clock.h>

#include <psptypes.h>
#include <stdio.h>
#include <psprtc.h>

namespace Aurora
{
	namespace System
	{

		u64 timeNow;
		u64 timeLastAsk;
		u32 tickResolution;

		Clock::Clock()
		{
			sceRtcGetCurrentTick( &timeLastAsk );
			tickResolution = sceRtcGetTickResolution();
		}

		void Clock::Reset()
		{
			
		}

		float Clock::getTime()
		{
			sceRtcGetCurrentTick( &timeNow );
			float dt = ( timeNow - timeLastAsk ) / ((float) tickResolution );
			timeLastAsk = timeNow;

			return dt;
		}
	}
}
