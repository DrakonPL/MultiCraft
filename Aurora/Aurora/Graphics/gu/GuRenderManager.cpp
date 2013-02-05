#include <Aurora/Graphics/gu/GuRenderManager.h>
#include <Aurora/Graphics/TextureManager.h>

#include <Aurora/Utils/Logger.h>

#define SCREEN_WIDTH 			480
#define SCREEN_HEIGHT 			272
#define SCREEN_WIDTH_F 			480.0f
#define SCREEN_HEIGHT_F			272.0f
#define	BUFFER_FORMAT			GU_PSM_8888
#define BUF_WIDTH 512
#define SCR_WIDTH 480
#define SCR_HEIGHT 272
#define PIXEL_SIZE				4
#define	FRAME_BUFFER_WIDTH 		512
#define FRAME_BUFFER_SIZE		FRAME_BUFFER_WIDTH*SCR_HEIGHT*PIXEL_SIZE

/*typedef struct
{
//unsigned char u, v;
//unsigned short color __attribute__((aligned(2)));
char x, y, z;
}__attribute__((packed)) Vertex8;

struct Vertex32
{
	float x,y,z;
};

struct TextureVertex
{
	float u, v;
	float x,y,z;
};*/

/*struct TextureVertex __attribute__((aligned(16))) _texturedCube[12*3] =
{
	{0, 0, -1,-1, 1}, // 0
	{1, 0, -1, 1, 1}, // 4
	{1, 1,  1, 1, 1}, // 5

	{0, 0, -1,-1, 1}, // 0
	{1, 1, 1, 1, 1}, // 5
	{0, 1, 1,-1, 1}, // 1

	{0, 0, -1,-1,-1}, // 3
	{1, 0,  1,-1,-1}, // 2
	{1, 1,  1, 1,-1}, // 6

	{0, 0, -1,-1,-1}, // 3
	{1, 1,  1, 1,-1}, // 6
	{0, 1, -1, 1,-1}, // 7

	{0, 0,  1,-1,-1}, // 0
	{1, 0,  1,-1, 1}, // 3
	{1, 1,  1, 1, 1}, // 7

	{0, 0,  1,-1,-1}, // 0
	{1, 1,  1, 1, 1}, // 7
	{0, 1,  1, 1,-1}, // 4

	{0, 0, -1,-1,-1}, // 0
	{1, 0, -1, 1,-1}, // 3
	{1, 1, -1, 1, 1}, // 7

	{0, 0, -1,-1,-1}, // 0
	{1, 1, -1, 1, 1}, // 7
	{0, 1, -1,-1, 1}, // 4

	{0, 0, -1, 1,-1}, // 0
	{1, 0,  1, 1,-1}, // 1
	{1, 1,  1, 1, 1}, // 2

	{0, 0, -1, 1,-1}, // 0
	{1, 1,  1, 1, 1}, // 2
	{0, 1, -1, 1, 1}, // 3

	{0, 0, -1,-1,-1}, // 4
	{1, 0, -1,-1, 1}, // 7
	{1, 1,  1,-1, 1}, // 6

	{0, 0, -1,-1,-1}, // 4
	{1, 1,  1,-1, 1}, // 6
	{0, 1,  1,-1,-1}, // 5
};

struct Vertex32 __attribute__((aligned(16))) _cubeVertices[12*3] =
{
	{-0.5f,-0.5f, 0.5f}, // 0
	{-0.5f, 0.5f, 0.5f}, // 4
	{ 0.5f, 0.5f, 0.5f}, // 5

	{-0.5f,-0.5f, 0.5f}, // 0
	{ 0.5f, 0.5f, 0.5f}, // 5
	{ 0.5f,-0.5f, 0.5f}, // 1

	{-0.5f,-0.5f,-0.5f}, // 3
	{ 0.5f,-0.5f,-0.5f}, // 2
	{ 0.5f, 0.5f,-0.5f}, // 6

	{-0.5f,-0.5f,-0.5f}, // 3
	{ 0.5f, 0.5f,-0.5f}, // 6
	{-0.5f, 0.5f,-0.5f}, // 7

	{ 0.5f,-0.5f,-0.5f}, // 0
	{ 0.5f,-0.5f, 0.5f}, // 3
	{ 0.5f, 0.5f, 0.5f}, // 7

	{ 0.5f,-0.5f,-0.5f}, // 0
	{ 0.5f, 0.5f, 0.5f}, // 7
	{ 0.5f, 0.5f,-0.5f}, // 4

	{-0.5f,-0.5f,-0.5f}, // 0
	{-0.5f, 0.5f,-0.5f}, // 3
	{-0.5f, 0.5f, 0.5f}, // 7

	{-0.5f,-0.5f,-0.5f}, // 0
	{-0.5f, 0.5f, 0.5f}, // 7
	{-0.5f,-0.5f, 0.5f}, // 4

	{-0.5f, 0.5f,-0.5f}, // 0
	{ 0.5f, 0.5f,-0.5f}, // 1
	{ 0.5f, 0.5f, 0.5f}, // 2

	{-0.5f, 0.5f,-0.5f}, // 0
	{ 0.5f, 0.5f, 0.5f}, // 2
	{-0.5f, 0.5f, 0.5f}, // 3

	{-0.5f,-0.5f,-0.5f}, // 4
	{-0.5f,-0.5f, 0.5f}, // 7
	{ 0.5f,-0.5f, 0.5f}, // 6

	{-0.5f,-0.5f,-0.5f}, // 4
	{ 0.5f,-0.5f, 0.5f}, // 6
	{ 0.5f,-0.5f,-0.5f}, // 5
};*/

static unsigned int __attribute__((aligned(16))) _list[262144];

namespace Aurora
{
	namespace Graphics
	{
		GuRenderManager::GuRenderManager()
		{
			// default values
			_width = 480;
			_height = 272;
			_pov = 53;
			_vSync = true;
			_fullScreen = false;
			_zOtrhoMin = -10.0f;
			_zOtrhoMax = 10.0f;
			_zMin = 0.1f;
			_zMax = 256.0f;

			//gu settings
			_fbp0 = 0;
			_fbp1 = 0;
			_zbp = 0;

			//texture stuff
			_currentTexture = -1;
			_imageIdCounter = 0;
			
		}

		void GuRenderManager::Init()
		{
			sceGuInit();
			
			_fbp0 = ( u32* ) valloc ( FRAME_BUFFER_SIZE ) ;
			_fbp1 = ( u32* ) valloc ( FRAME_BUFFER_SIZE );
			_zbp = ( u16* )  valloc ( FRAME_BUFFER_WIDTH*SCREEN_HEIGHT*2);

			_frameBuffer = vabsptr(vrelptr (_fbp0));

			sceGuStart(GU_DIRECT,_list);

			sceGuDrawBuffer(BUFFER_FORMAT, vrelptr (_fbp0), FRAME_BUFFER_WIDTH);
			sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, vrelptr (_fbp1), FRAME_BUFFER_WIDTH);
			sceGuDepthBuffer(vrelptr (_zbp), FRAME_BUFFER_WIDTH);

			sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
			sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
			//sceGuDepthRange(65535,0);
			sceGuDepthRange(50000, 10000);
			sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
			sceGuEnable(GU_SCISSOR_TEST);
			sceGuDepthFunc(GU_GEQUAL);
			sceGuEnable(GU_DEPTH_TEST);			

			sceGuDisable(GU_TEXTURE_2D);
			sceGuDisable(GU_CLIP_PLANES);
			
			sceGuEnable(GU_CULL_FACE);
            sceGuFrontFace(GU_CCW);


			sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);

			unsigned int test = 0;
			unsigned char* aa;
			_createTexture(aa,0,0,test);

			sceGuFinish();
			sceGuSync(0,0);

			sceDisplayWaitVblankStartCB();
			_frameBuffer = vabsptr(sceGuSwapBuffers());
			sceGuDisplay(GU_TRUE);

			sceCtrlSetSamplingCycle(0);
			sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

			sceKernelDcacheWritebackInvalidateAll();
		}

		void GuRenderManager::SetOrtho()
		{
			sceGumMatrixMode(GU_PROJECTION);
			sceGumLoadIdentity();
			sceGumOrtho(0, _width, _height, 0, _zOtrhoMin,_zOtrhoMax);

			sceGumMatrixMode(GU_VIEW);
			sceGumLoadIdentity();

			sceGumMatrixMode(GU_MODEL);
			sceGumLoadIdentity();
		}

		void GuRenderManager::SetOrtho(float left,float right,float bottom,float top,float zmin,float zmax)
		{
			sceGumMatrixMode(GU_PROJECTION);
			sceGumLoadIdentity();
			sceGumOrtho(left, right, bottom, top, zmin, zmax);

			sceGumMatrixMode(GU_VIEW);
			sceGumLoadIdentity();

			sceGumMatrixMode(GU_MODEL);
			sceGumLoadIdentity();
		}

		void GuRenderManager::SetPerspective()
		{
			sceGumMatrixMode(GU_PROJECTION);
			sceGumLoadIdentity();
			sceGumPerspective(_pov,(float)_width/(float)_height,_zMin,_zMax);
			sceGumStoreMatrix(&proj);

			sceGumMatrixMode(GU_VIEW);
			sceGumLoadIdentity();
			sceGumStoreMatrix(&view);

			sceGumMatrixMode(GU_MODEL);
			sceGumLoadIdentity();
		}

		void GuRenderManager::SetPerspective(float pov,float aspect,float zmin,float zmax)
		{
			sceGumMatrixMode(GU_PROJECTION);
			sceGumLoadIdentity();
			sceGumPerspective(pov,aspect,zmin,zmax);
			sceGumStoreMatrix(&proj);

			sceGumMatrixMode(GU_VIEW);
			sceGumLoadIdentity();
			sceGumStoreMatrix(&view);

			sceGumMatrixMode(GU_MODEL);
			sceGumLoadIdentity();
		}

		void GuRenderManager::ClearScreen()
		{
			sceGuClearColor(0xff554433);
			sceGuClearStencil(0);
			sceGuClearDepth(0);
			
			sceGuClear(GU_COLOR_BUFFER_BIT | GU_STENCIL_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
		}

		void GuRenderManager::StartFrame()
		{
			sceGuStart(GU_DIRECT,_list);

			_currentTexture = -1;
			//sceGuClearColor(0xff554433);
			//sceGuClearDepth(0);
			//sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
		}

		void GuRenderManager::EndFrame()
		{
			sceGuFinish();
			sceGuSync(0,0);

			//sceDisplayWaitVblankStart();
			_frameBuffer = vabsptr(sceGuSwapBuffers());
		}
		
		void GuRenderManager::CreateRenderTexture(RenderTexture* renderTexture)
		{
		
		}
		
		void GuRenderManager::DestroyRenderTexture(RenderTexture* renderTexture)
		{
		
		}
		
		void GuRenderManager::StartRenderToTexture(RenderTexture* renderTexture)
		{
			sceGuSync(0, 2);

			sceKernelDcacheWritebackAll();

			sceGuDrawBufferList(GU_PSM_8888, vrelptr(renderTexture->GetImage()->_pixels), renderTexture->GetImage()->_width);

			sceGuOffset(2048 - (renderTexture->GetImage()->_width>>1), 2048 - (renderTexture->GetImage()->_height>>1));
			sceGuViewport(2048, 2048, renderTexture->GetImage()->_width, renderTexture->GetImage()->_height);
			sceGuScissor(0, 0, renderTexture->GetImage()->_width, renderTexture->GetImage()->_height);
		}
		
		void GuRenderManager::EndRenderToTexture(RenderTexture* renderTexture)
		{
		
		}
		
		void GuRenderManager::RenderToScreen()
		{
			sceGuSync(0, 2);

			sceKernelDcacheWritebackAll();

			sceGuDrawBufferList(GU_PSM_8888, vrelptr(_frameBuffer), FRAME_BUFFER_WIDTH);

			sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
			sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
			sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
		}

		void GuRenderManager::UpdateCurrentCamera()
		{
			if(_currentCam != 0)
			{
				sceGumMatrixMode(GU_PROJECTION);
				sceGumLoadIdentity();
				sceGumLoadMatrix(&proj);

				sceGumMatrixMode(GU_VIEW);
				sceGumLoadIdentity();

				ScePspFVector3 pos = {_currentCam->m_vPosition.x + _currentCam->m_vOffset.x,_currentCam->m_vPosition.y + _currentCam->m_vOffset.y,_currentCam->m_vPosition.z + _currentCam->m_vOffset.z};
				ScePspFVector3 eye = {_currentCam->m_vView.x,_currentCam->m_vView.y,_currentCam->m_vView.z};
				ScePspFVector3 up = {_currentCam->m_vUpVector.x,_currentCam->m_vUpVector.y,_currentCam->m_vUpVector.z};

				sceGumLookAt(&pos, &eye, &up);
				sceGumUpdateMatrix();
				sceGumStoreMatrix(&view);

				sceGumMatrixMode(GU_MODEL);
				sceGumLoadIdentity();
			}
		}
		
		void GuRenderManager::ExtractFrustumPlanes(Frustum *frustum)
		{
			gumMultMatrix(&clip,&proj,&view);
			
			Plane *pPlane = 0;

			//left plane
			pPlane = &frustum->planes[Frustum::FRUSTUM_PLANE_LEFT];
			pPlane->set(clip.x.w + clip.x.x,clip.y.w + clip.y.x,clip.z.w + clip.z.x,clip.w.w + clip.w.x);
			pPlane->normalize();			
			
			//right plane
			pPlane = &frustum->planes[Frustum::FRUSTUM_PLANE_RIGHT];
			pPlane->set(clip.x.w - clip.x.x,clip.y.w - clip.y.x,clip.z.w - clip.z.x,clip.w.w - clip.w.x);
			pPlane->normalize();
			
			// Top clipping plane.
			pPlane = &frustum->planes[Frustum::FRUSTUM_PLANE_TOP];
			pPlane->set(clip.x.w - clip.x.y,clip.y.w - clip.y.y,clip.z.w - clip.z.y,clip.w.w - clip.w.y);
			pPlane->normalize();
			
			// Bottom clipping plane.
			pPlane = &frustum->planes[Frustum::FRUSTUM_PLANE_BOTTOM];
			pPlane->set(clip.x.w + clip.x.y,clip.y.w + clip.y.y,clip.z.w + clip.z.y,clip.w.w + clip.w.y);
			pPlane->normalize();
		
			// Far clipping plane.
			pPlane = &frustum->planes[Frustum::FRUSTUM_PLANE_FAR];
			pPlane->set(clip.x.w - clip.x.z,clip.y.w - clip.y.z,clip.z.w - clip.z.z,clip.w.w - clip.w.z);
			pPlane->normalize();
			
			// Near clipping plane.
			pPlane = &frustum->planes[Frustum::FRUSTUM_PLANE_NEAR];
			pPlane->set(clip.x.w + clip.x.z,clip.y.w + clip.y.z,clip.z.w + clip.z.z,clip.w.w + clip.w.z);
			pPlane->normalize();						
		}
		
		void GuRenderManager::ExtractFrustumPlanes(Frustum *frustum,float mvpMatrix[16])
		{
			Plane *pPlane = 0;

			//left plane
			pPlane = &frustum->planes[Frustum::FRUSTUM_PLANE_LEFT];
			pPlane->set(
				mvpMatrix[ 3] + mvpMatrix[ 0],
				mvpMatrix[ 7] + mvpMatrix[ 4],
				mvpMatrix[11] + mvpMatrix[ 8],
				mvpMatrix[15] + mvpMatrix[12]);
			pPlane->normalize();

			//right plane
			pPlane = &frustum->planes[Frustum::FRUSTUM_PLANE_RIGHT];
			pPlane->set(
				mvpMatrix[ 3] - mvpMatrix[ 0],
				mvpMatrix[ 7] - mvpMatrix[ 4],
				mvpMatrix[11] - mvpMatrix[ 8],
				mvpMatrix[15] - mvpMatrix[12]);
			pPlane->normalize();

			// Top clipping plane.
			pPlane = &frustum->planes[Frustum::FRUSTUM_PLANE_TOP];
			pPlane->set(
				mvpMatrix[ 3] - mvpMatrix[ 1],
				mvpMatrix[ 7] - mvpMatrix[ 5],
				mvpMatrix[11] - mvpMatrix[ 9],
				mvpMatrix[15] - mvpMatrix[13]);
			pPlane->normalize();

			// Bottom clipping plane.
			pPlane = &frustum->planes[Frustum::FRUSTUM_PLANE_BOTTOM];
			pPlane->set(
				mvpMatrix[ 3] + mvpMatrix[ 1],
				mvpMatrix[ 7] + mvpMatrix[ 5],
				mvpMatrix[11] + mvpMatrix[ 9],
				mvpMatrix[15] + mvpMatrix[13]);
			pPlane->normalize();

			// Far clipping plane.
			pPlane = &frustum->planes[Frustum::FRUSTUM_PLANE_FAR];
			pPlane->set(
				mvpMatrix[ 3] - mvpMatrix[ 2],
				mvpMatrix[ 7] - mvpMatrix[ 6],
				mvpMatrix[11] - mvpMatrix[10],
				mvpMatrix[15] - mvpMatrix[14]);
			pPlane->normalize();

			// Near clipping plane.
			pPlane = &frustum->planes[Frustum::FRUSTUM_PLANE_NEAR];
			pPlane->set(
				mvpMatrix[ 3] + mvpMatrix[ 2],
				mvpMatrix[ 7] + mvpMatrix[ 6],
				mvpMatrix[11] + mvpMatrix[10],
				mvpMatrix[15] + mvpMatrix[14]);
			pPlane->normalize();
		}

		void GuRenderManager::_createTexture(Image* image)
		{
			if (image != 0)
			{
				image->_id = _imageIdCounter;
				_imageIdCounter++;
			}
		}

		void GuRenderManager::_createTexture(unsigned char* pixels,int width,int height,unsigned int &texId)
		{
			texId = _imageIdCounter;
			_imageIdCounter++;
		}

		void GuRenderManager::_createEmptyTexture( Image* image, ImageLocation location )
		{
			image->_id = _imageIdCounter;
			_imageIdCounter++;

			int size = image->_width * image->_height * 4;
			image->_pixels = (unsigned char*)valloc(size);
		}

		void GuRenderManager::bindTexture(Image* image)
		{
			if(image->_id != _currentTexture)
			{
				sceGuTexMode(GU_PSM_8888,0,0,0);
				sceGuTexImage(0,image->_width,image->_height,image->_width,image->_pixels);

				sceGuTexFunc(GU_TFX_MODULATE,GU_TCC_RGBA);//GU_TFX_REPLACE
				sceGuTexFilter(GU_NEAREST,GU_NEAREST);//GU_LINEAR
				sceGuTexOffset( 0.0f, 0.0f );
				sceGuTexWrap(GU_REPEAT,GU_REPEAT);

				_currentTexture = image->_id;
			}
		}

		void GuRenderManager::bindTexture(std::string filename)
		{
			Image* image = TextureManager::Instance()->getImage(filename);

			if(image->_id != _currentTexture)
			{
				sceGuTexMode(GU_PSM_8888,0,0,0);
				sceGuTexImage(0,image->_width,image->_height,image->_width,image->_pixels);

				sceGuTexFunc(GU_TFX_MODULATE,GU_TCC_RGBA);
				sceGuTexFilter(GU_NEAREST,GU_NEAREST);//GU_NEAREST
				sceGuTexOffset( 0.0f, 0.0f );
				sceGuTexWrap(GU_REPEAT,GU_REPEAT);

				_currentTexture = image->_id;
			}
		}

		void GuRenderManager::PushMatrix()
		{
			sceGumPushMatrix();
		}
		
		void GuRenderManager::PopMatrix()
		{
			sceGumPopMatrix();
		}
		
		void GuRenderManager::SetMatrix(float matrix[16])
		{
			sceGumLoadMatrix((ScePspFMatrix4*)matrix);
		}
		
		void GuRenderManager::MultMatrix(float matrix[16])
		{
			sceGumMultMatrix((ScePspFMatrix4*)matrix);
		}
		
		void GuRenderManager::TranslateMatrix(Vector3 pos)
		{
			ScePspFVector3 loc = {pos.x,pos.y,pos.z};
			sceGumTranslate(&loc);
		}
		
		void GuRenderManager::RotateMatrix(Vector3 rotation)
		{
			ScePspFVector3 rot = {Aurora::Math::Math::degreesToRadians(rotation.x),Aurora::Math::Math::degreesToRadians(rotation.y),Aurora::Math::Math::degreesToRadians(rotation.z)};
			sceGumRotateXYZ(&rot);
		}
		
		void GuRenderManager::ScaleMatrix(Vector3 scale)
		{
			ScePspFVector3 sca = {scale.x,scale.y,scale.z};
			sceGumScale(&sca);
		}

		void GuRenderManager::SetColor(unsigned int col)
		{
			sceGuColor(col);
		}
		
		void GuRenderManager::SetBlending(bool state)
		{
			if(state)
			{
				sceGuEnable(GU_BLEND);
			}else
			{
				sceGuDisable(GU_BLEND);
			}
		}

		void* GuRenderManager::CreateVertexObject(VertexType vertexType,int size)
		{
			switch(vertexType)
			{
				case Simple :
					return (SimpleVertex*)sceGuGetMemory(size * sizeof(SimpleVertex));
				break;

				case Textured :
					return (TexturedVertex*)sceGuGetMemory(size * sizeof(TexturedVertex));
				break;

				case TextureColor :
					return (MultiVertex*)sceGuGetMemory(size * sizeof(MultiVertex));
				break;
			}

			return 0;
		}
		void GuRenderManager::DrawVertexObject(void* vertexObject,int vertexCound,bool textured,VertexType vertexType,VertexPrimitive vertecPrimitive)
		{
			if (vertexObject == 0)
				return;

			if (textured)
			{
				sceGuEnable(GU_TEXTURE_2D);
				sceGuEnable(GU_BLEND);
			}

			switch(vertexType)
			{
				case Simple :
				{
					SimpleVertex* vertices = (SimpleVertex*)vertexObject;
					sceGumDrawArray(GU_TRIANGLES, GU_VERTEX_32BITF|GU_TRANSFORM_3D, vertexCound, 0, vertices);
				}
				break;

				case Textured :
				{
					TexturedVertex* vertices = (TexturedVertex*)vertexObject;
					sceGumDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D, vertexCound, 0, vertices);
				}
				break;

				case TextureColor :				
				{

				}
				break;
			}

			if (textured)
			{
				sceGuDisable(GU_BLEND);
				sceGuDisable(GU_TEXTURE_2D);
			}
		}
		
		/*void GuRenderManager::_createModelObjVertices(ModelObj* model)
		{
			for (unsigned int i = 0; i < model->mMeshes.size();i++)
			{
				ObjMesh *mesh = model->mMeshes[i];

				//build mesh vertices
				mesh->meshVertices =  (TexturesPSPVertex*)memalign(16,( mesh->mFace.size() * 3) * sizeof(TexturesPSPVertex));
				mesh->triangleCount = mesh->mFace.size();
				mesh->vertexCount = mesh->mFace.size() * 3;

				int j =0;

				for(unsigned int f = 0;f < mesh->mFace.size();f++)
				{
					TexturesPSPVertex vert1;
					vert1.u = model->allUVMap[mesh->mUVFace[f].x].u;
					vert1.v = model->allUVMap[mesh->mUVFace[f].x].v;
					vert1.x = model->allVertex[mesh->mFace[f].x].x;
					vert1.y = model->allVertex[mesh->mFace[f].x].y;
					vert1.z = model->allVertex[mesh->mFace[f].x].z;
					memcpy(&mesh->meshVertices[j],&vert1,sizeof(TexturesPSPVertex));

					mesh->aabb.expandToInclude(model->allVertex[mesh->mFace[f].x]);
					j++;

					TexturesPSPVertex vert2;
					vert2.u = model->allUVMap[mesh->mUVFace[f].y].u;
					vert2.v = model->allUVMap[mesh->mUVFace[f].y].v;
					vert2.x = model->allVertex[mesh->mFace[f].y].x;
					vert2.y = model->allVertex[mesh->mFace[f].y].y;
					vert2.z = model->allVertex[mesh->mFace[f].y].z;
					memcpy(&mesh->meshVertices[j],&vert2,sizeof(TexturesPSPVertex));

					mesh->aabb.expandToInclude(model->allVertex[mesh->mFace[f].y]);
					j++;

					TexturesPSPVertex vert3;
					vert3.u = model->allUVMap[mesh->mUVFace[f].z].u;
					vert3.v = model->allUVMap[mesh->mUVFace[f].z].v;
					vert3.x = model->allVertex[mesh->mFace[f].z].x;
					vert3.y = model->allVertex[mesh->mFace[f].z].y;
					vert3.z = model->allVertex[mesh->mFace[f].z].z;
					memcpy(&mesh->meshVertices[j],&vert3,sizeof(TexturesPSPVertex));

					mesh->aabb.expandToInclude(model->allVertex[mesh->mFace[f].z]);
					j++;

				}
				mesh->mUVFace.clear();
				mesh->mFace.clear();
				mesh->mNormalFace.clear();

				//clear the cache or there will be some errors
				//sceKernelDcacheWritebackInvalidateRange(mesh->meshVertices,( mesh->mFace.size() * 3) * sizeof(TexturesPSPVertex));
				sceKernelDcacheWritebackInvalidateAll();
			}

			model->allVertex.clear();
			model->allNormal.clear();
			model->allUVMap.clear();

			sceKernelDcacheWritebackInvalidateAll();
		}*/

		/*void GuRenderManager::drawImage(Image* image)
		{
			bindTexture(image);

			sceGuEnable(GU_TEXTURE_2D);
			sceGuEnable(GU_BLEND);

			TexturedVertex* vertices = (TexturedVertex*)sceGuGetMemory(4 * sizeof(TexturedVertex));

			vertices[0].u = 0.0f; vertices[0].v = 0.0f; vertices[0].x = 0; vertices[0].y = 0; vertices[0].z =0;
			vertices[1].u = 0.0f; vertices[1].v = 1.0f; vertices[1].x = 0; vertices[1].y = image->_height; vertices[1].z =0;
			vertices[2].u = 1.0f; vertices[2].v = 0.0f; vertices[2].x = image->_width; vertices[2].y = 0; vertices[2].z =0;
			vertices[3].u = 1.0f; vertices[3].v = 1.0f; vertices[3].x = image->_width; vertices[3].y = image->_height; vertices[3].z =0;

			sceGuColor(0xffffffff);
			sceGumDrawArray(GU_TRIANGLE_STRIP, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D, 4, 0, vertices);

			sceGuDisable(GU_BLEND);
			sceGuDisable(GU_TEXTURE_2D);
		}

		void GuRenderManager::drawImage(Image* image,int posx,int posy)
		{
			bindTexture(image);
		}

		void GuRenderManager::drawImage(Image* image,int posx,int posy,int width,int height)
		{
			bindTexture(image);

			sceGuEnable(GU_TEXTURE_2D);
			sceGuEnable(GU_BLEND);
			
			sceGumDrawArray(GU_TRIANGLE_STRIP, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D, 4, 0, vertices);

			TexturedVertex* vertices = (TexturedVertex*)sceGuGetMemory(4 * sizeof(TexturedVertex));

			vertices[0].u = 0.0f; vertices[0].v = 0.0f; vertices[0].x = posx; vertices[0].y = posy; vertices[0].z =0;
			vertices[1].u = 0.0f; vertices[1].v = 1.0f; vertices[1].x = posx; vertices[1].y = posy + height; vertices[1].z =0;
			vertices[2].u = 1.0f; vertices[2].v = 0.0f; vertices[2].x = posx+width; vertices[2].y = posy; vertices[2].z =0;
			vertices[3].u = 1.0f; vertices[3].v = 1.0f; vertices[3].x = posx+width; vertices[3].y = posy + height; vertices[3].z =0;

			sceGuColor(0xffffffff);
			sceGumDrawArray(GU_TRIANGLE_STRIP, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D, 4, 0, vertices);

			sceGuDisable(GU_BLEND);
			sceGuDisable(GU_TEXTURE_2D);
		}

		float g_tabStops2[4] = {150, 210, 270, 330};
		void GuRenderManager::drawText(TrueTypeFont* font,float x, float y, const char *text, int align, unsigned int col)
		{
			if (!font->g_ftex) return;
			if (!text) return;

			_currentTexture = font->g_ftex;

			y = _height - y;

			if (align == Graphics::ALIGN_CENTER)
				x -= font->getTextLength(font->g_cdata, text)/2;
			else if (align == Graphics::ALIGN_RIGHT)
				x -= font->getTextLength(font->g_cdata, text);

			sceGuTexMode(GU_PSM_8888,0,0,0);
			sceGuTexImage(0,256,256,256,font->bitmap);

			sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
			sceGuTexFilter(GU_NEAREST,GU_NEAREST);
			sceGuTexOffset( 0.0f, 0.0f );
			sceGuTexWrap(GU_REPEAT,GU_REPEAT);

			sceGuColor(GU_RGBA(col&0xff, (col>>8)&0xff, (col>>16)&0xff, (col>>24)&0xff));
			sceGuEnable(GU_TEXTURE_2D);
			sceGuEnable(GU_BLEND);

			const float ox = x;

			while (*text)
			{
				int c = (unsigned char)*text;
				if (c == '\t')
				{
					for (int i = 0; i < 4; ++i)
					{
						if (x < g_tabStops2[i]+ox)
						{
							x = g_tabStops2[i]+ox;
							break;
						}
					}
				}
				else if (c >= 32 && c < 128)
				{
					stbtt_aligned_quad2 q;
					font->getBakedQuad(font->g_cdata, 256,256, c-32, &x,&y,&q);

					TexturedVertex* vertices = (TexturedVertex*)sceGuGetMemory(6 * sizeof(TexturedVertex));

					vertices[0].u = q.s0; vertices[0].v = q.t0; vertices[0].x =q.x0; vertices[0].y = q.y0; vertices[0].z =0;
					vertices[1].u = q.s1; vertices[1].v = q.t1; vertices[1].x =q.x1; vertices[1].y = q.y1; vertices[1].z =0;
					vertices[2].u = q.s1; vertices[2].v = q.t0; vertices[2].x =q.x1; vertices[2].y = q.y0; vertices[2].z =0;

					vertices[3].u = q.s0; vertices[3].v = q.t0; vertices[3].x =q.x0; vertices[3].y = q.y0; vertices[3].z =0;
					vertices[4].u = q.s0; vertices[4].v = q.t1; vertices[4].x =q.x0; vertices[4].y = q.y1; vertices[4].z =0;
					vertices[5].u = q.s1; vertices[5].v = q.t1; vertices[5].x =q.x1; vertices[5].y = q.y1; vertices[5].z =0;

					sceGumDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D, 6, 0, vertices);
				}
				++text;
			}


			sceGuDisable(GU_BLEND);
			sceGuDisable(GU_TEXTURE_2D);

			sceGuColor(0xffffffff);
		}

		void GuRenderManager::drawSprite(Sprite* sprite)
		{
			bindTexture(sprite->imageName);

			sceGumPushMatrix();

			ScePspFVector3 loc = {sprite->posX,sprite->posY,0.0f};
			sceGumTranslate(&loc);

			sceGuEnable(GU_TEXTURE_2D);
			sceGuEnable(GU_BLEND);

			sceGumDrawArray(GU_TRIANGLE_STRIP, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D, 4, 0, sprite->vertices);

			sceGuDisable(GU_BLEND);
			sceGuDisable(GU_TEXTURE_2D);

			sceGumPopMatrix();
		}
		
		void GuRenderManager::drawSprite(Sprite* sprite,float posx,float posy)
		{
			bindTexture(sprite->imageName);

			sceGumPushMatrix();

			ScePspFVector3 loc = {posx,posy,0.0f};
			sceGumTranslate(&loc);

			sceGuEnable(GU_TEXTURE_2D);
			sceGuEnable(GU_BLEND);

			sceGumDrawArray(GU_TRIANGLE_STRIP, GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D, 4, 0, sprite->vertices);

			sceGuDisable(GU_BLEND);
			sceGuDisable(GU_TEXTURE_2D);

			sceGumPopMatrix();
		}

		void GuRenderManager::drawCube(unsigned int color,Math::Vector3 position,Math::Vector3 scale,Math::Vector3 rotation)
		{
			sceGumPushMatrix();
			
			ScePspFVector3 loc = {position.x,position.y,position.z};
			sceGumTranslate(&loc);

			ScePspFVector3 rot = {Aurora::Math::Math::degreesToRadians(rotation.x),Aurora::Math::Math::degreesToRadians(rotation.y),Aurora::Math::Math::degreesToRadians(rotation.z)};
			sceGumRotateXYZ(&rot);			

			sceGuColor(GU_RGBA(color&0xff, (color>>8)&0xff, (color>>16)&0xff, (color>>24)&0xff));

			sceGumDrawArray(GU_TRIANGLES,GU_VERTEX_32BITF|GU_TRANSFORM_3D,12*3, 0,_cubeVertices);

			sceGuColor(0xffffffff);
			sceGumPopMatrix();
		}

		void GuRenderManager::DrawCubeTextured(Image* texture,Math::Vector3 position,Math::Vector3 scale,Math::Vector3 rotation)
		{
			sceGumPushMatrix();
			
			ScePspFVector3 loc = {position.x,position.y,position.z};
			sceGumTranslate(&loc);

			ScePspFVector3 rot = {Aurora::Math::Math::degreesToRadians(rotation.x),Aurora::Math::Math::degreesToRadians(rotation.y),Aurora::Math::Math::degreesToRadians(rotation.z)};
			sceGumRotateXYZ(&rot);			

			sceGuColor(GU_RGBA(255, 255, 255, 255));
			bindTexture(texture);
			sceGuEnable(GU_TEXTURE_2D);

			sceGumDrawArray(GU_TRIANGLES,GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,12*3, 0,_texturedCube);
			sceGuDisable(GU_TEXTURE_2D);

			sceGuColor(0xffffffff);
			sceGumPopMatrix();
		}

		void GuRenderManager::drawSprite3D(Sprite3D* sprite)
		{
			sceGumPushMatrix();

			ScePspFVector3 loc = {sprite->posX,sprite->posY,sprite->posZ};
			ScePspFVector3 rot = {Aurora::Math::Math::degreesToRadians(sprite->rotationX),Aurora::Math::Math::degreesToRadians(sprite->rotationY),Aurora::Math::Math::degreesToRadians(sprite->rotationZ)};
			ScePspFVector3 sca = {sprite->scaleX,sprite->scaleY,sprite->scaleZ};

			sceGumTranslate(&loc);
			sceGumRotateXYZ(&rot);
			sceGumScale(&sca);

			int posx = 0;
			int posy = 0;

			for (int y = sprite->height/2;y > -sprite->height/2;y--)
			{
				for (int x = -sprite->width/2;x < sprite->width/2;x++)
				{
					unsigned int colour = sprite->colours[posx + posy*sprite->height];
					if(colour != 0)
						drawCube(colour,Math::Vector3(x,y,0),Math::Vector3(1,1,1),Math::Vector3(0,0,0));
					posx++;
				}
				posx = 0;
				posy++;
			}

			sceGumPopMatrix();
		}

		void GuRenderManager::DrawModelObj(ModelObj *model,Math::Vector3 position,Math::Vector3 scale,Math::Vector3 rotation)
		{
			sceGumPushMatrix();

			ScePspFVector3 loc = {position.x,position.y,position.z};
			ScePspFVector3 rot = {Aurora::Math::Math::degreesToRadians(rotation.x),Aurora::Math::Math::degreesToRadians(rotation.y),Aurora::Math::Math::degreesToRadians(rotation.z)};
			ScePspFVector3 sca = {scale.x,scale.y,scale.z};

			sceGumTranslate(&loc);
			sceGumRotateXYZ(&rot);
			sceGumScale(&sca);

			sceGuColor(0xffffffff);
			sceGuEnable(GU_TEXTURE_2D);

			unsigned int count = model->mMeshes.size();

			for(unsigned int i = 0;i < count;i++)
			{
				ObjMesh *mesh = model->mMeshes[i];

				if(mesh->mMaterial != -1)
				{
					ObjMaterial *material = model->mMaterials[mesh->mMaterial];

					bindTexture(material->colorMapFilename);

					/*if(material->mipmapping)
					{
						TextureManager::Instance()->SetMipMapsTextures(material->texturID,material->mipmap1,material->mipmap2,material->mipmap3);

					}else
					{
						TextureManager::Instance()->SetTexture(material->texturID,GU_NEAREST,GU_NEAREST);
					}
				}

				if (mesh->mMaterial != -1 && model->mMaterials[mesh->mMaterial]->lightmapping)
				{
					//ObjMaterial *material = model->mMaterials[mesh->mMaterial];
					//TextureManager::Instance()->SetTexture(material->lightMapID);
				}

				//if(mesh->triangles)
					sceGumDrawArray(GU_TRIANGLES,GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,mesh->vertexCount,0,mesh->meshVertices);
				//else
				//	sceGumDrawArray(GU_TRIANGLE_STRIP,GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_3D,mesh->indicesCount,0,mesh->meshVertices);
			}

			sceGuDisable(GU_TEXTURE_2D);

			sceGumPopMatrix();
		}*/
	}
}
