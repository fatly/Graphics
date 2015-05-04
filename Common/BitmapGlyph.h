#ifndef __BITMAP_GLYPH_H__
#define __BITMAP_GLYPH_H__

#include "Bitmap.h"

namespace e
{
	class BitmapGlyph
	{
	public:
		BitmapGlyph(void);

		virtual ~BitmapGlyph(void);

		Bitmap* CreateBitmap(const TCHAR* text
			, const TCHAR* fontName
			, const int fontSize
			, const RGBA fontColor);

	private:
		Bitmap* CreateFontMask(const TCHAR* text, const TCHAR* fontName, int fontSize);
		Bitmap* CreateFontBitmap(const Bitmap* mask, const RGBA color);
		Bitmap* InvertAlpha(Bitmap* bitmap);
		Bitmap* AlphaBlend(Bitmap* dst, const Bitmap* src);
		Bitmap* Outline(Bitmap* bitmap);
		Bitmap* BlurMask(Bitmap* bitmap, int radius);
		Bitmap* ApplyMask(Bitmap* bitmap, const Bitmap* mask);
		Bitmap* CreateMaskBitmap(const Bitmap* mask);
		Bitmap* MergeMaskBitmap(const Bitmap* mask, const Bitmap* bitmap);
	};
}

#endif