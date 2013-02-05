#ifndef PSPGAMELOADER_H
#define PSPGAMELOADER_H

#include <Aurora/Utils/GameLoader.h>

namespace Aurora
{
	namespace Utils
	{
		class PspGameLoader: public GameLoader
		{
		private:

		public:

			PspGameLoader(GameManager* gameManager);

			void Run();

		};
	}
}

#endif