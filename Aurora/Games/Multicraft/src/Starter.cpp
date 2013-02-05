#include <Aurora/Utils/GameLoader.h>
#include <Aurora/Utils/GameManager.h>


#include "PlayState.h"

class ExampleGameManager : public GameManager
{
private:

	PlayState* exampleState;

public:

	void Configure()
	{
		//init render manager properties
		SystemManager::Instance()->SetWindowSize(480,272);
	}

	void Init()
	{
		//init whatever you need
		exampleState = new PlayState();
		exampleState->Init();

		ChangeState(exampleState);
	}

	void CleanUp()
	{
		exampleState->CleanUp();
		delete exampleState;
	}
};

int main()
{
	ExampleGameManager* exampleGame = new ExampleGameManager();

	GameLoader* loader = GameLoader::getGameLoader(exampleGame);

	loader->Run();

	return 0;
}
