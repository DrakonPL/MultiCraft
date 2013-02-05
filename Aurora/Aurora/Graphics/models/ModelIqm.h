#ifndef MODELIQM_H
#define MODELIQM_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <math.h>

#define IQM_MAGIC "INTERQUAKEMODEL"
#define IQM_VERSION 2

#include <Aurora/Graphics/models/ModelIqmUtils.h>
#include <Aurora/Graphics/models/ModelIqmGeometry.h>

namespace Aurora
{
	namespace Graphics
	{	

		struct iqmheader
		{
			char magic[16];
			unsigned int version;
			unsigned int filesize;
			unsigned int flags;
			unsigned int num_text, ofs_text;
			unsigned int num_meshes, ofs_meshes;
			unsigned int num_vertexarrays, num_vertexes, ofs_vertexarrays;
			unsigned int num_triangles, ofs_triangles, ofs_adjacency;
			unsigned int num_joints, ofs_joints;
			unsigned int num_poses, ofs_poses;
			unsigned int num_anims, ofs_anims;
			unsigned int num_frames, num_framechannels, ofs_frames, ofs_bounds;
			unsigned int num_comment, ofs_comment;
			unsigned int num_extensions, ofs_extensions;
		};

		struct iqmmesh
		{
			unsigned int name;
			unsigned int material;
			unsigned int first_vertex, num_vertexes;
			unsigned int first_triangle, num_triangles;
		};

		enum
		{
			IQM_POSITION     = 0,
			IQM_TEXCOORD     = 1,
			IQM_NORMAL       = 2,
			IQM_TANGENT      = 3,
			IQM_BLENDINDEXES = 4,
			IQM_BLENDWEIGHTS = 5,
			IQM_COLOR        = 6,
			IQM_CUSTOM       = 0x10
		};

		enum
		{
			IQM_BYTE   = 0,
			IQM_UBYTE  = 1,
			IQM_SHORT  = 2,
			IQM_USHORT = 3,
			IQM_INT    = 4,
			IQM_UINT   = 5,
			IQM_HALF   = 6,
			IQM_FLOAT  = 7,
			IQM_DOUBLE = 8,
		};

		struct iqmtriangle
		{
			unsigned int vertex[3];
		};

		struct iqmjoint
		{
			unsigned int name;
			int parent;
			float translate[3], rotate[4], scale[3];
		};

		struct iqmpose
		{
			int parent;
			unsigned int mask;
			float channeloffset[10];
			float channelscale[10];
		};

		struct iqmanim
		{
			unsigned int name;
			unsigned int first_frame, num_frames;
			float framerate;
			unsigned int flags;
		};

		enum
		{
			IQM_LOOP = 1<<0
		};

		struct iqmvertexarray
		{
			unsigned int type;
			unsigned int flags;
			unsigned int format;
			unsigned int size;
			unsigned int offset;
		};

		struct iqmbounds
		{
			float bbmin[3], bbmax[3];
			float xyradius, radius;
		};

		class ModelIqm
		{
		private:

			//for test right now
			unsigned char *meshdata, *animdata;
			float *inposition, *innormal, *intangent, *intexcoord;
			unsigned char *inblendindex, *inblendweight;
			float *outposition, *outnormal, *outtangent, *outbitangent;
			int nummeshes, numtris, numverts, numjoints, numframes, numanims;
			iqmtriangle *tris, *adjacency;
			iqmmesh *meshes;
			unsigned int *textures;
			iqmjoint *joints;
			iqmpose *poses;
			iqmanim *anims;
			iqmbounds *bounds;
			Matrix3x4 *baseframe, *inversebaseframe, *outframe, *frames;

		private:

			bool _loadMeshes(std::string filename,iqmheader hdr,unsigned char *buf);
			bool _loadAnimations(std::string filename,iqmheader hdr,unsigned char *buf);

		public:

			ModelIqm();

			bool LoadModel(std::string fileName);

			void Animate(float curframe);

			void Draw();

		};
	}
}

#endif
