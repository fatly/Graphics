#ifndef __CORE_CONVOLVE_H__
#define __CORE_CONVOLVE_H__

#include "Bitmap.h"

namespace e
{
	static void ConvolveEven(Bitmap* dst, const Bitmap* src, float* mask, int size)
	{
		int width = src->Width();
		int height = src->Height();
		int lineBytes = src->WidthBytes();
		int bytesPerPixel = src->PixelBytes();

		float sum[3] = { 0 };
		int radius = size - 1;

		if (src->biBitCount >= 24)
		{
			for (int y = 0; y < height; y++)
			{
				uint8* p0 = src->Get(0, y);	// B
				uint8* p1 = p0 + 1;			// G
				uint8* p2 = p0 + 2;			// R

				for (int x = 0; x < width; x++)
				{
					sum[0] = *p0 * mask[0];
					sum[1] = *p1 * mask[0];
					sum[2] = *p2 * mask[0];

					for (int i = 1; i <= radius; i++)
					{
						int loffset = (x - i > 0) ? i : 0;
						int roffset = (x + i < width - 1) ? i : 0;

						loffset *= bytesPerPixel;
						roffset *= bytesPerPixel;

						sum[0] += (*(p0 - loffset) + *(p0 + roffset)) * mask[i];
						sum[1] += (*(p1 - loffset) + *(p1 + roffset)) * mask[i];
						sum[2] += (*(p2 - loffset) + *(p2 + roffset)) * mask[i];
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
		else if (bytesPerPixel == 8)
		{
			for (int y = 0; y < height; y++)
			{
				uint8* p0 = src->Get(0, y);

				for (int x = 0; x < width; x++)
				{
					sum[0] = *p0 * mask[0];

					for (int i = 1; i <= radius; i++)
					{
						int loffset = (x - i > 0) ? i : 0;
						int roffset = (x + i < width - 1) ? i : 0;

						loffset *= bytesPerPixel;
						roffset *= bytesPerPixel;

						sum[0] += (*(p0 - loffset) + *(p0 + roffset)) * mask[i];
					}

					uint8* p1 = dst->Get(y, x);

					*p1 = (uint8)clamp(sum[0], 0, 255);

					p0 += bytesPerPixel;
				}
			}
		}
	}

	static void ConvolveOdd(Bitmap* dst, const Bitmap* src, float* mask, int size)
	{
		int width = src->Width();
		int height = src->Height();
		int lineBytes = src->WidthBytes();
		int bytesPerPixel = src->PixelBytes();

		float sum[3] = { 0 };
		int radius = size - 1;

		if (src->biBitCount >= 24)
		{
			for (int y = 0; y < height; y++)
			{
				uint8* p0 = src->Get(0, y);	// B
				uint8* p1 = p0 + 1;			// G
				uint8* p2 = p0 + 2;			// R

				for (int x = 0; x < width; x++)
				{
					sum[0] = *p0 * mask[0];
					sum[1] = *p1 * mask[0];
					sum[2] = *p2 * mask[0];

					for (int i = 1; i <= radius; i++)
					{
						int loffset = (x - i > 0) ? i : 0;
						int roffset = (x + i < width - 1) ? i : 0;

						loffset *= bytesPerPixel;
						roffset *= bytesPerPixel;

						sum[0] += (*(p0 - loffset) - *(p0 + roffset)) * mask[i];
						sum[1] += (*(p1 - loffset) - *(p1 + roffset)) * mask[i];
						sum[2] += (*(p2 - loffset) - *(p2 + roffset)) * mask[i];
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
		else if (src->biBitCount == 8)
		{
			for (int y = 0; y < height; y++)
			{
				uint8* p0 = src->Get(0, y);

				for (int x = 0; x < width; x++)
				{
					sum[0] = *p0 * mask[0];

					for (int i = 1; i <= radius; i++)
					{
						int loffset = (x - i > 0) ? i : 0;
						int roffset = (x + i < width - 1) ? i : 0;

						loffset *= bytesPerPixel;
						roffset *= bytesPerPixel;

						sum[0] += (*(p0 - loffset) - *(p0 + roffset)) * mask[i];
					}

					uint8* p1 = dst->Get(y, x);

					*p1 = (uint8)clamp(sum[0], 0, 255);

					p0 += bytesPerPixel;
				}
			}
		}
	}
}

#endif
