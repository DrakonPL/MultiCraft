#include <Aurora/Graphics/TextureManager.h>
#include <Aurora/Graphics/RenderManager.h>
#include <Aurora/Utils/Logger.h>

namespace Aurora
{
	namespace Graphics
	{
		TextureManager TextureManager::m_TextureManager;

		TextureManager* TextureManager::Instance()
		{
			return &m_TextureManager;
		}

		void TextureManager::AddImage(Image* image)
		{
			if(images.find(image->_fileName) == images.end())
			{
				//add to collection
				images.insert(std::pair<std::string,Image*>(image->_fileName,image));
			}
		}

		Image* TextureManager::loadImageFromFile(std::string fileName)
		{
			if(images.find(fileName) == images.end())
			{
				Image* image = new Image();
				if(image->loadImageFromFile(fileName))
				{
					//generate texture
					RenderManager::Instance()->CreateTexture(image);

					//add to collection
					images.insert(std::pair<std::string,Image*>(fileName,image));

					return image;
				}else
				{
					//error
					Utils::Logger::Instance()->LogMessage("Can't load texture file: %s \n",fileName.c_str());
					return 0;
				}
			}

			return images[fileName];
		}

		Image* TextureManager::loadImageFromMemory(std::string newName,void *data,std::size_t size)
		{
			if(images.find(newName) == images.end())
			{
				Image* image = new Image();
				if(image->loadImageFromMemory(newName,data,size))
				{
					//generate texture
					RenderManager::Instance()->CreateTexture(image);

					//add to collection
					images.insert(std::pair<std::string,Image*>(newName,image));

					return image;
				}else
				{
					//error
					return 0;
				}
			}

			return images[newName];
		}

		Image* TextureManager::createEmptyImage(std::string name,int width,int height,ImageLocation location)
		{
			if(images.find(name) == images.end())
			{
				Image* image = new Image();

				image->_fileName = name;
				image->_width = width;
				image->_height = height;

				RenderManager::Instance()->CreateEmptyTexture(image,location);

				//add to collection
				images.insert(std::pair<std::string,Image*>(name,image));

				return image;
			}

			return 0;
		}

		Image* TextureManager::getImage(std::string fileName)
		{
			if(images.find(fileName) == images.end())
			{
				return 0;
			}

			return images[fileName];
		}

		bool TextureManager::removeImage(std::string fileName)
		{
			std::map<std::string,Image*>::iterator it = images.find(fileName);

			if(it == images.end())
			{
				return false;
			}

			delete images[fileName];
			images.erase(it);
			return true;
		}

		void TextureManager::ReloadTextures()
		{
			for (std::map<std::string,Image*>::iterator it = images.begin(); it != images.end(); ++it)
			{
				//ugly way
				RenderManager::Instance()->CreateTexture(it->second);
			}
			//std::cout << it->first << " => " << it->second << '\n';
		}
	}
}
