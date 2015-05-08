#ifndef __CORE_GAUSSION_H__
#define __CORE_GAUSSION_H__

#include "Bitmap.h"

namespace e
{
	void Gaussion(Bitmap* src, int radius, float sigma);

	void Gaussion(Bitmap* src, float radius);
}

#endif