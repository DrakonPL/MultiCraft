#include <Aurora/Threads/Lock.hpp>
#include <Aurora/Threads/Mutex.hpp>


namespace Aurora
{
	namespace Threads
	{
		////////////////////////////////////////////////////////////
		Lock::Lock(Mutex& mutex) :
		m_mutex(mutex)
		{
			m_mutex.lock();
		}


		////////////////////////////////////////////////////////////
		Lock::~Lock()
		{
			m_mutex.unlock();
		}

	} // namespace sf
}
