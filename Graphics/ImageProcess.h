#ifndef __IMAGE_PROCESS_H__
#define __IMAGE_PROCESS_H__

#include "Defines.h"

namespace e
{
	struct Rect
	{
		int x0;
		int y0;
		int x1;
		int y1;

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

	class Bitmap;
	class ImageProcess
	{
	public:
		ImageProcess();
		virtual ~ImageProcess();
	public:

		Bitmap* GrayBitmap(const Bitmap* src);

		Bitmap* SmoothBitmap(const Bitmap* src, float sigma);

		Bitmap* Difference(const Bitmap* src);

		Bitmap* BinaryBitmap(const Bitmap* src, int threshold);

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

	private:

		Bitmap* Laplacian(const Bitmap* bitmap);
	};
}

#endif
