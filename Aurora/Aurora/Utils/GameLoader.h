#ifndef GAMELOADER_H
#define GAMELOADER_H

#include <Aurora/Utils/GameManager.h>

namespace Aurora
{
	namespace Utils
	{
		class GameLoader
		{
		protected:

			GameManager* _gameManager;

		public:

			GameLoader(GameManager* gameManager);

			virtual void Run() = 0;

			static GameLoader* getGameLoader(GameManager* gameManager);
		};
	}
}

#endif
