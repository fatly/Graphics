#ifndef __CORE_BASECLASS_H__
#define __CORE_BASECLASS_H__

#include "Defines.h"

namespace e
{

#define MAX_REGION_COUNT	64

	class Rect
	{
	public:
		int x0;
		int y0;
		int x1;
		int y1;
	public:
		Rect(void)
		{
			x0 = y0 = x1 = y1 = 0;
		}

		int L(void) const { return x0; }
		int T(void) const { return y0; }
		int R(void) const { return x1; }
		int B(void) const { return y1; }
		int W(void) const { return x1 - x0; }
		int H(void) const { return y1 - y0; }

		void Set(int x0, int y0, int x1, int y1)
		{
			this->x0 = x0;
			this->y0 = y0;
			this->x1 = x1;
			this->y1 = y1;
		}

		void Normalize(void)
		{
			if (x0 > x1) swap(x0, x1);
			if (y0 > y1) swap(y0, y1);
		}
	};

	struct Point
	{
		int x;
		int y;
	};

	class Region
	{
	public:
		struct Item
		{
			int x0;
			int y0;
			int x1;
			int y1;
			int width;
			int height;
			float rate;
		};

		Item regions[MAX_REGION_COUNT];

		int count;
	};
}

#endif
