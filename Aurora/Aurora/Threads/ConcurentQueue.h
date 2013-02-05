#ifndef CONCURENT_QUEUE_HPP
#define CONCURENT_QUEUE_HPP

#include <Aurora/Threads/Mutex.hpp>
#include <Aurora/Threads/Lock.hpp>

#include <queue>

namespace Aurora
{
	namespace Threads
	{
		template<typename T>
		class ConcurentQueue
		{
		private:

			std::queue<T> _queue;
			Mutex _mutex;

		public:

			void push(const T& data)
			{
				_mutex.lock();
				_queue.push(data);
				_mutex.unlock();
			}

			int size()
			{
				Lock lock(_mutex);
				return _queue.size();
			}

			bool empty()
			{
				Lock lock(_mutex);
				return _queue.empty();
			}

			T& front()
			{
				Lock lock(_mutex);
				return _queue.front();
			}

			T& back()
			{
				Lock lock(_mutex);
				return _queue.back();
			}

			void pop()
			{
				_mutex.lock();
				_queue.pop();
				_mutex.unlock();
			}

		};
	}
}

#endif