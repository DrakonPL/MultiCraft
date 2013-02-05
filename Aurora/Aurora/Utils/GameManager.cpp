#include <Aurora/Utils/GameManager.h>
#include <Aurora/Utils/GameState.h>

#include <stdio.h>
#include <string.h>

namespace Aurora
{
	namespace Utils
	{
		GameManager::GameManager()
		{
			m_running = true;
		}

		void GameManager::ChangeState(GameState* state)
		{
			// cleanup the current state
			if ( !states.empty() ) {
				states.back()->CleanUp();
				states.pop_back();
			}

			// store and init the new state
			states.push_back(state);
			states.back()->Enter();
		}

		void GameManager::PushState(GameState* state)
		{
			// pause current state
			if ( !states.empty() ) {
				states.back()->Pause();
			}

			// store and init the new state
			states.push_back(state);
			states.back()->Enter();
		}

		void GameManager::PopState()
		{
			// cleanup the current state
			if ( !states.empty() ) {
				states.back()->CleanUp();
				states.pop_back();
			}

			// resume previous state
			if ( !states.empty() ) {
				states.back()->Resume();
			}
		}

		void GameManager::GamePause()
		{
			states.back()->GamePause();
		}

		void GameManager::GameResume()
		{
			states.back()->GameResume();
		}

		void GameManager::HandleEvents()
		{
			states.back()->HandleEvents(this);
		}

		void GameManager::Update()
		{
			states.back()->Update(this);
		}

		void GameManager::Draw()
		{
			states.back()->Draw(this);
		}

	}
}

