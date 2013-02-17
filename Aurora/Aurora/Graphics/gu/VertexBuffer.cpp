#include <Aurora/Graphics/gu/VertexBuffer.h>
#include <Aurora/Graphics/gu/GuRenderManager.h>

namespace Aurora
{
	namespace Graphics
	{
		VertexBuffer::VertexBuffer()
		{
			_vertexCount = 0;
			_indicesCount = 0;
			_pspVertices = 0;

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
				
				_pspVertices = (CraftPSPVertex*)memalign(16,_indicesCount * sizeof(CraftPSPVertex));
				
				for(int i = 0; i < _indicesCount; i++)
				{
					_pspVertices[i].u = _vertices[_indices[i]].u;
					_pspVertices[i].v = _vertices[_indices[i]].v;
					_pspVertices[i].color = GU_COLOR( _vertices[_indices[i]].r,_vertices[_indices[i]].g,_vertices[_indices[i]].b,1.0f);
					_pspVertices[i].x = _vertices[_indices[i]].x;
					_pspVertices[i].y = _vertices[_indices[i]].y;
					_pspVertices[i].z = _vertices[_indices[i]].z;
				}
				
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
				
				//sceKernelDcacheWritebackInvalidateRange(pspVertices,_indicesCount * sizeof(pspVertices));
				//sceKernelDcacheWritebackAll();
				sceKernelDcacheWritebackInvalidateAll();
			}

			_built = true;
		}

		void VertexBuffer::Draw()
		{
			if(_vertexCount > 0 && _built == true)
			{
				if(_textured)
					sceGuEnable(GU_TEXTURE_2D);
				
				sceGumDrawArray(GU_TRIANGLES, GU_TEXTURE_32BITF |GU_COLOR_8888| GU_VERTEX_32BITF | GU_TRANSFORM_3D, _indicesCount, 0, _pspVertices);
				
				if(_textured)
					sceGuDisable(GU_TEXTURE_2D);
			}
		}

		void VertexBuffer::Delete()
		{
			if(_built == true)
			{
				if(_pspVertices != 0)
				{
					free(_pspVertices);
					_pspVertices = 0;
				}
				
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
