#include <Aurora/Threads/win/ThreadImpl.hpp>
#include <Aurora/Threads/Thread.hpp>
#include <cassert>
#include <process.h>


namespace Aurora
{
	namespace Threads
	{
		namespace priv
		{
		////////////////////////////////////////////////////////////
		ThreadImpl::ThreadImpl(Thread* owner)
		{
			m_thread = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, &ThreadImpl::entryPoint, owner, 0, &m_threadId));
		}


		////////////////////////////////////////////////////////////
		ThreadImpl::~ThreadImpl()
		{
			if (m_thread)
				CloseHandle(m_thread);
		}


		////////////////////////////////////////////////////////////
		void ThreadImpl::wait()
		{
			if (m_thread)
			{
				WaitForSingleObject(m_thread, INFINITE);
			}
		}


		////////////////////////////////////////////////////////////
		void ThreadImpl::terminate()
		{
			if (m_thread)
				TerminateThread(m_thread, 0);
		}


		////////////////////////////////////////////////////////////
		unsigned int __stdcall ThreadImpl::entryPoint(void* userData)
		{
			// The Thread instance is stored in the user data
			Thread* owner = static_cast<Thread*>(userData);

			// Forward to the owner
			owner->run();

			// Optional, but it is cleaner
			_endthreadex(0);

			return 0;
		}

		} // namespace priv

	} // namespace sf
}
