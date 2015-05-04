#include "ImageProcess.h"
#include "BaseClass.h"
#include "Convolve.h"
#include "Region.h"
#include "Convert.h"
#include "Morphology.h"
#include "Misc.h"
#include "Bitmap.h"
#include <math.h>

namespace e
{
	ImageProcess::ImageProcess()
	{

	}

	ImageProcess::~ImageProcess()
	{

	}

	Bitmap* ImageProcess::HSVFilter(const Bitmap* src
		, const float minH
		, const float maxH
		, const float minS
		, const float maxS
		, const float minV
		, const float maxV)
	{
		assert(src->IsValid() && src->biBitCount >= 24);

		Bitmap* dst = new Bitmap(src->Width(), src->Height(), 8, 0, false);
		assert(dst != 0);

		if (dst != 0)
		{
			HSVFilter8(dst, src, minH, maxH, minS, maxS, minV, maxV);
		}

		return dst;
	}

	Bitmap* ImageProcess::GrayBitmap(const Bitmap* src)
	{
		assert(src != 0 && src->biBitCount >= 24);

		Bitmap* dst = new Bitmap(src->Width(), src->Height(), 8);
		assert(dst != 0);

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

		return dst;
	}

	Bitmap* ImageProcess::SmoothBitmap(const Bitmap* src, float sigma, int mode)
	{
		assert(src->IsValid());
		assert(src->biBitCount == 8 || src->biBitCount >= 24);
		//根据sigma计算出高斯积卷内核
		const float width = 4.0f;
		sigma = max(sigma, 0.01f);
		int size = (int)ceil(sigma * width) + 1;

		float* mask = new float[size];
		assert(mask != 0);
		
		float sum = 0.0f;
		for (int i = 0; i < size; i++)
		{
			mask[i] = (float)exp(-0.5*square(i / sigma));
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

		if (mode == 0)
		{
			ConvolveEven(tmp, src, mask, size);
			ConvolveEven(dst, tmp, mask, size);
		}
		else
		{
			ConvolveOdd(tmp, src, mask, size);
			ConvolveOdd(dst, tmp, mask, size);
		}

		delete[] mask;
		delete tmp;

		return dst;
	}

	inline float diff(const Bitmap* src, int x0, int y0, int x1, int y1)
	{
		uint8* p0 = src->Get(x0, y0);
		uint8* p1 = src->Get(x1, y1);

		return sqrt((float)square(*(p0 + 0) - *(p1 + 0))
			+ square(*(p0 + 1) - *(p1 + 1))
			+ square(*(p0 + 2) - *(p1 + 2)));
	}

// 	Bitmap* ImageProcess::Difference(const Bitmap* src)
// 	{
// 		assert(src->IsValid() && src->biBitCount == 24);
// 
// 		int w = src->Width();
// 		int h = src->Height();
// 
// 		Bitmap* dst = new Bitmap(w, h, 8);
// 		assert(dst);
// 
// 		for (int y = 0; y < h; y++)
// 		{
// 			uint8* p = dst->Get(0, y);
// 
// 			for (int x = 0; x < w; x++)
// 			{
// 				float sum = 0.0f;
// 
// 				if (x < w - 1)
// 				{
// 					sum += diff(src, x, y, x + 1, y);
// 				}
// 
// 				if (y < h - 1)
// 				{
// 					sum += diff(src, x, y, x, y + 1);
// 				}
// 
// 				if (x < w - 1 && y < h - 1)
// 				{
// 					sum += diff(src, x, y, x + 1, y + 1);
// 				}
// 
// 				if (x < w - 1 && y>0)
// 				{
// 					sum += diff(src, x, y, x + 1, y - 1);
// 				}
// 
// 				*p++ = sum > 160 ? 255 : 0;
// 			}
// 		}
// 
// 		return dst;
// 	}

	Bitmap* ImageProcess::BinaryBitmap(const Bitmap* src, int threshold)
	{
		assert(src->IsValid() && src->biBitCount == 8);

		int w = src->Width();
		int h = src->Height();

		Bitmap* dst = new Bitmap(w, h, 8);
		assert(dst != 0);
		
		for (int y = 0; y < h; y++)
		{
			uint8* p0 = src->Get(0, y);
			uint8* p1 = dst->Get(0, y);

			for (int x = 0; x < w; x++)
			{
				*p1 = (*p0 < threshold) ? 0 : 255;

				p0++;
				p1++;
			}
		}

		return dst;
	}

	Bitmap* ImageProcess::Laplacian(const Bitmap* src)
	{
		assert(src->IsValid() && src->biBitCount == 8);

		int w = src->Width();
		int h = src->Height();
		int lineBytes = src->WidthBytes();
		int bytesPerPixel = src->PixelBytes();

		Bitmap* dst = new Bitmap(w, h, 8, 0, false);
		assert(dst != 0);

		for (int y = 1; y < h - 1; y++)
		{
			uint8* p0 = src->Get(1, y);
			uint8* p1 = dst->Get(1, y);

			for (int x = 1; x < w - 1; x++)
			{
				uint8 d2x = *(p0 - bytesPerPixel) + *(p0 + bytesPerPixel) - 2 * *p0;
				uint8 d2y = *(p0 - lineBytes) + *(p0 + lineBytes) - 2 * *p0;

				*p1 = d2x + d2y;

				p0++;
				p1++;
			}
		}

		return dst;
	}

	Bitmap* ImageProcess::RobertBitmap(const Bitmap* src)
	{
		assert(src->IsValid() && src->biBitCount == 8);

		int w = src->Width();
		int h = src->Height();
		int lineBytes = src->WidthBytes();
		int bytesPerPixel = src->PixelBytes();

		Bitmap* dst = new Bitmap(w, h, 8, 0, false);
		assert(dst != 0);

		uint8 pixel[4] = { 0 };
		for (int y = 1; y < h; y++)
		{
			uint8* p0 = src->Get(1, y);
			uint8* p1 = dst->Get(1, y);

			for (int x = 0; x < w - 1; x++)
			{
				pixel[0] = *(p0 + 0);
				pixel[1] = *(p0 + 1);
				pixel[2] = *(p0 - lineBytes + 0);
				pixel[3] = *(p0 - lineBytes + 1);

				//pixel[0] = *(p0 + 0);
				//pixel[1] = *(p0 + lineBytes + 0);
				//pixel[2] = *(p0 + 1);
				//pixel[3] = *(p0 + lineBytes + 1);

				float value = sqrt((float)square(pixel[0] - pixel[3]) + square(pixel[1] - pixel[2]));

				*p1 = (byte)clamp(value, 0, 255);

				p0++;
				p1++;
			}
		}

		return dst;
	}

	Bitmap* ImageProcess::ErosionBitmap(const Bitmap* src, bool vert /* = false */)
	{
		assert(src->IsValid() && src->biBitCount == 8);

		Bitmap* dst = new Bitmap(src->Width(), src->Height(), 8, 0);
		assert(dst != 0);

		if (dst != 0)
		{
			Erosion(dst, src, vert);
		}

		return dst;
	}

	Bitmap* ImageProcess::DilationBitmap(const Bitmap* src, bool vert /* = false */)
	{
		assert(src->IsValid() && src->biBitCount == 8);

		Bitmap* dst = new Bitmap(src->Width(), src->Height(), 8, 0);
		assert(dst != 0);

		if (dst != 0)
		{
			Dilation(dst, src, vert);
		}

		return dst;
	}

	Region* ImageProcess::FindRegion(const Bitmap* src
		, int xthreshold
		, int ythreshold
		, float minRate
		, float maxRate)
	{
		assert(src->IsValid() && src->biBitCount == 8);

		Region* region = new Region;
		assert(region != 0);

		if (region != 0)
		{
			_FindRegion(region, src, xthreshold, ythreshold, minRate, maxRate);
		}

		return region;
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

	Bitmap* ImageProcess::DrawRect(Bitmap* bitmap
		, const Rect* rect
		, int lineWidth /* = 1  */
		, RGBA color /* = 0xff0000ff */)
	{
		assert(bitmap->IsValid());
		return DrawRect(bitmap, rect->x0, rect->y0, rect->x1, rect->y1, lineWidth, color);
	}
}


