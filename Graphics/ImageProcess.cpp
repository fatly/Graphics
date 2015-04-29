#include "stdafx.h"
#include "ImageProcess.h"
#include "Convolve.h"
#include "Misc.h"
#include "Bitmap.h"

namespace e
{
	ImageProcess::ImageProcess()
	{

	}

	ImageProcess::~ImageProcess()
	{

	}

	Bitmap* ImageProcess::GrayBitmap(const Bitmap* src)
	{
		assert(src != 0 && src->biBitCount >= 24);

		Bitmap* dst = new Bitmap(src->Width(), src->Height(), 8);
		assert(dst != 0);

		if (dst != 0)
		{
			for (int y = 0; y < src->Height(); y++)
			{
				uint8* p0 = src->Get(0, y);
				uint8* p1 = dst->Get(0, y);

				for (int x = 0; x < src->Width(); x++)
				{
					uint8 b = *(p0 + 0);
					uint8 g = *(p0 + 1);
					uint8 r = *(p0 + 2);

					int value = (r * 29 + g * 150 + b * 77 + 128) >> 8;

					*p1 = min(value, 255);

					p0 += src->PixelBytes();
					p1 += dst->PixelBytes();
				}
			}
		}

		return dst;
	}

	Bitmap* ImageProcess::SmoothBitmap(const Bitmap* src, float sigma)
	{
		assert(src && src->biBitCount == 24);
		//根据sigma计算出积卷内核
		const float width = 4.0f;
		sigma = max(sigma, 0.01f);
		int size = (int)ceil(sigma * width) + 1;

		float* mask = new float[size];
		assert(mask != 0);
		
		float sum = 0.0f;
		for (int i = 0; i < size; i++)
		{
			mask[i] = exp(-0.5*square(i / sigma));
			sum += fabs(mask[i]);
		}
		sum -= fabs(mask[0]);
		//normalize
		sum = sum * 2 + fabs(mask[0]);
		for (int i = 0; i < size; i++)
		{
			mask[i] /= sum;
		}

		Bitmap* tmp = new Bitmap(src->Height(), src->Width(), src->biBitCount);
		Bitmap* dst = new Bitmap(src->Width(), src->Height(), src->biBitCount);
		assert(tmp && dst);

		ConvolveEven(tmp, src, mask, size);
		ConvolveEven(dst, tmp, mask, size);

		delete[] mask;
		delete tmp;

		return dst;
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

		//往内部收缩
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
		return DrawRect(bitmap, rect->x0, rect->y0, rect->x1, rect->y1, lineWidth, color);
	}
}


