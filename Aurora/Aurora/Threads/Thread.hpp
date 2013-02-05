#ifndef THREAD_HPP
#define THREAD_HPP

#include <cstdlib>

namespace Aurora
{
	namespace Threads
	{
		namespace priv
		{
			class ThreadImpl;
			struct ThreadFunc;
		}

		class Thread
		{
		public:

			template <typename F>
			Thread(F function);

			////////////////////////////////////////////////////////////
			/// \brief Construct the thread from a functor with an argument
			///
			/// This constructor works for function objects, as well
			/// as free function.
			/// It is a template, which means that the argument can
			/// have any type (int, std::string, void*, Toto, ...).
			///
			/// Use this constructor for this kind of function:
			/// \code
			/// void function(int arg);
			///
			/// // --- or ----
			///
			/// struct Functor
			/// {
			///     void operator()(std::string arg);
			/// };
			/// \endcode
			/// Note: this does *not* run the thread, use Launch().
			///
			/// \param function Functor or free function to use as the entry point of the thread
			/// \param argument argument to forward to the function
			///
			////////////////////////////////////////////////////////////
			template <typename F, typename A>
			Thread(F function, A argument);

			////////////////////////////////////////////////////////////
			/// \brief Construct the thread from a member function and an object
			///
			/// This constructor is template, which means that you can
			/// use it with any class.
			/// Use this constructor for this kind of function:
			/// \code
			/// class MyClass
			/// {
			/// public :
			///
			///     void function();
			/// };
			/// \endcode
			/// Note: this does *not* run the thread, use Launch().
			///
			/// \param function Entry point of the thread
			/// \param object Pointer to the object to use
			///
			////////////////////////////////////////////////////////////
			template <typename C>
			Thread(void(C::*function)(), C* object);

			////////////////////////////////////////////////////////////
			/// \brief Destructor
			///
			/// This destructor calls Wait(), so that the internal thread
			/// cannot survive after its sf::Thread instance is destroyed.
			///
			////////////////////////////////////////////////////////////
			~Thread();

			////////////////////////////////////////////////////////////
			/// \brief Run the thread
			///
			/// This function starts the entry point passed to the
			/// thread's constructor, and returns immediately.
			/// After this function returns, the thread's function is
			/// running in parallel to the calling code.
			///
			////////////////////////////////////////////////////////////
			void launch();

			////////////////////////////////////////////////////////////
			/// \brief Wait until the thread finishes
			///
			/// This function will block the execution until the
			/// thread's function ends.
			/// Warning: if the thread function never ends, the calling
			/// thread will block forever.
			/// If this function is called from its owner thread, it
			/// returns without doing anything.
			///
			////////////////////////////////////////////////////////////
			void wait();

			////////////////////////////////////////////////////////////
			/// \brief Terminate the thread
			///
			/// This function immediately stops the thread, without waiting
			/// for its function to finish.
			/// Terminating a thread with this function is not safe,
			/// and can lead to local variables not being destroyed
			/// on some operating systems. You should rather try to make
			/// the thread function terminate by itself.
			///
			////////////////////////////////////////////////////////////
			void terminate();

		private :

			friend class priv::ThreadImpl;

			////////////////////////////////////////////////////////////
			/// \brief Internal entry point of the thread
			///
			/// This function is called by the thread implementation.
			///
			////////////////////////////////////////////////////////////
			void run();

			////////////////////////////////////////////////////////////
			// Member data
			////////////////////////////////////////////////////////////
			priv::ThreadImpl* m_impl;       ///< OS-specific implementation of the thread
			priv::ThreadFunc* m_entryPoint; ///< Abstraction of the function to run
		};

		namespace priv
		{
			// Base class for abstract thread functions
			struct ThreadFunc
			{
				virtual ~ThreadFunc() {}
				virtual void run() = 0;
			};

			// Specialization using a functor (including free functions) with no argument
			template <typename T>
			struct ThreadFunctor : ThreadFunc
			{
				ThreadFunctor(T functor) : m_functor(functor) {}
				virtual void run() {m_functor();}
				T m_functor;
			};

			// Specialization using a functor (including free functions) with one argument
			template <typename F, typename A>
			struct ThreadFunctorWithArg : ThreadFunc
			{
				ThreadFunctorWithArg(F function, A arg) : m_function(function), m_arg(arg) {}
				virtual void run() {m_function(m_arg);}
				F m_function;
				A m_arg;
			};

			// Specialization using a member function
			template <typename C>
			struct ThreadMemberFunc : ThreadFunc
			{
				ThreadMemberFunc(void(C::*function)(), C* object) : m_function(function), m_object(object) {}
				virtual void run() {(m_object->*m_function)();}
				void(C::*m_function)();
				C* m_object;
			};
		}

		////////////////////////////////////////////////////////////
		template <typename F>
		Thread::Thread(F functor) :
		m_impl      (NULL),
		m_entryPoint(new priv::ThreadFunctor<F>(functor))
		{
		}


		////////////////////////////////////////////////////////////
		template <typename F, typename A>
		Thread::Thread(F function, A argument) :
		m_impl      (NULL),
		m_entryPoint(new priv::ThreadFunctorWithArg<F, A>(function, argument))
		{
		}


		////////////////////////////////////////////////////////////
		template <typename C>
		Thread::Thread(void(C::*function)(), C* object) :
		m_impl      (NULL),
		m_entryPoint(new priv::ThreadMemberFunc<C>(function, object))
		{
		}
	}
}

#endif
