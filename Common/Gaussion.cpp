#include "Gaussion.h"
#include "Defines.h"
#include <math.h>
#include <memory.h>
#include <assert.h>

namespace e
{
	void find_constants(double n_p[]
		, double n_m[]
		, double d_p[]
		, double d_m[]
		, double bd_p[]
		, double bd_m[]
		, double std_dev)
	{
		int    i;
		double constants[8];
		double div;

		/*  The constants used in the implemenation of a casual sequence
		*  using a 4th order approximation of the gaussian operator
		*/

		div = sqrt(2 * PI) * std_dev;

		constants[0] = -1.783 / std_dev;
		constants[1] = -1.723 / std_dev;
		constants[2] = 0.6318 / std_dev;
		constants[3] = 1.997 / std_dev;
		constants[4] = 1.6803 / div;
		constants[5] = 3.735 / div;
		constants[6] = -0.6803 / div;
		constants[7] = -0.2598 / div;

		n_p[0] = constants[4] + constants[6];
		n_p[1] = exp(constants[1]) *
			(constants[7] * sin(constants[3]) -
			(constants[6] + 2 * constants[4]) * cos(constants[3])) +
			exp(constants[0]) *
			(constants[5] * sin(constants[2]) -
			(2 * constants[6] + constants[4]) * cos(constants[2]));

		n_p[2] = 2 * exp(constants[0] + constants[1]) *
			((constants[4] + constants[6]) * cos(constants[3]) * cos(constants[2]) -
			constants[5] * cos(constants[3]) * sin(constants[2]) -
			constants[7] * cos(constants[2]) * sin(constants[3])) +
			constants[6] * exp(2 * constants[0]) +
			constants[4] * exp(2 * constants[1]);

		n_p[3] = exp(constants[1] + 2 * constants[0]) *
			(constants[7] * sin(constants[3]) - constants[6] * cos(constants[3])) +
			exp(constants[0] + 2 * constants[1]) *
			(constants[5] * sin(constants[2]) - constants[4] * cos(constants[2]));

		n_p[4] = 0.0;

		d_p[0] = 0.0;
		d_p[1] = -2 * exp(constants[1]) * cos(constants[3]) -
			2 * exp(constants[0]) * cos(constants[2]);

		d_p[2] = 4 * cos(constants[3]) * cos(constants[2]) * exp(constants[0] + constants[1]) +
			exp(2 * constants[1]) + exp(2 * constants[0]);

		d_p[3] = -2 * cos(constants[2]) * exp(constants[0] + 2 * constants[1]) -
			2 * cos(constants[3]) * exp(constants[1] + 2 * constants[0]);

		d_p[4] = exp(2 * constants[0] + 2 * constants[1]);

#ifndef ORIGINAL_READABLE_CODE
		memcpy(d_m, d_p, 5 * sizeof(double));
#else
		for (i = 0; i <= 4; i++)
			d_m[i] = d_p[i];
#endif

		n_m[0] = 0.0;

		for (i = 1; i <= 4; i++)
		{
			n_m[i] = n_p[i] - d_p[i] * n_p[0];
		}

	{
		double sum_n_p, sum_n_m, sum_d;
		double a, b;

		sum_n_p = 0.0;
		sum_n_m = 0.0;
		sum_d = 0.0;

		for (i = 0; i <= 4; i++)
		{
			sum_n_p += n_p[i];
			sum_n_m += n_m[i];
			sum_d += d_p[i];
		}

#ifndef ORIGINAL_READABLE_CODE
		sum_d++;
		a = sum_n_p / sum_d;
		b = sum_n_m / sum_d;
#else
		a = sum_n_p / (1 + sum_d);
		b = sum_n_m / (1 + sum_d);
#endif

		for (i = 0; i <= 4; i++)
		{
			bd_p[i] = d_p[i] * a;
			bd_m[i] = d_m[i] * b;
		}
	}
	}

	inline void transfer_pixels(double *src1
		, double *src2
		, uint8 *dest
		, int jump
		, int width)
	{
		double sum;
		for (int i = 0; i < width; i++)
		{
			sum = src1[i] + src2[i];

			sum = clamp(sum, 0, 255);

			*dest = (uint8)sum;
			dest += jump;
		}
	}

	inline double Calc(int x, int y, float sigma)
	{
		return exp(-(x*x + y*y) / (2 * sigma * sigma)) / (2 * 3.141592654 * sigma * sigma);
	}

	void Gaussion(Bitmap* src, int radius, float sigma)
	{
		int size = (2 * radius + 1) * (2 * radius + 1);
		float* kernal = new float[size];
		assert(kernal);

		float sum = 0.0;
		for (int index = 0, y = -radius; y <= radius; y++)
		{
			for (int x = -radius; x <= radius; x++, index++)
			{
				float a = (float)Calc(x, y, sigma);
				kernal[index] = a;
				sum += a;
			}
		}

		for (int index = 0, y = -radius; y <= radius; y++)
		{
			for (int x = -radius; x <= radius; x++, index++)
			{
				float a = kernal[index] / sum;
				kernal[index] = a;
			}
		}

		Bitmap* tmp = src->Clone();
		assert(tmp);

		for (int y = 0; y < src->Height(); y++)
		{
			uint8* p1 = tmp->Get(0, y);

			for (int x = 0; x < src->Width(); x++)
			{
				float sum[3] = { 0.0f };
				for (int index = 0, i = -radius; i <= radius; i++)
				{
					for (int j = -radius; j <= radius; j++, index++)
					{
						uint8* p0 = src->Get(clamp(x + i, 0, src->Width() - 1), clamp(y + j, 0, src->Height() - 1));
						sum[0] += *(p0 + 0) * kernal[index];
						sum[1] += *(p0 + 1) * kernal[index];
						sum[2] += *(p0 + 2) * kernal[index];
					}
				}

				*(p1 + 0) = (uint8)clamp(sum[0], 0, 255);
				*(p1 + 1) = (uint8)clamp(sum[1], 0, 255);
				*(p1 + 2) = (uint8)clamp(sum[2], 0, 255);

				p1 += tmp->PixelBytes();
			}
		}

		src->Swap(*tmp);

		delete[] kernal;
		delete tmp;
	}

	void Gaussion(Bitmap* src, float radius)
	{
		uint8 *sp_p, *sp_m;
		double n_p[5], n_m[5];
		double d_p[5], d_m[5];
		double bd_p[5], bd_m[5];
		double *val_p, *val_m, *vp, *vm;
		int    terms;
		int    initial_p[4];
		int    initial_m[4];
		double std_dev;

		int width = src->Width();
		int height = src->Height();
		// 
		val_p = new double[max(width, height)];
		val_m = new double[max(width, height)];

		radius = radius + 1;
		std_dev = sqrt(-(radius * radius) / (2 * log(1.0 / 255.0)));

		find_constants(n_p, n_m, d_p, d_m, bd_p, bd_m, std_dev);

		for (int k = 0; k < src->PixelBytes(); k++)
		{
			for (int x = 0; x < width; x++)
			{
				memset(val_p, 0, height * sizeof(double));
				memset(val_m, 0, height * sizeof(double));

				sp_p = src->Get(x, 0);
				sp_m = src->Get(x, height - 1);
				vp = val_p;
				vm = val_m + (height - 1);

				/*  Set up the first vals  */
				initial_p[0] = sp_p[k];
				initial_m[0] = sp_m[k];

				for (int y = 0; y < height; y++)
				{
					terms = (y < 4) ? y : 4;

					double* vpptr = vp;
					double* vmptr = vm;

					int i, j;
					for (i = 0; i <= terms; i++)
					{
						*vpptr += n_p[i] * sp_p[-i * src->WidthBytes() + k] - d_p[i] * vp[-i];
						*vmptr += n_m[i] * sp_m[i * src->WidthBytes() + k] - d_m[i] * vm[i];
					}
					for (j = i; j <= 4; j++)
					{
						*vpptr += (n_p[j] - bd_p[j]) * initial_p[0];
						*vmptr += (n_m[j] - bd_m[j]) * initial_m[0];
					}

					sp_p += src->WidthBytes();
					sp_m -= src->WidthBytes();
					vp++;
					vm--;
				}

				transfer_pixels(val_p, val_m, src->Get(x, 0) + k, src->WidthBytes(), height);
			}
		}

		for (int k = 0; k < src->PixelBytes(); k++)
		{
			for (int y = 0; y < height; y++)
			{
				memset(val_p, 0, width * sizeof(double));
				memset(val_m, 0, width * sizeof(double));

				sp_p = src->Get(0, y);
				sp_m = src->Get(width - 1, y);
				vp = val_p;
				vm = val_m + (width - 1);

				/*  Set up the first vals  */
				initial_p[0] = sp_p[k];
				initial_m[0] = sp_m[k];

				for (int x = 0; x < width; x++)
				{
					terms = (x < 4) ? x : 4;

					double* vpptr = vp;
					double* vmptr = vm;

					int i, j;
					for (i = 0; i <= terms; i++)
					{
						*vpptr += n_p[i] * sp_p[-i * src->PixelBytes() + k] - d_p[i] * vp[-i];
						*vmptr += n_m[i] * sp_m[i * src->PixelBytes() + k] - d_m[i] * vm[i];
					}

					for (j = i; j <= 4; j++)
					{
						*vpptr += (n_p[j] - bd_p[j]) * initial_p[0];
						*vmptr += (n_m[j] - bd_m[j]) * initial_m[0];
					}

					sp_p += src->PixelBytes();
					sp_m -= src->PixelBytes();
					vp++;
					vm--;
				}

				transfer_pixels(val_p, val_m, src->Get(0, y) + k, src->PixelBytes(), width);
			}
		}

		delete[] val_p;
		delete[] val_m;
	}
}
