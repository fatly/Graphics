#include "BitmapGlyph.h"
#include <Windows.h>
#include <tchar.h>
#include <assert.h>

namespace e
{
	HFONT CreateFont(const TCHAR* fontName, int fontSize, bool bold)
	{
		if (fontSize < 1)
		{
			fontSize = 1;
		}

		LOGFONT lf;
		memset(&lf, 0, sizeof(lf));

		lf.lfHeight = -fontSize;
		lf.lfWidth = 0;
		lf.lfEscapement = 0;
		lf.lfOrientation = 0;

		if (bold)
		{
			lf.lfWeight = FW_BOLD;
		}
		else
		{
			lf.lfWeight = FW_NORMAL;
		}

		lf.lfCharSet = GB2312_CHARSET;
		lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf.lfQuality = CLEARTYPE_QUALITY;// ANTIALIASED_QUALITY;
		lf.lfPitchAndFamily = DEFAULT_PITCH;

		if (_tcslen(fontName) > 32)
		{
#ifdef UNICODE
			wcscpy_s(lf.lfFaceName, 32, L"SYSTEM");
#else
			strcpy_s(lf.lfFaceName, 32, "SYSTEM");
#endif		
		}
		else
		{
#ifdef UNICODE
			wcscpy_s(lf.lfFaceName, 32, fontName);
#else
			strcpy_s(lf.lfFaceName, 32, fontName);
#endif
		}

		return ::CreateFontIndirect(&lf);
	}

	BitmapGlyph::BitmapGlyph(void)
	{
	}

	BitmapGlyph::~BitmapGlyph(void)
	{
	}

	Bitmap* BitmapGlyph::CreateFontMask(const TCHAR* text, const TCHAR* fontName, int fontSize)
	{
		int len = _tcslen(text);
		int width = 256;
		int height = 256;

		Bitmap* bitmap = new Bitmap;
		if (bitmap == 0) return 0;
		
		HFONT hFont = CreateFont(fontName, fontSize * 3, false);
		if (hFont == INVALID_HANDLE_VALUE)
		{
			delete bitmap;
			return 0;
		}
		
		HDC hDC = ::CreateCompatibleDC(NULL);
		HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);
		SIZE sz;
		::GetTextExtentPoint32(hDC, text, len, &sz);
		width = sz.cx;
		height = sz.cy;

		PVOID bits = NULL;
		BITMAPINFOHEADER biHeader = { 0 };
		biHeader.biSize = sizeof(BITMAPINFOHEADER);
		biHeader.biWidth = width;
		biHeader.biHeight = -height;
		biHeader.biBitCount = 32;
		biHeader.biPlanes = 1;
		//CreateDIBSection创建的bits为BGRA格式,正向的，非文件格式般颠倒
		HBITMAP hBitmap = CreateDIBSection(NULL, (BITMAPINFO*)&biHeader, DIB_RGB_COLORS, &bits, NULL, NULL);
		assert(hBitmap != NULL);

		::SelectObject(hDC, hBitmap);
		RECT rect = { 0, 0, width, height };
		::FillRect(hDC, &rect, (HBRUSH)::GetStockObject(BLACK_BRUSH));
		::SetBkMode(hDC, TRANSPARENT);
		::SetTextColor(hDC, RGB(255, 255, 255));
		::TextOut(hDC, 0, 0, text, len);
		::SelectObject(hDC, hOldFont);

		bitmap->Alloc(width / 3, height / 3, 8);

		int bytesPerPixel = 4;
		int lineBytes = WIDTHBYTES(width * 32);

		width /= 3;
		height /= 3;

		for (int y = 0; y < height; y++)
		{
			uint8* p0 = bitmap->Get(0, y);
			uint8* p1 = (uint8*)bits + y * lineBytes * 3;
			uint8* p2 = p1 + lineBytes;
			uint8* p3 = p2 + lineBytes;

			for (int x = 0; x < width; x++)
			{
				int index = x * bytesPerPixel * 3;

				p0[x] = (p1[index] + p1[index + bytesPerPixel] + (p1[index + bytesPerPixel * 2] >> 1)
					+ p2[index] + p2[index + bytesPerPixel] + p2[index + bytesPerPixel * 2]
					+ (p3[index] >> 1) + p3[index + bytesPerPixel] + p3[index + bytesPerPixel * 2]) >> 3;
			}
		}

		::DeleteObject(hBitmap);
		::DeleteObject(hFont);
		::DeleteObject(hDC);

		return bitmap;
	}

	Bitmap* BitmapGlyph::CreateFontBitmap(const Bitmap* mask, const RGBA color)
	{
		assert(mask != 0 && mask->biBitCount == 8);
		Bitmap* bitmap = new Bitmap(mask->Width(), mask->Height(), 32);
		assert(bitmap != 0);

		if (bitmap != 0)
		{
			bitmap->SetColor32(color);

			bitmap = InvertAlpha(bitmap);

			bitmap = ApplyMask(bitmap, mask);
		}

		return bitmap;
	}

	Bitmap* BitmapGlyph::InvertAlpha(Bitmap* bitmap)
	{
		assert(bitmap != 0 && bitmap->biBitCount == 32);

		int lineBytes = bitmap->WidthBytes();

		for (int y = 0; y < bitmap->Height(); y++)
		{
			uint8* p = bitmap->bits + y * lineBytes;

			for (int x = 0; x < bitmap->Width(); x++)
			{
				*(p + 3) = 255 - *(p + 3);

				p += 4;
			}
		}

		return bitmap;
	}

	Bitmap* BitmapGlyph::AlphaBlend(Bitmap* dst, const Bitmap* src)
	{
		assert(dst != 0 && dst->biBitCount == 32);
		assert(src != 0 && src->biBitCount == 32);
		//assert(dst->Width() == src->Width());
		//assert(dst->Height() == src->Height());
		int w = min(src->Width(), dst->Width());
		int h = min(src->Height(), dst->Height());

		int bytesPerPixel = src->PixelBytes();
		int lineBytes = src->WidthBytes();

		for (int y = 0; y < h; y++)
		{
			uint8* p0 = src->bits + y * src->WidthBytes();
			uint8* p1 = dst->bits + y * dst->WidthBytes();

			for (int x = 0; x < w; x++)
			{
				//src
				uint8 b0 = *(p0 + 0);
				uint8 g0 = *(p0 + 1);
				uint8 r0 = *(p0 + 2);
				uint8 a0 = *(p0 + 3);
				//dst
				uint8 b1 = *(p1 + 0);
				uint8 g1 = *(p1 + 1);
				uint8 r1 = *(p1 + 2);
				uint8 a1 = *(p1 + 3);

//				dst = src * src.alpha + dst * (1.0 - src.alpha)
//	 			uint8 b = a0 * b0 / 255 + (255 - a0) * b1 / 255;
//	 			uint8 g = a0 * g0 / 255 + (255 - a0) * g1 / 255;
//	 			uint8 r = a0 * r0 / 255 + (255 - a0) * r1 / 255;
//	 			uint8 a = a0 * a0 / 255 + (255 - a0) * a1 / 255;

				//dst = src + dst * (1.0 - src.alpha)
				uint8 b = b0 + b1 * (255 - a0) / 255;
				uint8 g = g0 + g1 * (255 - a0) / 255;
				uint8 r = r0 + r1 * (255 - a0) / 255;
				uint8 a = a0 + a1 * (255 - a0) / 255;

				*(p1 + 0) = b;
				*(p1 + 1) = g;
				*(p1 + 2) = r;
				*(p1 + 3) = a;

				p0 += src->PixelBytes();
				p1 += dst->PixelBytes();
			}
		}

		return dst;
	}

	Bitmap* BitmapGlyph::ApplyMask(Bitmap* bitmap, const Bitmap* mask)
	{
		assert(mask != 0 && mask->biBitCount == 8);
		assert(bitmap != 0 && bitmap->biBitCount == 32);
		assert(mask->Width() == bitmap->Width());
		assert(mask->Height() == bitmap->Height());

		Bitmap temp(bitmap->Width(), bitmap->Height(), 32);

		int bytesPerPixel0 = temp.PixelBytes();
		int bytesPerPixel1 = mask->PixelBytes();
		int lineBytes0 = temp.WidthBytes();
		int lineBytes1 = mask->WidthBytes();

		//取反alpha
		for (int y = 0; y < temp.Height(); y++)
		{
			uint8* p0 = temp.bits + y * lineBytes0;
			uint8* p1 = mask->bits + y * lineBytes1;

			for (int x = 0; x < temp.Width(); x++)
			{
				*(p0 + 3) = 255 - *(p1);

				p0 += bytesPerPixel0;
				p1 += bytesPerPixel1;
			}
		}

		AlphaBlend(bitmap, &temp);

		for (int y = 0; y < bitmap->Height(); y++)
		{
			uint8* p = bitmap->bits + y * lineBytes0;

			for (int x = 0; x < bitmap->Width(); x++)
			{
				*(p + 3) = 255 - *(p + 3);

				if (*(p + 3) == 0)
				{
					uint32* t = (uint32*)p;
					*t = 0;
				}

				p += bytesPerPixel0;
			}
		}

		return bitmap;
	}

	Bitmap* BitmapGlyph::Outline(Bitmap* mask)
	{
		assert(mask->biBitCount == 8);

		Bitmap* temp = mask->Clone();
		assert(temp != 0);

		int lineBytes = mask->WidthBytes();

		for (int y = 1; y < mask->Height() - 1; y++)
		{
			uint8* p0 = mask->bits + y * lineBytes;
			uint8* p2 = temp->bits + y * lineBytes;
			uint8* p1 = p2 - lineBytes;
			uint8* p3 = p2 + lineBytes;

			for (int x = 1; x < mask->Width() - 1; x++)
			{
				uint8 s0 = abs(p3[x] - p3[x]);
				uint8 s1 = abs(p2[x - 1] - p2[x + 1]);
				uint8 s2 = abs(p3[x - 1] - p1[x + 1]);
				uint8 s3 = abs(p1[x - 1] - p3[x + 1]);
				int sum = (s0 + s1 + s2 + s3) >> 2;
				p0[x] = min(sum, 255);
			}
		}

		SAFE_DELETE(temp);

		return mask;
	}

	Bitmap* BitmapGlyph::BlurMask(Bitmap* mask, int radius)
	{
		assert(mask != 0 && mask->biBitCount == 8 && radius >= 0);

		Bitmap temp(mask->biWidth, mask->biHeight, mask->biBitCount);

		int w = mask->biWidth;

		for (int i = 0; i < radius; i++)
		{
			uint8* src = mask->bits;
			uint8* dst = temp.bits;

			for (uint y = 1; y < mask->biHeight - 1; y++)
			{
				for (uint x = 1; x < mask->biWidth - 1; x++)
				{
					int c = src[(y - 1)*w + (x - 1)] + src[(y - 1)*w + x] + (src[(y - 1)*w + (x + 1)] >> 1)
						+ src[y*w + (x - 1)] + src[y*w + x] + src[y*w + (x + 1)]
						+ (src[(y + 1)*w + (x - 1)] >> 1) + src[(y + 1)*w + x] + src[(y + 1)*w + (x + 1)];

					dst[y*w + x] = c >> 3;
				}
			}

			temp.Swap(*mask);
		}

		return mask;
	}

	Bitmap* BitmapGlyph::CreateMaskBitmap(const Bitmap* mask)
	{
		assert(mask != 0 && mask->biBitCount == 8);

		Bitmap* bitmap = new Bitmap(mask->Width(), mask->Height(), 32);
		assert(bitmap != 0);

		if (bitmap != 0)
		{
			bitmap = ApplyMask(bitmap, mask);
		}

		return bitmap;
	}

	Bitmap* BitmapGlyph::MergeMaskBitmap(const Bitmap* mask, const Bitmap* bitmap)
	{
		assert(mask != 0 && bitmap != 0);
		assert(mask->biBitCount == 32 && bitmap->biBitCount == 32);
		assert(mask->Width() == bitmap->Width());
		assert(mask->Height() == bitmap->Height());
		//create a bitmap
		Bitmap* result = bitmap->Clone();
		assert(result != 0);

		AlphaBlend(result, mask);

		int lineBytes = result->WidthBytes();

		uint8 alpha = 0;
		for (int y = 0; y < result->Height(); y++)
		{
			uint8* p0 = bitmap->bits + y * lineBytes;
			uint8* p1 = mask->bits + y * lineBytes;
			uint8* p2 = result->bits + y * lineBytes;

			for (int x = 0; x < result->Width(); x++)
			{
				int index = x * 4 + 3;
				uint8 a = 255 - p0[index];
				uint8 b = 255 - p1[index];

				__asm
				{
					MOV AL, a;
					MUL b;
					MOV CL, 0xff;
					DIV CL;
					MOV alpha, AL;
					MOV AL, 255;
					SUB AL, alpha;
					MOV alpha, AL;
				}

				p2[index] = alpha;
			}
		}

		return result;
	}

	Bitmap* BitmapGlyph::CreateBitmap(const TCHAR* text
		, const TCHAR* fontName
		, const int fontSize
		, const RGBA fontColor)
	{
		Bitmap *result0 = 0, *result1 = 0, *mask = 0, *font = 0, *temp = 0;

		do 
		{
			mask = CreateFontMask(text, fontName, fontSize);
			if (mask == 0) break;

			font = CreateFontBitmap(mask, fontColor);
			if (font == 0) break;

			Outline(mask);

			temp = CreateMaskBitmap(mask);
			if (temp == 0) break;

			result0 = MergeMaskBitmap(font, temp);
			if (result0 == 0) break;

			BlurMask(mask, 2);

			SAFE_DELETE(temp);
			temp = CreateMaskBitmap(mask);
			if (temp == 0) break;

			result1 = MergeMaskBitmap(result0, temp);

		} while (0);

		SAFE_DELETE(mask);
		SAFE_DELETE(font);
		SAFE_DELETE(temp);
		SAFE_DELETE(result0);
	
		return result1;
	}
}

