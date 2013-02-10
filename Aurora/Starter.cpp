#include <Aurora/Utils/GameLoader.h>
#include <Aurora/Utils/GameManager.h>

#include "Tests/SimpleTest.h"
#include "Tests/ThreadTest.h"
#include "Tests/SpriteTest.h"
#include "Tests/SkyBoxTest.h"
#include "Tests/UnicodeTest.h"


class ExampleGameManager : public GameManager
{
private:

	SimpleTest* exampleState;

public:

	void Configure()
	{
		//init render manager properties
		SystemManager::Instance()->SetWindowSize(480,272);
	}

	void Init()
	{
		//init whatever you need
		exampleState = new SimpleTest();
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
