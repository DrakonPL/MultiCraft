#include <Aurora/Graphics/opengl/VertexBuffer.h>

#ifdef AURORA_PC
	#include <Aurora/Graphics/opengl/LegacyOpenGLRenderManager.h>
#elif defined AURORA_PS3
	#include <Aurora/Graphics/opengl/PSGLRenderManager.h>
#else
	#include <Aurora/Graphics/opengl/OpenGLES1RenderManager.h>
#endif

namespace Aurora
{
	namespace Graphics
	{
		VertexBuffer::VertexBuffer()
		{
			_vertexCount = 0;
			_indicesCount = 0;

			_vertices = 0;
			_indices = 0;

			_built = false;
		}

		void VertexBuffer::Generate(MultiVertex* vertices,unsigned short *indices,int vertexCount,int indicesCount,bool textured)
		{
			_vertices = vertices;
			_indices = indices;

			_vertexCount = vertexCount;
			_indicesCount = indicesCount;
			_textured = textured;
		}

		void VertexBuffer::Build()
		{
			if(_vertexCount > 0)
			{
				if (_vertices == 0 || _indices == 0)
				{
					Reset();
					return;
				}

				//vbo creation
				glGenBuffers(1, &_vboId);
				glBindBuffer(GL_ARRAY_BUFFER, _vboId);

				glBufferData(GL_ARRAY_BUFFER, sizeof(MultiVertex) * _vertexCount, NULL, GL_STATIC_DRAW);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(MultiVertex) * _vertexCount, _vertices); // Actually upload the data

				glBindBuffer(GL_ARRAY_BUFFER, 0);

				//index vbo creation
				glGenBuffers(1, &_indicesId); // Generate buffer
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesId);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indicesCount * sizeof(unsigned short), _indices, GL_STATIC_DRAW);
				
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

				if (_vertices != 0)
				{
					delete [] _vertices;
					_vertices = 0;
				}

				if (_indices != 0)
				{
					delete [] _indices;
					_indices = 0;
				}

				_built = true;
			}			
		}

		void VertexBuffer::Draw()
		{
			if(_vertexCount > 0 && _built == true)
			{
				if(_textured)
				{
					glEnable(GL_TEXTURE_2D);
					glEnable(GL_BLEND);
				}

				glBindBuffer(GL_ARRAY_BUFFER, _vboId);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesId);

				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glEnableClientState(GL_COLOR_ARRAY);
				glEnableClientState(GL_VERTEX_ARRAY);

				glVertexPointer(3, GL_FLOAT, sizeof(MultiVertex), BUFFER_OFFSET(0));
				glTexCoordPointer(2, GL_FLOAT, sizeof(MultiVertex), BUFFER_OFFSET(12));
				glColorPointer(4, GL_FLOAT, sizeof(MultiVertex), BUFFER_OFFSET(20));

				glDrawElements(GL_TRIANGLES,  GLsizei(_indicesCount), GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDisableClientState(GL_COLOR_ARRAY);
				glDisableClientState(GL_VERTEX_ARRAY);

				if(_textured)
				{
					glDisable(GL_TEXTURE_2D);
					glDisable(GL_BLEND);
				}

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
		}

		void VertexBuffer::Delete()
		{
			if(_built == true)
			{
				glDeleteBuffers(1, &_vboId);
				glDeleteBuffers(1, &_indicesId);

				_built = false;
			}
		}

		void VertexBuffer::Reset()
		{
			_vertexCount = 0;
			_indicesCount = 0;

			_vertices = 0;
			_indices = 0;

			_built = false;
		}
	}
}
