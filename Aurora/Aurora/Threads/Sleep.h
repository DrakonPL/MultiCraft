#ifndef SLEEP_H
#define SLEEP_H

namespace Aurora
{
	namespace Threads
	{
		////////////////////////////////////////////////////////////
		/// \ingroup system
		/// \brief Make the current thread sleep for a given duration
		///
		/// sf::sleep is the best way to block a program or one of its
		/// threads, as it doesn't consume any CPU power.
		///
		/// \param duration Time to sleep
		///
		////////////////////////////////////////////////////////////
		void sleep(int duration);
	}
}


#endif