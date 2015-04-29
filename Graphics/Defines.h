#ifndef __CORE_DEFINE_H__
#define __CORE_DEFINE_H__

// 不定长整形
	typedef char int8;
	typedef short int16;
	typedef unsigned int uint;
	typedef unsigned char uint8;
	typedef unsigned short uint16;
	typedef unsigned char byte;
// 定长整型
#if defined(_WIN64) || defined(__x86_64__)
	typedef int int32;
	typedef unsigned int uint32;
	typedef long long int64;
	typedef unsigned long long uint64;
	typedef long long intx;
	typedef unsigned long long uintx;
#else
	typedef int int32;
	typedef unsigned int uint32;
	typedef long long int64;
	typedef unsigned long long uint64;
	typedef int intx;
	typedef unsigned int uintx;
#endif

	typedef uint32 RGBA;

#define RC(x) (x & 0x000000ff)
#define GC(x) ((x & 0x0000ff00) >> 8)
#define BC(x) ((x & 0x00ff0000) >> 16)
#define AC(x) ((x & 0xff000000) >> 24)

#ifndef min
#define min(a, b) ((a)<(b)?(a):(b))
#endif

#ifndef max
#define max(a, b) ((a)<(b)?(b):(a))
#endif

#ifndef clamp
#define clamp(x, a, b) max((a), min((x), (b)))
#endif

#ifndef	WIDTHBYTES
#define WIDTHBYTES(bits) (((bits) + 31) / 32 * 4)
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if ((x) != 0){delete (x); (x) = 0;}
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) if ((x) != 0){delete[] (x); (x) = 0;}
#endif

#ifndef VIDEO_WIDTH
#define VIDEO_WIDTH 640
#endif

#ifndef VIDEO_HEIGHT
#define VIDEO_HEIGHT 480
#endif

	template<class T> inline void swap(T & l, T & r)
	{
		T temp = l;
		l = r;
		r = temp;
	}

	template<class T> inline void limit(T & value
		, const T & minValue
		, const T & maxValue)
	{
		value = max((minValue), min((value), (maxValue)));
	}


#endif