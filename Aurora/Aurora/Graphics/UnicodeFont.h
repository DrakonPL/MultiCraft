#ifndef UNICODEFONT_H
#define UNICODEFONT_H

#include "stb/fontstash.h"
#include "stb/fontstash_renderer.h"

namespace Aurora
{
	namespace Graphics
	{
		class UnicodeFont
		{
		private:

			sth_stash* _stash;
			fontstash_renderer* _stashRenderer;

		public:

			UnicodeFont(int width,int height);
			~UnicodeFont();

			int LoadFont(const char* fontFileName);
			int LoadFontFromMemory(unsigned char* buffer);

			void DrawText(int idx, float size, float x, float y, const char* text, float* dx);

			void BeginText();
			void EndText();
		};
	}
}


#endif
