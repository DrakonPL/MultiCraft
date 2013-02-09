#include <Aurora/Graphics/RenderManager.h>
#include <Aurora/Graphics/TextureManager.h>

#include "fontstash_renderer.h"

unsigned char compute_y(int r, int g, int b)
{
	return (unsigned char) (((r*77) + (g*150) +  (29*b)) >> 8);
}

unsigned char *convert_format2(unsigned char *data, int img_n, int req_comp, int x, int y)
{
	int i,j;
	unsigned char *good;

	if (req_comp == img_n) return data;

	good = new unsigned char[req_comp * x * y];//(unsigned char *) malloc(req_comp * x * y);
	if (good == NULL)
	{
		//free(data);
		return 0;
	}

	for (j=0; j < (int) y; ++j)
	{
		unsigned char *src  = data + j * x * img_n   ;
		unsigned char *dest = good + j * x * req_comp;

		#define COMBO(a,b)  ((a)*8+(b))
		#define CASE(a,b)   case COMBO(a,b): for(i=x-1; i >= 0; --i, src += a, dest += b)
		// convert source image with img_n components to one with req_comp components;
		// avoid switch per pixel, so use switch per scanline and massive macros
		switch (COMBO(img_n, req_comp))
		{
			CASE(1,2) dest[0]=src[0], dest[1]=255; break;
			CASE(1,3) dest[0]=dest[1]=dest[2]=src[0]; break;
			CASE(1,4) 
			{
				dest[0] = dest[1] = dest[2] = 255;
				dest[3] = src[0];
			}
			break;
			CASE(2,1) dest[0]=src[0]; break;
			CASE(2,3) dest[0]=dest[1]=dest[2]=src[0]; break;
			CASE(2,4) dest[0]=dest[1]=dest[2]=src[0], dest[3]=src[1]; break;
			CASE(3,4) dest[0]=src[0],dest[1]=src[1],dest[2]=src[2],dest[3]=255; break;
			CASE(3,1) dest[0]=compute_y(src[0],src[1],src[2]); break;
			CASE(3,2) dest[0]=compute_y(src[0],src[1],src[2]), dest[1] = 255; break;
			CASE(4,1) dest[0]=compute_y(src[0],src[1],src[2]); break;
			CASE(4,2) dest[0]=compute_y(src[0],src[1],src[2]), dest[1] = src[3]; break;
			CASE(4,3) dest[0]=src[0],dest[1]=src[1],dest[2]=src[2]; break;

		}
		#undef CASE
	}

	//free(data);
	return good;
}

void fontstash_renderer::updateTexture(sth_texture* texture, sth_glyph* glyph, int textureWidth, int textureHeight)
{
	if (glyph)
	{
		if (texture->image->_pixels != 0)
		{
			delete [] texture->image->_pixels;
		}

		texture->image->_pixels = convert_format2(texture->m_texels,1,4,textureWidth, textureHeight);

		Aurora::Graphics::RenderManager::Instance()->UpdateTexture(texture->image);

	} else
	{
		if (textureWidth && textureHeight)
		{
			texture->m_texels = (unsigned char*)malloc(textureWidth*textureHeight);
			memset(texture->m_texels,0,textureWidth*textureHeight);

			texture->image = new Aurora::Graphics::Image();

			texture->image->_fileName = "";
			texture->image->_width = textureWidth;
			texture->image->_height = textureHeight;
			texture->image->_pixels = convert_format2(texture->m_texels,1,4,textureWidth, textureHeight);

			Aurora::Graphics::RenderManager::Instance()->CreateTexture(texture->image);
			Aurora::Graphics::TextureManager::Instance()->AddImage(texture->image);
		}
		else
		{
			//delete texture
			if (texture->m_userData)
			{
				//Aurora::Graphics::RenderManager::Instance()->remove ??
			
			}

		}

	}
}

void fontstash_renderer::render(sth_texture* texture)
{
	//add option to set this glFrontFace( GL_CW );	
	Aurora::Graphics::RenderManager::Instance()->bindTexture(texture->image);

	Aurora::Graphics::TexturedVertex* vertices = (Aurora::Graphics::TexturedVertex*)Aurora::Graphics::RenderManager::Instance()->CreateVertexObject(Aurora::Graphics::Textured,texture->nverts);

	memcpy(vertices,texture->newverts,sizeof(Aurora::Graphics::TexturedVertex) * texture->nverts);

	Aurora::Graphics::FrontFace frontFace = Aurora::Graphics::RenderManager::Instance()->GetFrontFace();
	Aurora::Graphics::RenderManager::Instance()->SetFrontFace(Aurora::Graphics::CW);

	Aurora::Graphics::RenderManager::Instance()->DrawVertexObject(vertices,texture->nverts,true,Aurora::Graphics::Textured,Aurora::Graphics::Triangle);
	
	Aurora::Graphics::RenderManager::Instance()->SetFrontFace(frontFace);
}

