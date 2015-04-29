#include "stdafx.h"
#include "ImageProcess.h"
#include "Bitmap.h"

namespace e
{
	ImageProcess::ImageProcess()
	{

	}

	ImageProcess::~ImageProcess()
	{

	}

	Bitmap* ImageProcess::GrayBitmap(const Bitmap* bitmap)
	{
		assert(bitmap != 0 && bitmap->biBitCount >= 24);

		Bitmap* result = new Bitmap(bitmap->Width(), bitmap->Height(), 8);
		assert(result != 0);

		if (result != 0)
		{
			for (int y = 0; y < bitmap->Height(); y++)
			{
				uint8* p0 = bitmap->Get(0, y);
				uint8* p1 = result->Get(0, y);

				for (int x = 0; x < bitmap->Width(); x++)
				{
					uint8 b = *(p0 + 0);
					uint8 g = *(p0 + 1);
					uint8 r = *(p0 + 2);

					int value = (r * 29 + g * 150 + b * 77 + 128) >> 8;

					*p1 = min(value, 255);

					p0 += bitmap->PixelBytes();
					p1 += result->PixelBytes();
				}
			}
		}

		return result;
	}

	Bitmap* ImageProcess::SmoothBitmap(const Bitmap* bitmap, float sigma)
	{
		return 0;
	}

	Bitmap* ImageProcess::DrawRect(Bitmap* bitmap
		, int x0
		, int y0
		, int x1
		, int y1
		, int lineWidth /* = 1 */
		, RGBA color /* = 0xff0000ff */)
	{
		assert(bitmap != 0);
		assert(x0 >= 0 && x0 < bitmap->Width());
		assert(y0 >= 0 && y0 < bitmap->Height());
		assert(x1 >= 0 && x1 < bitmap->Width());
		assert(y1 >= 0 && y1 < bitmap->Height());
		assert(lineWidth >= 1);

		limit(x0, 0, bitmap->Width() - 1);
		limit(y0, 0, bitmap->Height() - 1);
		limit(x1, 0, bitmap->Width() - 1);
		limit(y1, 0, bitmap->Height() - 1);

		if (x0 > x1) swap(x0, x1);
		if (y0 > y1) swap(y0, y1);

		uint8 b = BC(color);
		uint8 g = GC(color);
		uint8 r = RC(color);

		//ÍùÄÚ²¿»­
		for (int i = 0; i < lineWidth; i++)
		{
			uint8* p0 = bitmap->Get(x0, min(y0 + i, y1));
			uint8* p1 = bitmap->Get(x0, max(y1 - i, y0));

			for (int x = x0; x <= x1; x++)
			{
				*(p0 + 0) = b;
				*(p0 + 1) = g;
				*(p0 + 2) = r;

				*(p1 + 0) = b;
				*(p1 + 1) = g;
				*(p1 + 2) = r;

				p0 += bitmap->PixelBytes();
				p1 += bitmap->PixelBytes();
			}

			p0 = bitmap->Get(min(x0 + i, x1), y0);
			p1 = bitmap->Get(max(x1 - i, x0), y0);

			for (int y = y0; y <= y1; y++)
			{
				*(p0 + 0) = b;
				*(p0 + 1) = g;
				*(p0 + 2) = r;

				*(p1 + 0) = b;
				*(p1 + 1) = g;
				*(p1 + 2) = r;

				p0 += bitmap->WidthBytes();
				p1 += bitmap->WidthBytes();
			}
		}

		return bitmap;
	}

	Bitmap* ImageProcess::DrawRect(Bitmap* bitmap, const Rect* rect, int lineWidth, RGBA color)
	{
		assert(bitmap && rect);
		return DrawRect(bitmap, rect->L(), rect->T(), rect->R(), rect->B(), lineWidth, color);
	}
}


