#ifndef __IMAGE_PROCESS_H__
#define __IMAGE_PROCESS_H__

#include "Defines.h"

namespace e
{
	struct Rect
	{
		int x;
		int y;
		int w;
		int h;

		Rect(void)
		{
			x = y = w = h = 0;
		}

		int L(void) const { return x; }
		int T(void) const { return y; }
		int R(void) const { return x + w - 1; }
		int B(void) const { return y + h - 1; }
		int W(void) const { return w; }
		int H(void) const { return h; }

		void Set(int x0, int y0, int x1, int y1)
		{
			if (x0 > x1) swap(x0, x1);
			if (y0 > y1) swap(y0, y1);

			x = x0;
			y = y0;
			w = x1 - x0 + 1;
			h = y1 - y0 + 1;
		}
	};

	class Bitmap;
	class ImageProcess
	{
	public:
		ImageProcess();
		virtual ~ImageProcess();
	public:

		Bitmap* GrayBitmap(const Bitmap* bitmap);

		Bitmap* SmoothBitmap(const Bitmap* bitmap, float sigma);

		Bitmap* DrawRect(Bitmap* bitmap
			, int x0
			, int y0
			, int x1
			, int y1
			, int lineWidth = 1
			, RGBA color = 0xff0000ff);

		Bitmap* DrawRect(Bitmap* bitmap
			, const Rect* rect
			, int lineWidth
			, RGBA color);
	};
}

#endif
