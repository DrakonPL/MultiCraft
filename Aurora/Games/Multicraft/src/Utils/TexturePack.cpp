#include "TexturePack.h"

TexturePack::TexturePack()
{
	_worldImage = TextureManager::Instance()->loadImageFromFile("Assets/Minecraft/terrain.png");
}

void TexturePack::SetWorldTexture()
{
	RenderManager::Instance()->bindTexture(_worldImage);
}
