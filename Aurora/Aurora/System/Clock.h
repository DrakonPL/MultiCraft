#ifndef CLOCK_H
#define CLOCK_H

namespace Aurora
{
	namespace System
	{
		class Clock
		{
		protected:

			float lastTime;

		public:

			Clock();

			void Reset();
			float getTime();
		};
	}
}

#endif
