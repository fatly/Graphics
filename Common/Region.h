#ifndef __CORE_REGION_H__
#define __CORE_REGION_H__

#include "BaseClass.h"
#include "Bitmap.h"
#include <assert.h>

#define PRE_LOCATION_BIAS 2

namespace e
{	
	static int JoinBrokenRegion(Region* region
		, int width
		, int height
		, int dx
		, int dy
		, int xthreshold
		, int ythreshold
		, float minRate
		, float maxRate);

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

		static int direction[4][2] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };
		Point* queue = new Point[width * height];
		assert(queue != 0);

		Bitmap* tmp = src->Clone();
		assert(tmp != 0);

		region->count[0] = 0;
		region->count[1] = 0;
		region->count[2] = 0;
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

					region->regions[0][region->count[0]].x0 = x;
					region->regions[0][region->count[0]].x1 = x;
					region->regions[0][region->count[0]].y0 = y;
					region->regions[0][region->count[0]].y1 = y;

					if (region->count[0] > MAX_REGION_COUNT)
					{
						goto _outloop;
					}

					while (head < rear)
					{
						x0 = queue[head].x;
						y0 = queue[head].y;
						head++;

						if (x0 < region->regions[0][region->count[0]].x0)
						{
							region->regions[0][region->count[0]].x0 = x0;
						}
						else if (x0 > region->regions[0][region->count[0]].x1)
						{
							region->regions[0][region->count[0]].x1 = x0;
						}
						
						if (y0 < region->regions[0][region->count[0]].y0)
						{
							region->regions[0][region->count[0]].y0 = y0;
						}
						else if (y0 > region->regions[0][region->count[0]].y1)
						{
							region->regions[0][region->count[0]].y1 = y0;
						}

						for (int i = 0; i < 4; i++)
						{
							x1 = x0 + direction[i][0];
							y1 = y0 + direction[i][1];

							if (x1>=0 && x1 < width && y1>=0 && y1 < height)
							{
								uint8* a = tmp->Get(x1, y1);

								if (*a == 255)
								{
									*a = 0;
									queue[rear].x = x1;
									queue[rear].y = y1;
									rear++;
								}
							}
						}
					}//end while(完成一个region的查找)

					//对找到的region进行筛选
					Region::Item &item = region->regions[0][region->count[0]];

					if ((item.x1 - item.x0 > xthreshold) && (item.y1 - item.y0 > ythreshold))
					{
						item.width  = item.x1 - item.x0 + 1;
						item.height = item.y1 - item.y0 + 1;
						item.rate = (float)item.width / item.height;

						if ((item.width < width / 2) && (item.height < height / 2))
						{
							if (item.rate >= minRate && item.rate <= maxRate)
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

								region->count[0]++;
							}
							else
							{
								Region::Item temp = item;
								temp.width  = temp.x1 - temp.x0 + 1;
								temp.height = temp.y1 - temp.y0 + 1;
								temp.rate = (float)temp.width / temp.height;
								region->regions[1][region->count[1]++] = temp;
							}
						}
					}// end if
					else if ((item.x1 - item.x0 > xthreshold / 7) && (item.y1 - item.y0 >= ythreshold))
					{
						Region::Item temp = item;
						temp.width = temp.x1 - temp.x0 + 1;
						temp.height = temp.y1 - temp.y0 + 1;
						temp.rate = (float)temp.width / temp.height;

						if (region->count[1] < MAX_REGION_COUNT)
						{
							region->regions[1][region->count[1]++] = temp;
						}
					}
					else
					{
						Region::Item temp = item;
						temp.width  = temp.x1 - temp.x0 + 1;
						temp.height = temp.y1 - temp.y0 + 1;
						temp.rate = (float)temp.width / temp.height;

						if (temp.rate >= minRate && temp.rate <= maxRate)
						{
							if (region->count[2] < MAX_REGION_COUNT)
							{
								region->regions[2][region->count[2]++] = temp;
							}
						}
					}
				}
			}
		}

		JoinBrokenRegion(region, width, height, 5, 5, xthreshold, ythreshold, minRate, maxRate);

	_outloop:

		delete queue;
		delete tmp;

		return region->count[0];
	}

	static int GridID(Region::Item & item, int width, int height, int rw, int rh)
	{
		//center point
		int x = (item.x0 + item.x1) / 2;
		int y = (item.y0 + item.y1) / 2;

		int cols = (width / rw) + ((width % rw) > 0 ? 1 : 0);
		//int rows = (height / ry) + ((height % ry) > 0 ? 1 : 0);

		x /= rw;
		y /= rh;

		return y * cols + x;
	}

	static bool IsSameRow(int id0, int id1, int width, int rw)
	{
		int cols = (width / rw) + ((width % rw) > 0 ? 1 : 0);
		//int rows = (height / rh) + ((height % rh) > 0 ? 1 : 0);

		return (id0 / cols) == (id1 / cols);
	}

	static int JoinBrokenRegion(Region* region
		, int width
		, int height
		, int dx
		, int dy
		, int xthreshold
		, int ythreshold
		, float minRate
		, float maxRate)
	{
		if (region->count[1] < 2) return 0;

		int rw = region->regions[1][0].width;
		int rh = ythreshold;
		for (int i = 1; i < region->count[1]; i++)
		{
			if (region->regions[1][i].width < rw)
			{
				rw = region->regions[1][i].height;
			}
		}

		//sort by id
		for (int i = 0; i < region->count[1] - 1; i++)
		{
			int k = i; 
			int id0 = GridID(region->regions[1][i], width, height, rw, rh);
			for (int j = i + 1; j < region->count[1]; j++)
			{
				int id1 = GridID(region->regions[1][j], width, height, rw, rh);
				if (id1 < id0)
				{
					id0 = id1;
					k = j;
				}
			}

			if (k != i)
			{
				Region::Item temp = region->regions[1][i];
				region->regions[1][i] = region->regions[1][k];
				region->regions[1][k] = temp;
			}
		}
		
		for (int i = 0; i < region->count[1] - 1; i++)
		{
			int count = 0;
			Region::Item &prev = region->regions[1][i];
			Region::Item &cur = region->regions[1][i + 1];
			int id0 = GridID(prev, width, height, rw, rh);
			int id1 = GridID(cur, width, height, rw, rh);
			if (!IsSameRow(id0, id1, width, rw)) continue;

			int dx0 = cur.x0 - prev.x1;
			int dy0 = min(abs(prev.y0 - cur.y0), abs(prev.y1 - cur.y1));

			if (dx0 <= dx && dy0 <= dy)
			{
				count = 2;

				if (i + 2 < region->count[1])
				{
					Region::Item &next = region->regions[1][i + 2];
					int id2 = GridID(next, width, height, rw, rh);

					if (IsSameRow(id1, id2, width, rw))
					{
						int dx1 = next.x0 - cur.x1;
						int dy1 = min(abs(cur.y0 - next.y0), abs(cur.y1 - next.y1));

						if (dx1 <= dx && dy1 <= dy)
						{
							count = 3;
						}
					}
				}

				int x0, y0, x1, y1, w, h;

				if (count == 2)
				{
					x0 = prev.x0;
					y0 = min(prev.y0, cur.y0);
					x1 = cur.x1;
					y1 = max(prev.y1, cur.y1);
					w = x1 - x0 + 1;
					h = y1 - y0 + 1;

					float rate = (float)(cur.x1 - cur.x0 + 1) / (prev.x1 - prev.x0 + 1);
					if (2.2f < rate && rate < 3.0f)//must be broken,and has next item
					{
						x1 += prev.x1 - prev.x0 + 1;
						w += prev.x1 - prev.x0 + 1;
					}
				}
				else if (count == 3)
				{
					Region::Item &next = region->regions[1][i + 2];
					x0 = prev.x0;
					y0 = min(prev.y0, min(cur.y0, next.y0));
					x1 = next.x1;
					y1 = max(prev.y1, max(cur.y1, next.y1));
					w = x1 - x0 + 1;
					h = y1 - y0 + 1;
				}

				if (w >= xthreshold && h >= ythreshold)
				{
					float rate = (float)w / h;

					if (rate >= minRate && rate <= maxRate)
					{
						Region::Item & item = region->regions[0][region->count[0]];

						item.x0 = x0;
						item.y0 = y0;
						item.x1 = x1;
						item.y1 = y1;

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

						item.width = item.x1 - item.x0 + 1;
						item.height = item.y1 - item.y0 + 1;
						item.rate = (float)item.width / item.height;

						region->count[0]++;
					}
				}
			}

			if (count == 2)
				i += 1;
			else if (count == 3)
				i += 2;
		}
		
		return 0;
	}
}

#endif
