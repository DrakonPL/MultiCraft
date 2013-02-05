#ifndef GuRenderManager_H
#define GuRenderManager_H

#include <Aurora/Graphics/RenderManager.h>
#include <Aurora/Graphics/Vertex.h>
#include <Aurora/Graphics/gu/vram.h>

#include <pspge.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <psputils.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <psptypes.h>

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>


namespace Aurora
{
	namespace Graphics
	{
		class GuRenderManager : public RenderManager
		{
		private:

			//unsigned int __attribute__((aligned(64))) _list[262144];

			void* _fbp0;
			void* _fbp1;
			void* _zbp;
			void* _frameBuffer;

			int _imageIdCounter;
			
			ScePspFMatrix4 proj;
			ScePspFMatrix4 view;
			ScePspFMatrix4 clip;

		protected:

			void _createTexture(Image* image);
			void _createTexture(unsigned char* pixels,int width,int height,unsigned int &texId);
			void _createEmptyTexture( Image* image, ImageLocation location );

		public:

			GuRenderManager();

			void Init();

			void SetOrtho();
			void SetOrtho(float left,float right,float bottom,float top,float zmin,float zmax);
			void SetPerspective();			
			void SetPerspective(float pov,float aspect,float zmin,float zmax);

			void ClearScreen();
			void StartFrame();
			void EndFrame();

			void bindTexture(Image* image);
			void bindTexture(std::string filename);
			
			//render to texture
			void CreateRenderTexture(RenderTexture* renderTexture);
			void DestroyRenderTexture(RenderTexture* renderTexture);
			void StartRenderToTexture(RenderTexture* renderTexture);
			void EndRenderToTexture(RenderTexture* renderTexture);
			void RenderToScreen();
			
			//camera
			void UpdateCurrentCamera();
			void ExtractFrustumPlanes(Frustum *frustum);
			void ExtractFrustumPlanes(Frustum *frustum,float mvpMatrix[16]);
			
			void PushMatrix();
			void PopMatrix();
			void SetMatrix(float matrix[16]);
			void MultMatrix(float matrix[16]);
			void TranslateMatrix(Vector3 pos);
			void RotateMatrix(Vector3 rotation);
			void ScaleMatrix(Vector3 scale);

			void SetColor(unsigned int col);
			void SetBlending(bool state);

			void* CreateVertexObject(VertexType vertexType,int size);
			void DrawVertexObject(void* vertexObject,int vertexCound,bool textured,VertexType vertexType,VertexPrimitive vertecPrimitive);

			/*void drawImage(Image* image);
			void drawImage(Image* image,int posx,int posy);
			void drawImage(Image* image,int posx,int posy,int width,int height);

			void drawSprite(Sprite* sprite);
			void drawSprite(Sprite* sprite,float posx,float posy);
			void drawSprite3D(Sprite3D* sprite);

			void SetTextOrtho();
			void drawText(TrueTypeFont* font,float x, float y, const char *text, int align, unsigned int col);
			

			//shapes
			void drawCube(unsigned int color,Math::Vector3 position,Math::Vector3 scale,Math::Vector3 rotation);
			void DrawCubeTextured(Image* texture,Math::Vector3 position,Math::Vector3 scale,Math::Vector3 rotation);
			void DrawModelObj(ModelObj *model,Math::Vector3 position,Math::Vector3 scale,Math::Vector3 rotation);*/
		};
	}
}

#endif
