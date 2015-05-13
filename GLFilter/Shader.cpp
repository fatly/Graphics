#include "stdafx.h"
#include "Shader.h"

namespace e
{
	const char* g_vexterShader = {
		"void main(void)							\n"
		"{											\n"
		"	gl_TexCoord[0] = gl_MultiTexCoord0;		\n"
		"	gl_Position = ftransform();				\n"
		"}											\n"
	};

	//////////////////////////////////////////////////////////////////////////
	//----------------------动态生成shader string 部分-------------------------
	//////////////////////////////////////////////////////////////////////////

	inline double Calc(int x, int y, float sigma)
	{
		return exp(-(x*x + y*y) / (2 * sigma * sigma)) / (2 * 3.141592654 * sigma * sigma);
	}

	void MakeGauss(char** buffer, int size, int radius, float sigma)
	{
		assert(buffer != 0);
		double values[100][100] = { 0 };
		double sum = 0.0;

		for (int y = -radius; y <= radius; y++)
		{
			for (int x = -radius; x <= radius; x++)
			{
				double a = Calc(x, y, sigma);
				values[y + radius][x + radius] = a;
				sum += a;
			}
		}

		for (int index = 0, y = -radius; y <= radius; y++)
		{
			strcat_s(*buffer, size, "\t");
			char text[128] = { 0 };
			for (int x = -radius; x <= radius; x++, index++)
			{
				double a = values[y + radius][x + radius];
				a /= sum;
				values[y + radius][x + radius] = a;
				sprintf_s(text, "kernal[%d] = float(%lf); ", index, a);
				strcat_s(*buffer, size, text);
			}
			strcat_s(*buffer, size, "\r\n");
		}
	}

	const char* CreateShaderString(int radius, float sigma, bool gray)
	{
		assert(radius >= 1);
		assert(sigma >= 0.1f);

		int totalBytes = 10240 * 4;
		char* buffer = new char[totalBytes];
		assert(buffer);
		if (buffer == 0) return 0;

		char* kernal = new char[totalBytes];
		assert(kernal);
		if (kernal == 0)
		{
			delete[] buffer;
			return 0;
		}

		memset(buffer, 0, sizeof(char)*totalBytes);
		memset(kernal, 0, sizeof(char)*totalBytes);

		MakeGauss(&kernal, totalBytes, radius, sigma);

		int count = (radius * 2 + 1) * (radius * 2 + 1);

		sprintf_s(buffer
			, totalBytes
			, "uniform sampler2D Texture0;															\n"
			"uniform sampler2D Texture1;															\n"
			"uniform vec2 TexSize;																	\n"
			"uniform int enableMatte;																\n"
			"float kernal[%d];																		\n"
			
			"vec4 gray(vec4 a)																		\n"
			"{																						\n"
			"	float c = 0.3 * a.x + 0.59 * a.y + 0.11 * a.z;										\n"
			"	return vec4(c, c, c, 0);															\n"
			"}																						\n"

			"vec4 screen(vec4 a, vec4 b)															\n"
			"{																						\n"
			"	a = (1.0 - a) * 255; b = (1.0 - b) * 255;											\n"
			"	vec4 t = a * b + 128;																\n"
			"	vec4 c = (t / 255 + t) / 255;														\n"
			"	return 1.0 - c/255;																	\n"
			"}																						\n"

			"vec4 softlight(vec4 a, vec4 b)															\n"
			"{																						\n"
			"	return 2.0 * a * b + a * a - 2.0 * a * a * b;										\n"
			"}																						\n"

			"vec4 gauss_filter(float kernal[%d], int radius, sampler2D image, vec2 uv, vec2 texSize)\n"
			"{																						\n"
			"	vec4 c = vec4(0.0, 0.0, 0.0, 0.0);													\n"
			"	float dx = 1.0 / texSize.x;															\n"
			"	float dy = 1.0 / texSize.y;															\n"
			"	for (int n=0, y=-radius; y<=radius; y++)											\n"
			"	{																					\n"
			"		for (int x=-radius; x<=radius; x++, n++)										\n"
			"		{																				\n"
			"			vec2 _uv = vec2(uv.x + x * dx, uv.y + y * dy);								\n"
			"			c += texture2D(image, _uv) * kernal[n];										\n"
			"		}																				\n"
			"	}																					\n"
			"	return c;																			\n"
			"}																						\n"

			"vec4 alpha_blend(vec4 a, vec4 b)														\n"
			"{																						\n"
			"	return a * vec4(1.0 - b.a) + b * vec4(b.a);											\n"
			"}																						\n"

			"void main(void)																		\n"
			"{																						\n"
			"	int radius = %d;																	\n"
			"%s\n"
			"	vec2 uv = gl_TexCoord[0].st;														\n"
			"	vec4 a = texture2D(Texture0, uv);													\n"
			"	vec4 b = gauss_filter(kernal, radius, Texture0, uv, TexSize);						\n"
			"   vec4 c = softlight(a, %s);															\n"
			"	if (enableMatte == 0)																\n"
			"	{																					\n"
			"		gl_FragColor = c;																\n"
			"   }																					\n"
			"   else																				\n"
			"	{																					\n"
			"       vec4 e = texture2D(Texture1, uv);												\n"
			"		gl_FragColor = alpha_blend(c, e);												\n"
			"   }																					\n"
			"}																						\n"
			, count
			, count
			, radius
			, kernal
			, gray?"gray(b)":"b");

		delete[] kernal;

		return buffer;
	}
}
