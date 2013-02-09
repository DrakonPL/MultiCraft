#include <Aurora/Graphics/UnicodeFont.h>

namespace Aurora
{
	namespace Graphics
	{
		UnicodeFont::UnicodeFont(int width,int height)
		{
			_stashRenderer = new fontstash_renderer();
			_stash = sth_create(width,height,_stashRenderer);
		}

		UnicodeFont::~UnicodeFont()
		{
			delete _stashRenderer;
			delete _stash;
		}

		int UnicodeFont::LoadFont(const char* fontFileName)
		{
			return sth_add_font(_stash,fontFileName);
		}

		int UnicodeFont::LoadFontFromMemory(unsigned char* buffer)
		{
			return sth_add_font_from_memory(_stash,buffer);
		}

		void UnicodeFont::DrawText(int idx, float size, float x, float y, const char* text, float* dx)
		{
			sth_draw_text( _stash, idx, size, x, y, text, dx);
		}
		
		void UnicodeFont::BeginText()
		{
			sth_begin_draw(_stash);
		}

		void UnicodeFont::EndText()
		{
			sth_end_draw(_stash);
		}
	}
}
