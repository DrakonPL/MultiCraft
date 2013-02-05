#include <Aurora/Threads/psp/ThreadImpl.hpp>
#include <Aurora/Threads/Thread.hpp>

bool initialized = false;

namespace Aurora
{
	namespace Threads
	{
		namespace priv
		{
			////////////////////////////////////////////////////////////
			ThreadImpl::ThreadImpl(Thread* owner) :
			m_isActive(true)
			{
				if(!initialized)
				{
					pthread_init();
					initialized = true;
				}
			
				m_isActive = pthread_create(&m_thread, NULL, &ThreadImpl::entryPoint, owner) == 0;
			}


			////////////////////////////////////////////////////////////
			void ThreadImpl::wait()
			{
				if (m_isActive)
				{
					pthread_join(m_thread, NULL);
				}
			}


			////////////////////////////////////////////////////////////
			void ThreadImpl::terminate()
			{
				if (m_isActive)
					pthread_cancel(m_thread);
			}


			////////////////////////////////////////////////////////////
			void* ThreadImpl::entryPoint(void* userData)
			{
				// The Thread instance is stored in the user data
				Thread* owner = static_cast<Thread*>(userData);

				pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

				// Forward to the owner
				owner->run();

				return NULL;
			}

		} // namespace priv

	} // namespace sf
}