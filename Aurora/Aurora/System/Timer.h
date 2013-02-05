#ifndef TIMER_H
#define TIMER_H

namespace Aurora
{
	namespace System
	{
		class Timer
		{
		protected:

			double lastTime;

		public:

			Timer();

			void Reset();
			double getTime();
			double getDt();
		};
	}
}

#endif
