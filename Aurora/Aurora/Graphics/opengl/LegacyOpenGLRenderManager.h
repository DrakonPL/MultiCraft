#ifndef LegacyOPENGLRenderManager_H
#define LegacyOPENGLRenderManager_H

#include <Aurora/Graphics/RenderManager.h>

#define GLEW_STATIC
#include <glew.h>

namespace Aurora
{
	namespace Graphics
	{
		class LegacyOpengGLRenderManager : public RenderManager
		{

		protected:

			void _createTexture(Image* image);
			void _createTexture(unsigned char* pixels,int width,int height,unsigned int &texId);
			void _createEmptyTexture( Image* image, ImageLocation location );
			static void _checkFramebufferStatus(RenderTexture* renderTexture);

		public:

			LegacyOpengGLRenderManager();

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

			//matrix stuff
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

			//shapes
			void drawCube(unsigned int color,Math::Vector3 position,Math::Vector3 scale,Math::Vector3 rotation);
			void DrawCubeTextured(Image* texture,Math::Vector3 position,Math::Vector3 scale,Math::Vector3 rotation);*/
		};
	}
}

#endif