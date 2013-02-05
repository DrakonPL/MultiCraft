#ifndef WINGAMELOADER_H
#define WINGAMELOADER_H

#include <Aurora/Utils/GameLoader.h>


namespace Aurora
{
	namespace Utils
	{
		class WinGameLoader: public GameLoader
		{
		private:

			

		public:

			WinGameLoader(GameManager* gameManager);

			void Run();

		};
	}
}

#endif