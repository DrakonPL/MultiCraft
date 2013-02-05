#include <Aurora/Graphics/RenderTexture.h>
#include <Aurora/Graphics/RenderManager.h>
#include <Aurora/Graphics/TextureManager.h>

namespace Aurora
{
	namespace Graphics
	{
		RenderTexture::RenderTexture(std::string name,int width,int height,ImageLocation location)
		{
			_status = "";

			//create empty image
			_image = TextureManager::Instance()->createEmptyImage(name,width,height,location);

			//now generate buffers by render manager if needed
			RenderManager::Instance()->CreateRenderTexture(this);
		}

		RenderTexture::~RenderTexture()
		{
			RenderManager::Instance()->DestroyRenderTexture(this);
		}
	}
}

