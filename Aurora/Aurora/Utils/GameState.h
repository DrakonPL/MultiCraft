#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <Aurora/Utils/GameManager.h>

namespace Aurora
{
	namespace Utils
	{

		class GameState
		{
		public:
			virtual void Init() = 0;
			virtual void Enter() = 0;
			virtual void CleanUp() = 0;

			virtual void Pause() = 0;
			virtual void Resume() = 0;

			virtual void GamePause() = 0;
			virtual void GameResume() = 0;

			virtual void HandleEvents(GameManager* sManager) = 0;
			virtual void Update(GameManager* sManager) = 0;
			virtual void Draw(GameManager* sManager) = 0;

			void ChangeState(GameManager* sManager, GameState* state)
			{
				sManager->ChangeState(state);
			}


		protected:
			GameState() { }
		};
	}
}

#endif
