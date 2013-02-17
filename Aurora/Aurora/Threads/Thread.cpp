#include <Aurora/Threads/Thread.hpp>

#ifdef AURORA_PC
	#include <Aurora/Threads/win/ThreadImpl.hpp>
#endif

#ifdef AURORA_PSP
    #include <Aurora/Threads/psp/ThreadImpl.hpp>
#endif

#ifdef AURORA_ANDROID
    #include <Aurora/Threads/unix/ThreadImpl.hpp>
#endif

#ifdef AURORA_UNIX_MAC
    #include <Aurora/Threads/unix/ThreadImpl.hpp>
#endif

namespace Aurora
{
	namespace Threads
	{
		Thread::~Thread()
		{
			wait();
			delete m_entryPoint;
		}

		////////////////////////////////////////////////////////////
		void Thread::launch()
		{
			wait();
			m_impl = new priv::ThreadImpl(this);
		}

		////////////////////////////////////////////////////////////
		void Thread::wait()
		{
			if (m_impl)
			{
				m_impl->wait();
				delete m_impl;
				m_impl = NULL;
			}
		}

		////////////////////////////////////////////////////////////
		void Thread::terminate()
		{
			if (m_impl)
			{
				m_impl->terminate();
				delete m_impl;
				m_impl = NULL;
			}
		}

		////////////////////////////////////////////////////////////
		void Thread::run()
		{
			m_entryPoint->run();
		}
	}
}