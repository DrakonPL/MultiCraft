#include <Aurora/Threads/unix/ThreadImpl.hpp>
#include <Aurora/Threads/Thread.hpp>

#ifdef AURORA_ANDROID
	#include "pthread_cancel.h"
#endif

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

				// Tell the thread to handle cancel requests immediatly
                #ifndef AURORA_ANDROID
				pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
                #endif

				// Forward to the owner
				owner->run();

				return NULL;
			}

		} // namespace priv

	} // namespace sf
}