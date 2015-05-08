#ifndef __CORE_SOFTGLOW_H__
#define __CORE_SOFTGLOW_H__

namespace e
{
	class SoftGlow
	{
	public:
		SoftGlow();
		virtual ~SoftGlow();

		Bitmap* Process1(const Bitmap* src, int radius, float sharpness, float brightness);
		Bitmap* Process2(const Bitmap* src, int radius, float sharpness, float brightness);

	private:

	};
}

#endif