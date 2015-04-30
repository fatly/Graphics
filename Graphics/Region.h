#ifndef __CORE_REGION_H__
#define __CORE_REGION_H__

#include "BaseClass.h"
#include "Bitmap.h"
#include <assert.h>

#define PRE_LOCATION_BIAS	0

namespace e
{	
	static int _FindRegion(Region* region
		, const Bitmap* src
		, int xthreshold
		, int ythreshold
		, float minRate
		, float maxRate)
	{
		int x0, y0, x1, y1;
		int head, rear;
		int width = src->Width();
		int height = src->Height();
		int lineBytes = src->WidthBytes();
		int bytesPerPixel = src->PixelBytes();

		static int direction[4][2] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };
		Point* queue = new Point[width * height * 4];
		assert(queue != 0);

		Bitmap* tmp = src->Clone();
		assert(tmp != 0);

		region->count = 0;
		head = rear = 0;

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				uint8* p = tmp->Get(x, y);

				if (*p == 255)
				{
					queue[rear].x = x;
					queue[rear].y = y;
					rear++;

					*p = 0;

					region->regions[region->count].x0 = x;
					region->regions[region->count].x1 = x;
					region->regions[region->count].y0 = y;
					region->regions[region->count].y1 = y;

					if (region->count > MAX_REGION_COUNT)
					{
						goto _outloop;
					}

					while (head < rear)
					{
						x0 = queue[head].x;
						y0 = queue[head].y;
						head++;

						if (x0 < region->regions[region->count].x0)
						{
							region->regions[region->count].x0 = x0;
						}
						else if (x0 > region->regions[region->count].x1)
						{
							region->regions[region->count].x1 = x0;
						}
						
						if (y0 < region->regions[region->count].y0)
						{
							region->regions[region->count].y0 = y0;
						}
						else if (y0 > region->regions[region->count].y1)
						{
							region->regions[region->count].y1 = y0;
						}

						for (int i = 0; i < 4; i++)
						{
							x1 = x0 + direction[i][0];
							y1 = y0 + direction[i][1];
						}

						if (x1>0 && x1< width && y1>0 && y1 < height)
						{
							uint8* a = tmp->Get(x1, y1);

							if (*a != 0)
							{
								*a = 0;
								queue[rear].x = x1;
								queue[rear].y = y1;
								rear++;
							}
						}
					}//end while

					Region::Item &item = region->regions[region->count];

					if ((item.x1 - item.x0 >= xthreshold) && (item.y1 - item.y0 >= ythreshold))
					{
						item.width  = item.x1 - item.x0 + 1;
						item.height = item.y1 - item.y0 + 1;
						item.rate = (float)item.width / item.height;

						if ((item.width < width / 2) && (item.height < height / 2))
						{
							if (item.rate > minRate && item.rate < maxRate)
							{
								if (item.x0 - PRE_LOCATION_BIAS < 0)
								{
									item.x0 = 0;
								}
								else
								{
									item.x0 -= PRE_LOCATION_BIAS;
								}

								if (item.x1 + PRE_LOCATION_BIAS > width - 1)
								{
									item.x1 = width - 1;
								}
								else
								{
									item.x1 += PRE_LOCATION_BIAS;
								}

								if (item.y0 - PRE_LOCATION_BIAS < 0)
								{
									item.y0 = 0;
								}
								else
								{
									item.y0 -= PRE_LOCATION_BIAS;
								}

								if (item.y1 + PRE_LOCATION_BIAS > height - 1)
								{
									item.y1 = height - 1;
								}
								else
								{
									item.y1 += PRE_LOCATION_BIAS;
								}

								item.width  = item.x1 - item.x0 + 1;
								item.height = item.y1 - item.y0 + 1;

								region->count++;
							}
						}
					}
				}
			}
		}
	_outloop:

		delete queue;
		delete tmp;

		return region->count;
	}
}

#endif
