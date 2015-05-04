#ifndef __CORE_MORPHOLOGY_H__
#define __CORE_MORPHOLOGY_H__

#include "Bitmap.h"

namespace e
{
	static void Erosion(Bitmap* dst, const Bitmap* src, bool vert)
	{
		int width = src->Width();
		int height = src->Height();
		//水平方向
		if (!vert)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 1; x < width-1; x++)
				{
					uint8* p0 = src->Get(x, y);
					uint8* p1 = dst->Get(x, y);

					if (*p0 == 0)
					{
						*p1 = 0;

						if (*(p0 - 1) == 255 || *(p0 + 1) == 255)
						{
							*p1 = 255;
						}
					}
					else
					{
						*p1 = 255;
					}
				}
			}
		}
		else
		{
			int lineBytes = src->WidthBytes();
			for (int y = 1; y < height - 1; y++)
			{
				for (int x = 0; x < width; x++)
				{
					uint8* p0 = src->Get(x, y);
					uint8* p1 = dst->Get(x, y);

					if (*p0 == 0)
					{
						*p1 = 0;

						if (*(p0 - lineBytes) == 255 || *(p0 + lineBytes) == 255)
						{
							*p1 = 255;
						}
					}
					else
					{
						*p1 = 255;
					}
				}
			}
		}
	}

	static void Dilation(Bitmap* dst, const Bitmap* src, bool vert)
	{
		int width = src->Width();
		int height = src->Height();
		//水平方向
		if (!vert)
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 1; x < width - 1; x++)
				{
					uint8* p0 = src->Get(x, y);
					uint8* p1 = dst->Get(x, y);

					if (*p0 == 255)
					{
						*p1 = 255;

						if (*(p0 - 1) == 0 || *(p0 + 1) == 0)
						{
							*p1 = 0;
						}
					}
					else
					{
						*p1 = 0;
					}
				}
			}
		}
		else
		{
			int lineBytes = src->WidthBytes();
			for (int y = 1; y < height - 1; y++)
			{
				for (int x = 0; x < width; x++)
				{
					uint8* p0 = src->Get(x, y);
					uint8* p1 = dst->Get(x, y);

					if (*p0 == 255)
					{
						*p1 = 255;

						if (*(p0 - lineBytes) == 0 || *(p0 + lineBytes) == 0)
						{
							*p1 = 0;
						}
					}
					else
					{
						*p1 = 0;
					}
				}
			}
		}
	}
}

#endif
