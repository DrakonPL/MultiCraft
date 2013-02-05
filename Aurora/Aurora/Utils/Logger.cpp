#include <Aurora/Utils/Logger.h>

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

namespace Aurora
{
	namespace Utils
	{
		Logger* Logger::_logger = 0;

		Logger::Logger()
		{
			_active = true;
		}

		Logger* Logger::Instance()
		{
			if (_logger == 0)
			{
				_logger = new Logger();
			}
			return _logger;
		}

		void Logger::LogMessage(const char *message, ...)
		{
			if(_active)
			{
				va_list argList;
				char cbuffer[1024];
				va_start(argList, message);
				vsnprintf(cbuffer, 1024, message, argList);
				va_end(argList);

				// Open File for Appending
				FILE* file = fopen("Log.txt","a");
				// Opened File
				if(file != NULL)
				{
					// Write Buffer
					fwrite(cbuffer,sizeof(char),strlen(cbuffer),file);

					// Close File
					fclose(file);
				}
			}
		}

		void Logger::SetLoggerState(bool state)
		{
			_active = state;
		}
	}
}
