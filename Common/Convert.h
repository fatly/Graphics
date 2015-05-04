#ifndef __CORE_CONVERT_H__
#define __CORE_CONVERT_H__

#include "Defines.h"
#include "Bitmap.h"

namespace e
{
	static void RGB2HSV(float r, float g, float b, float & h, float & s, float & v)
	{
		float _min = min(r, min(g, b));
		float _max = max(r, max(g, b));

		v = _max;

		float delta = _max - _min;

		if (delta == 0) delta = 1.0f;

		if (_max != 0)
		{
			s = delta / _max;
		}
		else
		{
			s = 0;
		}

		if (_max == _min)
		{
			h = 0;
		}
		else if (_max == r && g >= b)
		{
			h = 60 * (g - b) / delta;
		}
		else if (_max == r && g < b)
		{
			h = 60 * (g - b) / delta + 360;
		}
		else if (_max == g)
		{
			h = 60 * (b - r) / delta + 120;
		}
		else if (_max == b)
		{
			h = 60 * (r - g) / delta + 240;
		}
	}

	static void HSVFilter8(Bitmap* dst
		, const Bitmap* src
		, float minH
		, float maxH
		, float minS
		, float maxS
		, float minV
		, float maxV)
	{
		float h, s, v;
		for (int y = 0; y < src->Height(); y++)
		{
			for (int x = 0; x < src->Width(); x++)
			{
				uint8* p0 = src->Get(x, y);
				uint8* p1 = dst->Get(x, y);

				float b = *(p0 + 0) / 255.0f;
				float g = *(p0 + 1) / 255.0f;
				float r = *(p0 + 2) / 255.0f;

				RGB2HSV(r, g, b, h, s, v);

				if (h >= minH && h <= maxH && s >= minS && s <= maxS && v >= minV && v <= maxV)
				{
					*p1 = 255;
				}
				else
				{
					*p1 = 0;
				}
			}
		}
	}

	static void HSVFilter24(Bitmap* dst
		, const Bitmap* src
		, float minH
		, float maxH
		, float minS
		, float maxS
		, float minV
		, float maxV)
	{
		float h, s, v;
		for (int y = 0; y < src->Height(); y++)
		{
			for (int x = 0; x < src->Width(); x++)
			{
				uint8* p0 = src->Get(x, y);
				uint8* p1 = dst->Get(x, y);

				float b = *(p0 + 0) / 255.0f;
				float g = *(p0 + 1) / 255.0f;
				float r = *(p0 + 2) / 255.0f;

				RGB2HSV(r, g, b, h, s, v);

				if (h >= minH && h <= maxH && s >= minS && s <= maxS && v >= minV && v <= maxV)
				{
					*(p1 + 0) = *(p0 + 0);
					*(p1 + 1) = *(p0 + 1);
					*(p1 + 2) = *(p0 + 2);
				}
				else
				{
					*(p1 + 0) = 0;
					*(p1 + 1) = 0;
					*(p1 + 2) = 0;
				}
			}
		}
	}
}

#endif
