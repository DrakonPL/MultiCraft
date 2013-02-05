#include <Aurora/Threads/Mutex.hpp>

#ifdef AURORA_PC
    #include <Aurora/Threads/win/MutexImpl.hpp>
#endif

#ifdef AURORA_PSP
    #include <Aurora/Threads/psp/MutexImpl.hpp>
#endif

#ifdef AURORA_ANDROID
    #include <Aurora/Threads/unix/MutexImpl.hpp>
#endif

#ifdef AURORA_IOS
#include <Aurora/Threads/unix/MutexImpl.hpp>
#endif

#ifdef AURORA_UNIX_MAC
#include <Aurora/Threads/unix/MutexImpl.hpp>
#endif



namespace Aurora
{
	namespace Threads
	{
		////////////////////////////////////////////////////////////
		Mutex::Mutex()
		{
			m_mutexImpl = new priv::MutexImpl;
		}


		////////////////////////////////////////////////////////////
		Mutex::~Mutex()
		{
			delete m_mutexImpl;
		}


		////////////////////////////////////////////////////////////
		void Mutex::lock()
		{
			m_mutexImpl->lock();
		}


		////////////////////////////////////////////////////////////
		void Mutex::unlock()
		{
			m_mutexImpl->unlock();
		}

	} // namespace sf
}