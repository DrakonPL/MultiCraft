#ifndef TEXTUREPACK_H
#define TEXTUREPACK_H

#include <Aurora/Graphics/Image.h>
#include <Aurora/Graphics/RenderManager.h>
#include <Aurora/Graphics/TextureManager.h>
using namespace Aurora::Graphics;

class TexturePack
{
private:

	Image* _worldImage;

public:

	TexturePack();

	void SetWorldTexture();

};

#endif
