#ifndef LOGGER_H
#define LOGGER_H

#include <Aurora/Utils/GameManager.h>

namespace Aurora
{
	namespace Utils
	{
		class Logger
		{
		private:

			static Logger* _logger;

			bool _active;

		private:

			Logger();

		public:

			static Logger* Instance();

			void LogMessage(const char *message, ...);
			void SetLoggerState(bool state);
		};

	}
}

#endif
