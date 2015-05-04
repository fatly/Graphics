#ifndef __IMAGE_PROCESS_H__
#define __IMAGE_PROCESS_H__

#include "Defines.h"

namespace e
{
	class Rect;
	class Bitmap;
	class Region;
	class ImageProcess
	{
	public:
		ImageProcess();
		virtual ~ImageProcess();
	public:

		Bitmap* Convert2HSV(const Bitmap* src
			, float hfactor
			, float sfactor
			, float vfactor);

		Bitmap* GrayBitmap(const Bitmap* src);

		Bitmap* SmoothBitmap(const Bitmap* src, float sigma);

		Bitmap* BinaryBitmap(const Bitmap* src, int threshold);
		
		Bitmap* Laplacian(const Bitmap* src);//¿≠∆’¿≠Àπ

		Bitmap* RobertBitmap(const Bitmap* src);

		Bitmap* ErosionBitmap(const Bitmap* src, bool vert = false);

		Bitmap* DilationBitmap(const Bitmap* src, bool vert = false);

		Region* FindRegion(const Bitmap* src
			, int xthreshold
			, int ythreshold
			, float minRate
			, float maxRate);

		Bitmap* DrawRect(Bitmap* bitmap
			, int x0
			, int y0
			, int x1
			, int y1
			, int lineWidth = 1
			, RGBA color = 0xff0000ff);

	};
}

#endif
