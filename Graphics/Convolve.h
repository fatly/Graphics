#ifndef __CORE_CONVOLVE_H__
#define __CORE_CONVOLVE_H__

#include "Bitmap.h"

namespace e
{
	static void ConvolveEven(Bitmap* dst, const Bitmap* src, float* mask, int size)
	{
		int bytesPerPixel = src->PixelBytes();
		int lineBytes = src->WidthBytes();

		float sum[3] = { 0 };

		for (int y = 0; y < src->Height(); y++)
		{
			uint8* p0 = src->Get(1, y);	// B
			uint8* p1 = p0 + 1;			// G
			uint8* p2 = p0 + 2;			// R

			for (int x = 1; x < src->Width()-1; x++)
			{
				sum[0] = *p0 * mask[0];
				sum[1] = *p1 * mask[0];
				sum[2] = *p2 * mask[0];

				for (int i = 1; i < size; i++)
				{
					sum[0] += (*(p0 - bytesPerPixel) + *(p0 + bytesPerPixel)) * mask[i];
					sum[1] += (*(p1 - bytesPerPixel) + *(p1 + bytesPerPixel)) * mask[i];
					sum[2] += (*(p2 - bytesPerPixel) + *(p2 + bytesPerPixel)) * mask[i];
				}

				uint8* p3 = dst->Get(y, x);

				*(p3 + 0) = (uint8)clamp(sum[0], 0, 255);
				*(p3 + 1) = (uint8)clamp(sum[1], 0, 255);
				*(p3 + 2) = (uint8)clamp(sum[2], 0, 255);

				p0 += bytesPerPixel;
				p1 += bytesPerPixel;
				p2 += bytesPerPixel;
			}
		}
	}

	static void ConvolveOdd(Bitmap* dst, const Bitmap* src, float* mask, int size)
	{
		int bytesPerPixel = src->PixelBytes();
		int lineBytes = src->WidthBytes();

		float sum[3] = { 0 };

		for (int y = 0; y < src->Height(); y++)
		{
			uint8* p0 = src->Get(1, y);	// B
			uint8* p1 = p0 + 1;			// G
			uint8* p2 = p0 + 2;			// R

			for (int x = 1; x < src->Width() - 1; x++)
			{
				sum[0] = *p0 * mask[0];
				sum[1] = *p1 * mask[0];
				sum[2] = *p2 * mask[0];

				for (int i = 1; i < size; i++)
				{
					sum[0] += (*(p0 - bytesPerPixel) - *(p0 + bytesPerPixel)) * mask[i];
					sum[1] += (*(p1 - bytesPerPixel) - *(p1 + bytesPerPixel)) * mask[i];
					sum[2] += (*(p2 - bytesPerPixel) - *(p2 + bytesPerPixel)) * mask[i];
				}

				uint8* p3 = dst->Get(y, x);

				*(p3 + 0) = (uint8)sum[0];
				*(p3 + 1) = (uint8)sum[1];
				*(p3 + 2) = (uint8)sum[2];
			}
		}
	}
}

#endif
