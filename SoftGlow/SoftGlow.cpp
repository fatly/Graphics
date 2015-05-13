#include "stdafx.h"
#include "SoftGlow.h"
#include "Gaussion.h"

#define SIGMOIDAL_BASE   2
#define SIGMOIDAL_RANGE 20
#define INT_MULT(a,b,t)  ((t) = (a) * (b) + 0x80, ((((t) >> 8) + (t)) >> 8))


inline int gimp_rgb_to_l_int(int red, int green, int blue)
{
	int minValue, maxValue;

	if (red > green)
	{
		maxValue = max(red, blue);
		minValue = min(green, blue);
	}
	else
	{
		maxValue = max(green, blue);
		minValue = min(red, blue);
	}
	//return (red * 29 + green * 150 + blue * 77 + 128) >> 8;
	return ROUND((maxValue + minValue) / 2.0);
}

inline double CalcSharpAndBright(uint8 x, float sharpness, float brightness)
{
	double val = x / 255.0;
	val = 255.0 / (1 + exp(-(SIGMOIDAL_BASE + (sharpness * SIGMOIDAL_RANGE)) * (val - 0.5)));
	val = val * brightness;
	return clamp(val, 0, 255);
}

namespace e
{
	SoftGlow::SoftGlow()
	{

	}

	SoftGlow::~SoftGlow()
	{

	}

	Bitmap* SoftGlow::Process1(const Bitmap* src, int radius, float sharpness, float brightness)
	{
		assert(src && src->IsValid());

		int width = src->Width();
		int height = src->Height();

		Bitmap* tmp = new Bitmap(width, height, 8);
		assert(tmp);

		for (int y = 0; y < height; y++)
		{
			uint8* p0 = src->Get(0, y);
			uint8* p1 = tmp->Get(0, y);

			for (int x = 0; x < width; x++)
			{
				uint8 b = *(p0 + 0);
				uint8 g = *(p0 + 1);
				uint8 r = *(p0 + 2);

				*p1 = (uint8)gimp_rgb_to_l_int(r, g, b);

				*p1 = (uint8)CalcSharpAndBright(*p1, sharpness, brightness);

				p0 += src->PixelBytes();
				p1 += tmp->PixelBytes();
			}
		}

		Gaussion(tmp, radius);

		tmp->Save("f:\\tmp0.bmp");

		Bitmap* dst = new Bitmap(src->Width(), src->Height(), src->biBitCount);
		assert(dst);
		int temp;
		for (int y = 0; y < height; y++)
		{
			uint8* p0 = src->Get(0, y);
			uint8* p1 = tmp->Get(0, y);
			uint8* p2 = dst->Get(0, y);

			for (int x = 0; x < width; x++)
			{
				*(p2 + 0) = 255 - INT_MULT((255 - *(p0 + 0)), (255 - *p1), temp);
				*(p2 + 1) = 255 - INT_MULT((255 - *(p0 + 1)), (255 - *p1), temp);
				*(p2 + 2) = 255 - INT_MULT((255 - *(p0 + 2)), (255 - *p1), temp);

				p0 += src->PixelBytes();
				p1 += tmp->PixelBytes();
				p2 += dst->PixelBytes();
			}
		}

		delete tmp;

		return dst;
	}

	Bitmap* SoftGlow::Process2(const Bitmap* src, int radius, float sharpness, float brightness)
	{
		assert(src && src->IsValid());

		Bitmap* dst = new Bitmap(src->Width(), src->Height(), src->biBitCount, 0, false);
		assert(dst);

		for (int y = 0; y < src->Height(); y++)
		{
			uint8* p0 = src->Get(0, y);
			uint8* p1 = dst->Get(0, y);

			for (int x = 0; x < src->Width(); x++)
			{
				*(p1 + 0) = (uint8)CalcSharpAndBright(*(p0 + 0), sharpness, brightness);
				*(p1 + 1) = (uint8)CalcSharpAndBright(*(p0 + 1), sharpness, brightness);
				*(p1 + 2) = (uint8)CalcSharpAndBright(*(p0 + 2), sharpness, brightness);

				p0 += src->PixelBytes();
				p1 += dst->PixelBytes();
			}
		}

		Gaussion(dst, radius);

		dst->Save("f:\\tmp0.bmp");

		int temp;
		for (int y = 0; y < src->Height(); y++)
		{
			uint8* p0 = src->Get(0, y);
			uint8* p1 = dst->Get(0, y);

			for (int x = 0; x < src->Width(); x++)
			{
				*(p1 + 0) = 255 - INT_MULT((255 - *(p0 + 0)), (255 - *(p1 + 0)), temp);
				*(p1 + 1) = 255 - INT_MULT((255 - *(p0 + 1)), (255 - *(p1 + 1)), temp);
				*(p1 + 2) = 255 - INT_MULT((255 - *(p0 + 2)), (255 - *(p1 + 2)), temp);

				p0 += src->PixelBytes();
				p1 += dst->PixelBytes();
			}
		}

		return dst;
	}
}

