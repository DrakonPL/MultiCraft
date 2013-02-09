#ifndef _OPENGL_FONTSTASH_CALLBACKS_H
#define _OPENGL_FONTSTASH_CALLBACKS_H

#include "fontstash.h"

struct	fontstash_renderer : public RenderCallbacks
{
	void updateTexture(sth_texture* texture, sth_glyph* glyph, int textureWidth, int textureHeight);
	void render(sth_texture* texture);
};



#endif
