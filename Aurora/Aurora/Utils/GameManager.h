#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <vector>

namespace Aurora
{
	namespace Utils
	{
		class GameState;

		class GameManager
		{
		public:

			GameManager();

			virtual void Configure() = 0;
			virtual void Init() = 0;
			virtual void CleanUp() = 0;

			void ChangeState(GameState* state);
			void PushState(GameState* state);
			void PopState();

			void GamePause();
			void GameResume();

			void HandleEvents();
			void Update();
			void Draw();

			bool Running() {return m_running;}
			void Quit() {m_running = false;}

		private:

			std::vector<GameState*> states;
			bool m_running;
		};
	}
}

#endif
