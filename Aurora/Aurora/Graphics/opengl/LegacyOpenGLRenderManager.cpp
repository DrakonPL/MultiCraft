#include <Aurora/Graphics/opengl/LegacyOpenGLRenderManager.h>
#include <Aurora/Graphics/TextureManager.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



namespace Aurora
{
	namespace Graphics
	{
		LegacyOpengGLRenderManager::LegacyOpengGLRenderManager()
		{
			// default values
			_width = 640;
			_height = 480;
			_pov = 53;
			_vSync = true;
			_fullScreen = false;
			_zOtrhoMin = -10;
			_zOtrhoMax = 10;
			_zMin = 0.01f;
			_zMax = 256.0f;

			SetVievport(0,0,_width,_height);
		}

		void LegacyOpengGLRenderManager::Init()
		{
			// init glew
			GLenum err = glewInit();
			if (GLEW_OK != err)
			{
				fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			}

			// set opengl values
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glClearDepth(1.0f);
			glClearStencil(0);  
			glDepthFunc(GL_LEQUAL);
			glClearColor(0.7f, 0.9f, 1.0f, 1.0f);
			glDisable(GL_TEXTURE_2D);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glEnable( GL_CULL_FACE );
			glCullFace( GL_BACK );

			SetFrontFace( CCW );

			_currentTexture = -1;
		}

		void LegacyOpengGLRenderManager::SetOrtho()
		{
			glViewport(_viewportX, _viewportY, _viewportWidth, _viewportHeight);

			glMatrixMode(GL_PROJECTION);	
			glLoadIdentity();

			glOrtho(0, _width, _height, 0, _zOtrhoMin, _zOtrhoMax);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}

		void LegacyOpengGLRenderManager::SetOrtho(float left,float right,float bottom,float top,float zmin,float zmax)
		{
			glViewport(_viewportX, _viewportY, _viewportWidth, _viewportHeight);

			glMatrixMode(GL_PROJECTION);	
			glLoadIdentity();

			glOrtho(left, right, bottom, top, _zOtrhoMin, _zOtrhoMax);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}

		void LegacyOpengGLRenderManager::SetPerspective()
		{
			glViewport(_viewportX, _viewportY, _viewportWidth, _viewportHeight);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			gluPerspective(_pov,(GLfloat)_width/(GLfloat)_height,_zMin,_zMax);

			glMatrixMode(GL_MODELVIEW);	
			glLoadIdentity();
		}

		void LegacyOpengGLRenderManager::SetPerspective(float pov,float aspect,float zmin,float zmax)
		{
			glViewport(_viewportX, _viewportY, _viewportWidth, _viewportHeight);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			gluPerspective(pov,aspect,zmin,zmax);

			glMatrixMode(GL_MODELVIEW);	
			glLoadIdentity();
		}

		void LegacyOpengGLRenderManager::bindTexture(Image* image)
		{
			if(image->_id != _currentTexture)
			{
				glBindTexture(GL_TEXTURE_2D, image->_id);
				_currentTexture = image->_id;
			}			
		}

		void LegacyOpengGLRenderManager::bindTexture(std::string filename)
		{
			unsigned int id = TextureManager::Instance()->getImage(filename)->_id;

			if(id != _currentTexture)
			{
				glBindTexture(GL_TEXTURE_2D, id);
				_currentTexture = id;
			}
		}

		void LegacyOpengGLRenderManager::ExtractFrustumPlanes(Frustum *frustum)
		{
			float projectionMatrix[16];	//projection matrix
			float viewMatrix[16];		//model view matrix
			float mvpMatrix[16];	//model-view-projection matrix

			glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
			glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);

			mvpMatrix[ 0] = viewMatrix[ 0] * projectionMatrix[ 0] + viewMatrix[ 1] * projectionMatrix[ 4] + viewMatrix[ 2] * projectionMatrix[ 8] + viewMatrix[ 3] * projectionMatrix[12];
			mvpMatrix[ 1] = viewMatrix[ 0] * projectionMatrix[ 1] + viewMatrix[ 1] * projectionMatrix[ 5] + viewMatrix[ 2] * projectionMatrix[ 9] + viewMatrix[ 3] * projectionMatrix[13];
			mvpMatrix[ 2] = viewMatrix[ 0] * projectionMatrix[ 2] + viewMatrix[ 1] * projectionMatrix[ 6] + viewMatrix[ 2] * projectionMatrix[10] + viewMatrix[ 3] * projectionMatrix[14];
			mvpMatrix[ 3] = viewMatrix[ 0] * projectionMatrix[ 3] + viewMatrix[ 1] * projectionMatrix[ 7] + viewMatrix[ 2] * projectionMatrix[11] + viewMatrix[ 3] * projectionMatrix[15];

			mvpMatrix[ 4] = viewMatrix[ 4] * projectionMatrix[ 0] + viewMatrix[ 5] * projectionMatrix[ 4] + viewMatrix[ 6] * projectionMatrix[ 8] + viewMatrix[ 7] * projectionMatrix[12];
			mvpMatrix[ 5] = viewMatrix[ 4] * projectionMatrix[ 1] + viewMatrix[ 5] * projectionMatrix[ 5] + viewMatrix[ 6] * projectionMatrix[ 9] + viewMatrix[ 7] * projectionMatrix[13];
			mvpMatrix[ 6] = viewMatrix[ 4] * projectionMatrix[ 2] + viewMatrix[ 5] * projectionMatrix[ 6] + viewMatrix[ 6] * projectionMatrix[10] + viewMatrix[ 7] * projectionMatrix[14];
			mvpMatrix[ 7] = viewMatrix[ 4] * projectionMatrix[ 3] + viewMatrix[ 5] * projectionMatrix[ 7] + viewMatrix[ 6] * projectionMatrix[11] + viewMatrix[ 7] * projectionMatrix[15];

			mvpMatrix[ 8] = viewMatrix[ 8] * projectionMatrix[ 0] + viewMatrix[ 9] * projectionMatrix[ 4] + viewMatrix[10] * projectionMatrix[ 8] + viewMatrix[11] * projectionMatrix[12];
			mvpMatrix[ 9] = viewMatrix[ 8] * projectionMatrix[ 1] + viewMatrix[ 9] * projectionMatrix[ 5] + viewMatrix[10] * projectionMatrix[ 9] + viewMatrix[11] * projectionMatrix[13];
			mvpMatrix[10] = viewMatrix[ 8] * projectionMatrix[ 2] + viewMatrix[ 9] * projectionMatrix[ 6] + viewMatrix[10] * projectionMatrix[10] + viewMatrix[11] * projectionMatrix[14];
			mvpMatrix[11] = viewMatrix[ 8] * projectionMatrix[ 3] + viewMatrix[ 9] * projectionMatrix[ 7] + viewMatrix[10] * projectionMatrix[11] + viewMatrix[11] * projectionMatrix[15];

			mvpMatrix[12] = viewMatrix[12] * projectionMatrix[ 0] + viewMatrix[13] * projectionMatrix[ 4] + viewMatrix[14] * projectionMatrix[ 8] + viewMatrix[15] * projectionMatrix[12];
			mvpMatrix[13] = viewMatrix[12] * projectionMatrix[ 1] + viewMatrix[13] * projectionMatrix[ 5] + viewMatrix[14] * projectionMatrix[ 9] + viewMatrix[15] * projectionMatrix[13];
			mvpMatrix[14] = viewMatrix[12] * projectionMatrix[ 2] + viewMatrix[13] * projectionMatrix[ 6] + viewMatrix[14] * projectionMatrix[10] + viewMatrix[15] * projectionMatrix[14];
			mvpMatrix[15] = viewMatrix[12] * projectionMatrix[ 3] + viewMatrix[13] * projectionMatrix[ 7] + viewMatrix[14] * projectionMatrix[11] + viewMatrix[15] * projectionMatrix[15];

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

		void LegacyOpengGLRenderManager::ExtractFrustumPlanes(Frustum *frustum,float mvpMatrix[16])
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

		void LegacyOpengGLRenderManager::UpdateCurrentCamera()
		{
			if(_currentCam != 0)
			{
				gluLookAt(_currentCam->m_vPosition.x + _currentCam->m_vOffset.x,_currentCam->m_vPosition.y + _currentCam->m_vOffset.y,_currentCam->m_vPosition.z + _currentCam->m_vOffset.z,
					_currentCam->m_vView.x,_currentCam->m_vView.y,_currentCam->m_vView.z,
					_currentCam->m_vUpVector.x,_currentCam->m_vUpVector.y,_currentCam->m_vUpVector.z);

				ExtractFrustumPlanes(_currentCam->camFrustum);
			}
		}

		void LegacyOpengGLRenderManager::ClearScreen()
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glLoadIdentity();
		}

		void LegacyOpengGLRenderManager::StartFrame()
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);
			glLoadIdentity();
		}

		void LegacyOpengGLRenderManager::EndFrame()
		{

		}

		void LegacyOpengGLRenderManager::CreateTexture(Image* image)
		{
			if (image != 0)
			{
				glGenTextures(1, &image->_id);
				glBindTexture(GL_TEXTURE_2D, image->_id);
				gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, image->_width, image->_height, GL_RGBA, GL_UNSIGNED_BYTE, image->_pixels);
				//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->_width, image->_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->_pixels);

				//clamp dong like 3d models
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_LINEAR
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_NEAREST

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//GL_LINEAR
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_NEAREST
			}
		}

		void LegacyOpengGLRenderManager::CreateTexture(unsigned char* pixels,int width,int height,unsigned int &texId)
		{
			if(pixels != 0)
			{
				glGenTextures(1, &texId);
				glBindTexture(GL_TEXTURE_2D, texId);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width,height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

				delete [] pixels;
				pixels = 0;
			}
		}

		void LegacyOpengGLRenderManager::CreateEmptyTexture( Image* image, ImageLocation location )
		{
			if (image != 0)
			{
				glGenTextures(1, &image->_id);
				glBindTexture(GL_TEXTURE_2D, image->_id);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->_width,image->_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}
		}

		void LegacyOpengGLRenderManager::UpdateTexture(Image* image)
		{
			if (image != 0)
			{
				glBindTexture(GL_TEXTURE_2D, image->_id);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  image->_width,image->_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->_pixels);
			}
		}

		void* LegacyOpengGLRenderManager::CreateVertexObject(VertexType vertexType,int size)
		{
			switch(vertexType)
			{
				case Simple :
					return new SimpleVertex[size];
				break;

				case Textured :
					return new TexturedVertex[size];
				break;

				case TextureColor :
					return new MultiVertex[size];
				break;
			}

			return 0;
		}

		void LegacyOpengGLRenderManager::SetColor(unsigned int col)
		{
			glColor4ub(col&0xff, (col>>8)&0xff, (col>>16)&0xff, (col>>24)&0xff);
		}

		void LegacyOpengGLRenderManager::SetBlending(bool state)
		{
			if(state)
			{
				glEnable(GL_BLEND);
			}else
			{
				glDisable(GL_BLEND);
			}
		}

		void LegacyOpengGLRenderManager::SetFrontFace(FrontFace face)
		{
			if(face == CW)
			{
				glFrontFace( GL_CW );
				_frontFace = CW;
			}else
			{
				glFrontFace( GL_CCW );
				_frontFace = CCW;
			}
		}

		void LegacyOpengGLRenderManager::DrawVertexObject(void* vertexObject,int vertexCound,bool textured,VertexType vertexType,VertexPrimitive vertecPrimitive)
		{
			if (vertexObject == 0)
				return;

			if (textured)
			{
				glEnable(GL_TEXTURE_2D);
				glEnable(GL_BLEND);
			}

			switch(vertexType)
			{
				case Simple:
				{
					SimpleVertex* vertices = (SimpleVertex*)vertexObject;

					glEnableClientState(GL_VERTEX_ARRAY);

					glVertexPointer(3, GL_FLOAT, GLsizei(sizeof(SimpleVertex)), &vertices[0].x);

					glDrawArrays(GL_TRIANGLES,0,vertexCound);

					glDisableClientState(GL_VERTEX_ARRAY);
				}
				break;

				case Textured:
				{
					TexturedVertex* vertices = (TexturedVertex*)vertexObject;

					glEnableClientState(GL_VERTEX_ARRAY);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);

					glVertexPointer(3, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &vertices[0].x);
					glTexCoordPointer(2, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &vertices[0].u);

					glDrawArrays(GL_TRIANGLES,0,vertexCound);

					glDisableClientState(GL_VERTEX_ARRAY);
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				}
				break;

				case TextureColor:				
				{

				}
				break;
			}

			if (textured)
			{
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_BLEND);
			}


			delete [] vertexObject;

		}

		void LegacyOpengGLRenderManager::_checkFramebufferStatus(RenderTexture* renderTexture)
		{
			// check FBO status
			GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
			switch(status)
			{
			case GL_FRAMEBUFFER_COMPLETE_EXT:
				renderTexture->_status = "Framebuffer complete.";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
				renderTexture->_status = "[ERROR] Framebuffer incomplete: Attachment is NOT complete.";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				renderTexture->_status = "[ERROR] Framebuffer incomplete: No image is attached to FBO.";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				renderTexture->_status = "[ERROR] Framebuffer incomplete: Attached images have different dimensions.";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				renderTexture->_status = "[ERROR] Framebuffer incomplete: Color attached images have different internal formats.";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
				renderTexture->_status = "[ERROR] Framebuffer incomplete: Draw buffer.";
				break;

			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
				renderTexture->_status = "[ERROR] Framebuffer incomplete: Read buffer.";
				break;

			case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
				renderTexture->_status = "[ERROR] Unsupported by FBO implementation.";
				break;

			default:
				renderTexture->_status = "[ERROR] Unknow error.";
				break;
			}
		}

		void LegacyOpengGLRenderManager::CreateRenderTexture(RenderTexture* renderTexture)
		{
			if (renderTexture == 0)
				return;

			//create frame buffer object
			glGenFramebuffersEXT(1, &renderTexture->_fboId);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, renderTexture->_fboId);

			// attach a texture to FBO color attachement point
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, renderTexture->GetImage()->_id, 0);

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

			//create render buffer object
			glGenRenderbuffersEXT(1, &renderTexture->_rboId);
			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderTexture->_rboId);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, renderTexture->GetImage()->_width, renderTexture->GetImage()->_height);
			//glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);


			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, renderTexture->_fboId);
			// attach a renderbuffer to depth attachment point
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, renderTexture->_rboId);

			_checkFramebufferStatus(renderTexture);

			//unbind farmebuffer
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}

		void LegacyOpengGLRenderManager::DestroyRenderTexture(RenderTexture* renderTexture)
		{
			if (renderTexture == 0)
				return;

			glDeleteFramebuffersEXT(1, &renderTexture->_fboId);
			glDeleteRenderbuffersEXT(1, &renderTexture->_rboId);
		}

		void LegacyOpengGLRenderManager::StartRenderToTexture(RenderTexture* renderTexture)
		{
			glViewport(0, 0, renderTexture->GetImage()->_width, renderTexture->GetImage()->_height);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			gluPerspective(_pov, renderTexture->GetImage()->_width/renderTexture->GetImage()->_height,_zMin,_zMax);

			glMatrixMode(GL_MODELVIEW);	
			glLoadIdentity();

			// set the rendering destination to FBO
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, renderTexture->_fboId);

			//now render all stuff you want
		}

		void LegacyOpengGLRenderManager::EndRenderToTexture(RenderTexture* renderTexture)
		{
			// unbind texture frame buffer 
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}

		void LegacyOpengGLRenderManager::RenderToScreen()
		{
			//in opegl case just set again perspective
			SetPerspective();
		}

		void LegacyOpengGLRenderManager::PushMatrix()
		{
			glPushMatrix();
		}

		void LegacyOpengGLRenderManager::PopMatrix()
		{
			glPopMatrix();
		}

		void LegacyOpengGLRenderManager::SetMatrix(float matrix[16])
		{
			glLoadMatrixf(matrix);
		}

		void LegacyOpengGLRenderManager::MultMatrix(float matrix[16])
		{
			glMultMatrixf(matrix);
		}

		void LegacyOpengGLRenderManager::TranslateMatrix(Vector3 pos)
		{
			glTranslatef(pos.x,pos.y,pos.z);
		}

		void LegacyOpengGLRenderManager::RotateMatrix(Vector3 rotation)
		{
			glRotatef(rotation.x,1.0f,0.0f,0.0f);
			glRotatef(rotation.y,0.0f,1.0f,0.0f);
			glRotatef(rotation.z,0.0f,0.0f,1.0f);
		}

		void LegacyOpengGLRenderManager::ScaleMatrix(Vector3 scale)
		{
			glScalef(scale.x,scale.y,scale.z);
		}

		//2D drawing
		void LegacyOpengGLRenderManager::DrawImage(Image* image)
		{
			bindTexture(image);

			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glColor4f(1.0f, 1.0f, 1.0f,1.0f);

			TexturedVertex *vertices = new TexturedVertex[4];
			vertices[0].u = 0.0f; vertices[0].v = 0.0f; vertices[0].x = 0; vertices[0].y = 0; vertices[0].z =0;
			vertices[1].u = 0.0f; vertices[1].v = 1.0f; vertices[1].x = 0; vertices[1].y = image->_height; vertices[1].z =0;
			vertices[2].u = 1.0f; vertices[2].v = 0.0f; vertices[2].x = image->_width; vertices[2].y = 0; vertices[2].z =0;
			vertices[3].u = 1.0f; vertices[3].v = 1.0f; vertices[3].x = image->_width; vertices[3].y = image->_height; vertices[3].z =0;

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glVertexPointer(3, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &vertices[0].x);
			glTexCoordPointer(2, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &vertices[0].u);

			glDrawArrays(GL_TRIANGLE_STRIP,0,4);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);

			delete [] vertices;
		}

		void LegacyOpengGLRenderManager::DrawImage(Image* image,int posx,int posy)
		{
			bindTexture(image);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glColor4f(1.0f, 1.0f, 1.0f,1.0f);

			TexturedVertex *vertices = new TexturedVertex[4];
			vertices[0].u = 0.0f;vertices[0].v = 0.0f;vertices[0].x = posx;vertices[0].y = posy;vertices[0].z = 0.0f;
			vertices[1].u = 0.0f;vertices[1].v = 1.0f;vertices[1].x = posx;vertices[1].y = posy+image->_height;vertices[1].z = 0.0f;
			vertices[2].u = 1.0f;vertices[2].v = 0.0f;vertices[2].x = posx+image->_width;vertices[2].y = posy;vertices[2].z = 0.0f;
			vertices[3].u = 1.0f;vertices[3].v = 1.0f;vertices[3].x = posx+image->_width;vertices[3].y = posy+image->_height;vertices[3].z = 0.0f;

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glVertexPointer(3, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &vertices[0].x);
			glTexCoordPointer(2, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &vertices[0].u);

			glDrawArrays(GL_TRIANGLE_STRIP,0,4);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);

			delete [] vertices;
		}

		void LegacyOpengGLRenderManager::DrawImage(Image* image,int posx,int posy,int width,int height)
		{
			bindTexture(image);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glColor4f(1.0f, 1.0f, 1.0f,1.0f);

			TexturedVertex *vertices = new TexturedVertex[4];
			vertices[0].u = 0.0f;vertices[0].v = 0.0f;vertices[0].x = posx;vertices[0].y = posy;vertices[0].z = 0.0f;
			vertices[1].u = 0.0f;vertices[1].v = 1.0f;vertices[1].x = posx;vertices[1].y = posy+height;vertices[1].z = 0.0f;
			vertices[2].u = 1.0f;vertices[2].v = 0.0f;vertices[2].x = posx+width;vertices[2].y = posy;vertices[2].z = 0.0f;
			vertices[3].u = 1.0f;vertices[3].v = 1.0f;vertices[3].x = posx+width;vertices[3].y = posy+height;vertices[3].z = 0.0f;

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glVertexPointer(3, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &vertices[0].x);
			glTexCoordPointer(2, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &vertices[0].u);

			glDrawArrays(GL_TRIANGLE_STRIP,0,4);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);

			delete [] vertices;
		}


		void LegacyOpengGLRenderManager::DrawSprite(Sprite* sprite)
		{
			glPushMatrix();

			glTranslatef(sprite->posX,sprite->posY,0.0f);

			bindTexture(sprite->imageName);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glColor3f(1,1,1);

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glVertexPointer(3, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &sprite->vertices[0].x);
			glTexCoordPointer(2, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &sprite->vertices[0].u);

			glDrawArrays(GL_TRIANGLE_STRIP,0,4);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
			glPopMatrix();
		}

		void LegacyOpengGLRenderManager::DrawSprite(Sprite* sprite,float posx,float posy)
		{
			glPushMatrix();

			glTranslatef(posx,posy,0.0f);

			bindTexture(sprite->imageName);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glColor3f(1,1,1);

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glVertexPointer(3, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &sprite->vertices[0].x);
			glTexCoordPointer(2, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &sprite->vertices[0].u);

			glDrawArrays(GL_TRIANGLE_STRIP,0,4);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
			glPopMatrix();
		}

		
		/*void LegacyOpengGLRenderManager::drawImage(Image* image)
		{
			bindTexture(image);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glColor4f(1.0f, 1.0f, 1.0f,1.0f);

			TexturedVertex *vertices = new TexturedVertex[4];
			vertices[0].u = 0.0f;vertices[0].v = 0.0f;vertices[0].x = 0.0f;vertices[0].y = 0.0f;vertices[0].z = 0.0f;
			vertices[1].u = 0.0f;vertices[1].v = 1.0f;vertices[1].x = 0.0f;vertices[1].y = image->_height;vertices[1].z = 0.0f;
			vertices[2].u = 1.0f;vertices[2].v = 0.0f;vertices[2].x = image->_width;vertices[2].y = 0;vertices[2].z = 0.0f;
			vertices[3].u = 1.0f;vertices[3].v = 1.0f;vertices[3].x = image->_width = image->_height;vertices[3].z = 0.0f;

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glVertexPointer(3, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &vertices[0].x);
			glTexCoordPointer(2, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &vertices[0].u);

			glDrawArrays(GL_TRIANGLE_STRIP,0,4);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);

			delete [] vertices;
		}

		void LegacyOpengGLRenderManager::drawImage(Image* image,int posx,int posy)
		{
			bindTexture(image);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glColor4f(1.0f, 1.0f, 1.0f,1.0f);

			TexturedVertex *vertices = new TexturedVertex[4];
			vertices[0].u = 0.0f;vertices[0].v = 0.0f;vertices[0].x = posx;vertices[0].y = posy;vertices[0].z = 0.0f;
			vertices[1].u = 0.0f;vertices[1].v = 1.0f;vertices[1].x = posx;vertices[1].y = posy+image->_height;vertices[1].z = 0.0f;
			vertices[2].u = 1.0f;vertices[2].v = 0.0f;vertices[2].x = posx+image->_width;vertices[2].y = posy;vertices[2].z = 0.0f;
			vertices[3].u = 1.0f;vertices[3].v = 1.0f;vertices[3].x = posx+image->_width;vertices[3].y = posy+image->_height;vertices[3].z = 0.0f;

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glVertexPointer(3, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &vertices[0].x);
			glTexCoordPointer(2, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &vertices[0].u);

			glDrawArrays(GL_TRIANGLE_STRIP,0,4);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);

			delete [] vertices;
		}

		void LegacyOpengGLRenderManager::drawImage(Image* image,int posx,int posy,int width,int height)
		{
			bindTexture(image);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glColor4f(1.0f, 1.0f, 1.0f,1.0f);

			TexturedVertex *vertices = new TexturedVertex[4];
			vertices[0].u = 0.0f;vertices[0].v = 0.0f;vertices[0].x = posx;vertices[0].y = posy;vertices[0].z = 0.0f;
			vertices[1].u = 0.0f;vertices[1].v = 1.0f;vertices[1].x = posx;vertices[1].y = posy+height;vertices[1].z = 0.0f;
			vertices[2].u = 1.0f;vertices[2].v = 0.0f;vertices[2].x = posx+width;vertices[2].y = posy;vertices[2].z = 0.0f;
			vertices[3].u = 1.0f;vertices[3].v = 1.0f;vertices[3].x = posx+width;vertices[3].y = posy+height;vertices[3].z = 0.0f;

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glVertexPointer(3, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &vertices[0].x);
			glTexCoordPointer(2, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &vertices[0].u);

			glDrawArrays(GL_TRIANGLE_STRIP,0,4);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);

			delete [] vertices;
		}

		void LegacyOpengGLRenderManager::drawSprite(Sprite* sprite)
		{
			glPushMatrix();

			glTranslatef(sprite->posX,sprite->posY,0.0f);

			bindTexture(sprite->imageName);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glColor3f(1,1,1);

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glVertexPointer(3, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &sprite->vertices[0].x);
			glTexCoordPointer(2, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &sprite->vertices[0].u);

			glDrawArrays(GL_TRIANGLE_STRIP,0,4);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
			glPopMatrix();
		}

		void LegacyOpengGLRenderManager::drawSprite(Sprite* sprite,float posx,float posy)
		{
			glPushMatrix();

			glTranslatef(posx,posy,0.0f);

			bindTexture(sprite->imageName);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glColor3f(1,1,1);

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glVertexPointer(3, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &sprite->vertices[0].x);
			glTexCoordPointer(2, GL_FLOAT, GLsizei(sizeof(TexturedVertex)), &sprite->vertices[0].u);

			glDrawArrays(GL_TRIANGLE_STRIP,0,4);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			glDisable(GL_TEXTURE_2D);
			glDisable(GL_BLEND);
			glPopMatrix();
		}

		void LegacyOpengGLRenderManager::drawSprite3D(Sprite3D* sprite)
		{
			glPushMatrix();

			glTranslatef(sprite->posX,sprite->posY,sprite->posZ);
			glRotatef(sprite->rotationX,1.0f,0.0f,0.0f);
			glRotatef(sprite->rotationY,0.0f,1.0f,0.0f);
			glRotatef(sprite->rotationZ,0.0f,0.0f,1.0f);
			glScalef(sprite->scaleX,sprite->scaleY,sprite->scaleZ);

			int posx = 0;
			int posy = 0;

			for (int y = sprite->height/2;y > -sprite->height/2;y--)
			{
				for (int x = -sprite->width/2;x < sprite->width/2;x++)
				{
					unsigned int colour = sprite->colours[posx + posy*sprite->height];
					if(colour != 0)
						drawCube(colour,Math::Vector3(x,y,0.0f),Math::Vector3(1.0f,1.0f,1.0f),Math::Vector3(0.0f,0.0f,0.0f));
					posx++;
				}
				posx = 0;
				posy++;
			}

			glPopMatrix();
		}

		//shapes
		/*void LegacyOpengGLRenderManager::drawCube(unsigned int color,Math::Vector3 position,Math::Vector3 scale,Math::Vector3 rotation)
		{
			glPushMatrix();

			glTranslatef(position.x,position.y,position.z);
			glRotatef(rotation.x,1.0f,0.0f,0.0f);
			glRotatef(rotation.y,0.0f,1.0f,0.0f);
			glRotatef(rotation.z,0.0f,0.0f,1.0f);
			

			glColor4ub(color&0xff, (color>>8)&0xff, (color>>16)&0xff, (color>>24)&0xff);

			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, cubeVert);

			glDrawArrays(GL_TRIANGLES,0,36);

			glDisableClientState(GL_VERTEX_ARRAY);
						
			glPopMatrix();
		}

		void LegacyOpengGLRenderManager::DrawCubeTextured(Image* texture,Math::Vector3 position,Math::Vector3 scale,Math::Vector3 rotation)
		{
			glPushMatrix();

			glTranslatef(position.x,position.y,position.z);
			glRotatef(rotation.x,1.0f,0.0f,0.0f);
			glRotatef(rotation.y,0.0f,1.0f,0.0f);
			glRotatef(rotation.z,0.0f,0.0f,1.0f);
			

			glColor4ub(255, 255, 255, 255);

			glBindTexture(GL_TEXTURE_2D, texture->_id);
			glEnable(GL_TEXTURE_2D);

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glTexCoordPointer(2,GL_FLOAT,0, cubeST);
			glVertexPointer(3, GL_FLOAT, 0, cubeVert);

			glDrawArrays(GL_TRIANGLES,0,36);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			glDisable(GL_TEXTURE_2D);

			glPopMatrix();
		}*/
	}
}
