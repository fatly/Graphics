#ifndef __IMAGE_PROCESS_H__
#define __IMAGE_PROCESS_H__

#include "Defines.h"
#include "Object.h"

namespace e
{
	class Rect;
	class Bitmap;
	class Region;
	class ImageProcess : public RefCountObj
	{
	public:
		ImageProcess();
		virtual ~ImageProcess();
	public:

		Bitmap* HSVFilter(const Bitmap* src
			, const float minH
			, const float maxH
			, const float minS
			, const float maxS
			, const float minV
			, const float maxV);

		Bitmap* GrayBitmap(const Bitmap* src);

		Bitmap* SmoothBitmap(const Bitmap* src, float sigma, int mode);

		Bitmap* BinaryBitmap(const Bitmap* src, int threshold);
		
		Bitmap* Laplacian(const Bitmap* src);//拉普拉斯

		Bitmap* RobertBitmap(const Bitmap* src);
		//腐蚀黑点
		Bitmap* ErosionBitmap(const Bitmap* src, bool vert = false);
		//膨胀黑点
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

		Bitmap* DrawRect(Bitmap* bitmap
			, const Rect* rect
			, int lineWidth = 1
			, RGBA color = 0xff0000ff);

	};
}

#endif
