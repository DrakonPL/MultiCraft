#include <Aurora/Graphics/models/ModelIqm.h>

//test
#include <Aurora/Graphics/opengl/LegacyOpenGLRenderManager.h>
#include <Aurora/Graphics/TextureManager.h>

namespace Aurora
{
	namespace Graphics
	{
		ModelIqm::ModelIqm()
		{
			meshdata = NULL;
			animdata = NULL;
			inposition = NULL;
			innormal = NULL;
			intangent = NULL;
			intexcoord = NULL;

			inblendindex = NULL;
			inblendweight = NULL;

			outposition = NULL;
			outnormal = NULL;
			outtangent = NULL;
			outbitangent = NULL;

			nummeshes = 0;
			numtris = 0;
			numverts = 0;
			numjoints = 0;
			numframes = 0;
			numanims = 0;

			tris = NULL;
			adjacency = NULL;
			meshes = NULL;
			textures = NULL;
			joints = NULL;
			poses = NULL;
			anims = NULL;
			bounds = NULL;
		}

		bool ModelIqm::LoadModel(std::string fileName)
		{
			FILE *f = fopen(fileName.c_str(), "rb");
			if(!f) return false;

			unsigned char *buf = NULL;
			iqmheader hdr;

			//check version
			if(fread(&hdr, 1, sizeof(hdr), f) != sizeof(hdr) || memcmp(hdr.magic, IQM_MAGIC, sizeof(hdr.magic)))
				return false;

			//endian swap
			lilswap(&hdr.version, (sizeof(hdr) - sizeof(hdr.magic))/sizeof(unsigned int));
			if(hdr.version != IQM_VERSION)
				return false;

			// sanity check... don't load files bigger than 16 MB
			if(hdr.filesize > (16<<20)) 
				return false;

			//read whole file to buffer
			buf = new uchar[hdr.filesize];
			if(fread(buf + sizeof(hdr), 1, hdr.filesize - sizeof(hdr), f) != hdr.filesize - sizeof(hdr))
				return false;

			if(hdr.num_meshes > 0 && !_loadMeshes(fileName, hdr, buf))
				return false;

			if(hdr.num_anims > 0 && !_loadAnimations(fileName, hdr, buf))
				return false;

			fclose(f);

			return true;
		}

		bool ModelIqm::_loadMeshes(std::string filename,iqmheader hdr,unsigned char *buf)
		{
			if(meshdata) 
				return false;

			lilswap((uint *)&buf[hdr.ofs_vertexarrays], hdr.num_vertexarrays*sizeof(iqmvertexarray)/sizeof(uint));
			lilswap((uint *)&buf[hdr.ofs_triangles], hdr.num_triangles*sizeof(iqmtriangle)/sizeof(uint));
			lilswap((uint *)&buf[hdr.ofs_meshes], hdr.num_meshes*sizeof(iqmmesh)/sizeof(uint));
			lilswap((uint *)&buf[hdr.ofs_joints], hdr.num_joints*sizeof(iqmjoint)/sizeof(uint));
			
			if(hdr.ofs_adjacency) 
				lilswap((uint *)&buf[hdr.ofs_adjacency], hdr.num_triangles*sizeof(iqmtriangle)/sizeof(uint));

			meshdata = buf;
			nummeshes = hdr.num_meshes;
			numtris = hdr.num_triangles;
			numverts = hdr.num_vertexes;
			numjoints = hdr.num_joints;

			outposition = new float[3*numverts];
			outnormal = new float[3*numverts];
			outtangent = new float[3*numverts];
			outbitangent = new float[3*numverts];
			outframe = new Matrix3x4[hdr.num_joints];
			textures = new unsigned int[nummeshes];

			memset(textures, 0, nummeshes*sizeof(unsigned int));

			const char *str = hdr.ofs_text ? (char *)&buf[hdr.ofs_text] : "";
			iqmvertexarray *vas = (iqmvertexarray *)&buf[hdr.ofs_vertexarrays];

			for(int i = 0; i < (int)hdr.num_vertexarrays; i++)
			{
				iqmvertexarray &va = vas[i];
				switch(va.type)
				{
				case IQM_POSITION: if(va.format != IQM_FLOAT || va.size != 3) return false; inposition = (float *)&buf[va.offset]; lilswap(inposition, 3*hdr.num_vertexes); break;
				case IQM_NORMAL: if(va.format != IQM_FLOAT || va.size != 3) return false; innormal = (float *)&buf[va.offset]; lilswap(innormal, 3*hdr.num_vertexes); break;
				case IQM_TANGENT: if(va.format != IQM_FLOAT || va.size != 4) return false; intangent = (float *)&buf[va.offset]; lilswap(intangent, 4*hdr.num_vertexes); break;
				case IQM_TEXCOORD: if(va.format != IQM_FLOAT || va.size != 2) return false; intexcoord = (float *)&buf[va.offset]; lilswap(intexcoord, 2*hdr.num_vertexes); break;
				case IQM_BLENDINDEXES: if(va.format != IQM_UBYTE || va.size != 4) return false; inblendindex = (uchar *)&buf[va.offset]; break;
				case IQM_BLENDWEIGHTS: if(va.format != IQM_UBYTE || va.size != 4) return false; inblendweight = (uchar *)&buf[va.offset]; break;
				}
			}

			tris = (iqmtriangle *)&buf[hdr.ofs_triangles];
			meshes = (iqmmesh *)&buf[hdr.ofs_meshes];
			joints = (iqmjoint *)&buf[hdr.ofs_joints];
			
			if(hdr.ofs_adjacency)
				adjacency = (iqmtriangle *)&buf[hdr.ofs_adjacency];


			baseframe = new Matrix3x4[hdr.num_joints];
			inversebaseframe = new Matrix3x4[hdr.num_joints];
			for(int i = 0; i < (int)hdr.num_joints; i++)
			{
				iqmjoint &j = joints[i];
				baseframe[i] = Matrix3x4(Quat(j.rotate).normalize(), Vec3(j.translate), Vec3(j.scale));
				inversebaseframe[i].invert(baseframe[i]);
				if(j.parent >= 0) 
				{
					baseframe[i] = baseframe[j.parent] * baseframe[i];
					inversebaseframe[i] *= inversebaseframe[j.parent];
				}
			}

			//load textures
			for(int i = 0; i < (int)hdr.num_meshes; i++)
			{
				iqmmesh &m = meshes[i];
				//printf("%s: loaded mesh: %s\n", filename, &str[m.name]);

				std::string pathName(filename);
				size_t found = pathName.find_last_of("/");
				pathName = pathName.substr(0,found)+ "/" + &str[m.material];

				TextureManager::Instance()->loadImageFromFile(pathName);
				textures[i]  = TextureManager::Instance()->getImage(pathName)->_id;

				//if(textures[i])
					//printf("%s: loaded material: %s\n", filename, &str[m.material]);
			}

			return true;
		}

		bool ModelIqm::_loadAnimations(std::string filename,iqmheader hdr,unsigned char *buf)
		{
			if((int)hdr.num_poses != numjoints)
				return false;

			if(animdata)
			{
				delete[] animdata;
				delete[] frames;
				animdata = NULL;
				anims = NULL;
				frames = 0;
				numframes = 0;
				numanims = 0;
			}        

			lilswap((uint *)&buf[hdr.ofs_poses], hdr.num_poses*sizeof(iqmpose)/sizeof(uint));
			lilswap((uint *)&buf[hdr.ofs_anims], hdr.num_anims*sizeof(iqmanim)/sizeof(uint));
			lilswap((ushort *)&buf[hdr.ofs_frames], hdr.num_frames*hdr.num_framechannels);

			if(hdr.ofs_bounds)
				lilswap((uint *)&buf[hdr.ofs_bounds], hdr.num_frames*sizeof(iqmbounds)/sizeof(uint));

			animdata = buf;
			numanims = hdr.num_anims;
			numframes = hdr.num_frames;

			const char *str = hdr.ofs_text ? (char *)&buf[hdr.ofs_text] : "";

			anims = (iqmanim *)&buf[hdr.ofs_anims];
			poses = (iqmpose *)&buf[hdr.ofs_poses];
			frames = new Matrix3x4[hdr.num_frames * hdr.num_poses];

			ushort *framedata = (ushort *)&buf[hdr.ofs_frames];

			if(hdr.ofs_bounds)
				bounds = (iqmbounds *)&buf[hdr.ofs_bounds];

			for(int i = 0; i < (int)hdr.num_frames; i++)
			{
				for(int j = 0; j < (int)hdr.num_poses; j++)
				{
					iqmpose &p = poses[j];
					Quat rotate;
					Vec3 translate, scale;
					translate.x = p.channeloffset[0]; if(p.mask&0x01) translate.x += *framedata++ * p.channelscale[0];
					translate.y = p.channeloffset[1]; if(p.mask&0x02) translate.y += *framedata++ * p.channelscale[1];
					translate.z = p.channeloffset[2]; if(p.mask&0x04) translate.z += *framedata++ * p.channelscale[2];
					rotate.x = p.channeloffset[3]; if(p.mask&0x08) rotate.x += *framedata++ * p.channelscale[3];
					rotate.y = p.channeloffset[4]; if(p.mask&0x10) rotate.y += *framedata++ * p.channelscale[4];
					rotate.z = p.channeloffset[5]; if(p.mask&0x20) rotate.z += *framedata++ * p.channelscale[5];
					rotate.w = p.channeloffset[6]; if(p.mask&0x40) rotate.w += *framedata++ * p.channelscale[6];
					scale.x = p.channeloffset[7]; if(p.mask&0x80) scale.x += *framedata++ * p.channelscale[7];
					scale.y = p.channeloffset[8]; if(p.mask&0x100) scale.y += *framedata++ * p.channelscale[8];
					scale.z = p.channeloffset[9]; if(p.mask&0x200) scale.z += *framedata++ * p.channelscale[9];

					// Concatenate each pose with the inverse base pose to avoid doing this at animation time.
					// If the joint has a parent, then it needs to be pre-concatenated with its parent's base pose.
					// Thus it all negates at animation time like so: 
					//   (parentPose * parentInverseBasePose) * (parentBasePose * childPose * childInverseBasePose) =>
					//   parentPose * (parentInverseBasePose * parentBasePose) * childPose * childInverseBasePose =>
					//   parentPose * childPose * childInverseBasePose
					Matrix3x4 m(rotate.normalize(), translate, scale);
					if(p.parent >= 0) frames[i*hdr.num_poses + j] = baseframe[p.parent] * m * inversebaseframe[j];
					else frames[i*hdr.num_poses + j] = m * inversebaseframe[j];
				}
			}

			for(int i = 0; i < (int)hdr.num_anims; i++)
			{
				iqmanim &a = anims[i];
				//printf("%s: loaded anim: %s\n", filename, &str[a.name]);
			}

			return true;
		}

		void ModelIqm::Animate(float curframe)
		{
			if(!numframes) return;

			int frame1 = (int)floor(curframe),frame2 = frame1 + 1;
			float frameoffset = curframe - frame1;

			frame1 %= numframes;
			frame2 %= numframes;

			Matrix3x4 *mat1 = &frames[frame1 * numjoints],*mat2 = &frames[frame2 * numjoints];

			// Interpolate matrixes between the two closest frames and concatenate with parent matrix if necessary.
			// Concatenate the result with the inverse of the base pose.
			// You would normally do animation blending and inter-frame blending here in a 3D engine.
			for(int i = 0; i < numjoints; i++)
			{
				Matrix3x4 mat = mat1[i]*(1 - frameoffset) + mat2[i]*frameoffset;
				if(joints[i].parent >= 0) outframe[i] = outframe[joints[i].parent] * mat;
				else outframe[i] = mat;
			}

			// The actual vertex generation based on the matrixes follows...
			const Vec3 *srcpos = (const Vec3 *)inposition, *srcnorm = (const Vec3 *)innormal;
			const Vec4 *srctan = (const Vec4 *)intangent; 
			Vec3 *dstpos = (Vec3 *)outposition, *dstnorm = (Vec3 *)outnormal, *dsttan = (Vec3 *)outtangent, *dstbitan = (Vec3 *)outbitangent; 
			const uchar *index = inblendindex, *weight = inblendweight;

			for(int i = 0; i < numverts; i++)
			{
				// Blend matrixes for this vertex according to its blend weights. 
				// the first index/weight is always present, and the weights are
				// guaranteed to add up to 255. So if only the first weight is
				// presented, you could optimize this case by skipping any weight
				// multiplies and intermediate storage of a blended matrix. 
				// There are only at most 4 weights per vertex, and they are in 
				// sorted order from highest weight to lowest weight. Weights with 
				// 0 values, which are always at the end, are unused.
				Matrix3x4 mat = outframe[index[0]] * (weight[0]/255.0f);
				for(int j = 1; j < 4 && weight[j]; j++)
					mat += outframe[index[j]] * (weight[j]/255.0f);

				// Transform attributes by the blended matrix.
				// Position uses the full 3x4 transformation matrix.
				// Normals and tangents only use the 3x3 rotation part 
				// of the transformation matrix.
				*dstpos = mat.transform(*srcpos);

				// Note that if the matrix includes non-uniform scaling, normal vectors
				// must be transformed by the inverse-transpose of the matrix to have the
				// correct relative scale. Note that invert(mat) = adjoint(mat)/determinant(mat),
				// and since the absolute scale is not important for a vector that will later
				// be renormalized, the adjoint-transpose matrix will work fine, which can be
				// cheaply generated by 3 cross-products.
				//
				// If you don't need to use joint scaling in your models, you can simply use the
				// upper 3x3 part of the position matrix instead of the adjoint-transpose shown 
				// here.
				Matrix3x3 matnorm(mat.b.cross3(mat.c), mat.c.cross3(mat.a), mat.a.cross3(mat.b));

				*dstnorm = matnorm.transform(*srcnorm);
				// Note that input tangent data has 4 coordinates, 
				// so only transform the first 3 as the tangent vector.
				*dsttan = matnorm.transform(Vec3(*srctan));
				// Note that bitangent = cross(normal, tangent) * sign, 
				// where the sign is stored in the 4th coordinate of the input tangent data.
				*dstbitan = dstnorm->cross(*dsttan) * srctan->w;

				srcpos++;
				srcnorm++;
				srctan++;
				dstpos++;
				dstnorm++;
				dsttan++;
				dstbitan++;

				index += 4;
				weight += 4;
			}
		}

		void ModelIqm::Draw()
		{
			glPushMatrix();

			//glTranslatef(translate.x*scale, translate.y*scale, translate.z*scale);
			glRotatef(90, -1, 0, 0);
			glScalef(0.25f, 0.25f, 0.25f);

			glColor3f(1, 1, 1);
			glVertexPointer(3, GL_FLOAT, 0, numframes > 0 ? outposition : inposition);
			glNormalPointer(GL_FLOAT, 0, numframes > 0 ? outnormal : innormal);
			glTexCoordPointer(2, GL_FLOAT, 0, intexcoord);

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			glEnable(GL_TEXTURE_2D);

			for(int i = 0; i < nummeshes; i++)
			{
				iqmmesh &m = meshes[i];
				glBindTexture(GL_TEXTURE_2D, textures[i]);
				glDrawElements(GL_TRIANGLES, 3*m.num_triangles, GL_UNSIGNED_INT, &tris[m.first_triangle]);
			}

			glDisable(GL_TEXTURE_2D);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);

			glPopMatrix();
		}
		
	}
}

