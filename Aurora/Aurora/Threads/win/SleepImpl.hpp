#ifndef SLEEPIMPL_HPP
#define SLEEPIMPL_HPP


namespace Aurora
{
	namespace Threads
	{
		namespace priv
		{
			////////////////////////////////////////////////////////////
			/// \brief Windows implementation of sf::Sleep
			///
			/// \param time Time to sleep
			///
			////////////////////////////////////////////////////////////
			void sleepImpl(int time);

		} // namespace priv

	} // namespace sf
}


#endif
