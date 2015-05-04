#ifndef __CORE_CONVERT_H__
#define __CORE_CONVERT_H__

#include "Defines.h"
#include "Bitmap.h"

namespace e
{
	static void RGB2HSV(float r, float g, float b, float & h, float & s, float & v)
	{
		float _min, _max, delta;

		_min = min(r, min(g, b));
		_max = max(r, max(g, b));

		v = _max;

		delta = _max - _min;

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

	static void HSV(Bitmap* dst, const Bitmap* src, float hfactor, float sfactor, float vfactor)
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

				*(p1 + 0) = (uint8)(h * hfactor);
				*(p1 + 1) = (uint8)(s * sfactor);
				*(p1 + 2) = (uint8)(v * vfactor);
			}
		}
	}
}

#endif
