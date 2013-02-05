#ifndef MUTEXIMPL_HPP
#define MUTEXIMPL_HPP

#include <windows.h>

namespace Aurora
{
	namespace Threads
	{
		namespace priv
		{
			////////////////////////////////////////////////////////////
			/// \brief Windows implementation of mutexes
			////////////////////////////////////////////////////////////
			class MutexImpl
			{
			public :

				////////////////////////////////////////////////////////////
				/// \brief Default constructor
				///
				////////////////////////////////////////////////////////////
				MutexImpl();

				////////////////////////////////////////////////////////////
				/// \brief Destructor
				///
				////////////////////////////////////////////////////////////
				~MutexImpl();

				////////////////////////////////////////////////////////////
				/// \brief Lock the mutex
				///
				////////////////////////////////////////////////////////////
				void lock();

				////////////////////////////////////////////////////////////
				/// \brief Unlock the mutex
				///
				////////////////////////////////////////////////////////////
				void unlock();

			private :

				////////////////////////////////////////////////////////////
				// Member data
				////////////////////////////////////////////////////////////
				CRITICAL_SECTION m_mutex; ///< Win32 handle of the mutex
			};

		} // namespace priv

	} // namespace sf
}


#endif // SFML_MUTEXIMPL_HPP
