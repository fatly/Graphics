#include "stdafx.h"
#include "Shader.h"

namespace e
{
	const char * vexterShader = {
		"void main(void)							\n"
		"{											\n"
		"	gl_TexCoord[0] = gl_MultiTexCoord0;		\n"
		"	gl_Position = ftransform();				\n"
		"}											\n"
	};

	const char * fragmentShader = {
		"uniform sampler2D Texture0;															\n"
		"uniform sampler2D Texture1;															\n"
		"uniform vec2 TexSize;																	\n"
		"uniform int enableMatte;																\n"
		"float kernal[49];																		\n"

		"float hardlight(float a, float b)														\n"
		"{																						\n"
		"	if (b <= 0.5)																		\n"
		"	{																					\n"
		"		return a * b / 0.5;																\n"
		"	}																					\n"
		"	else																				\n"
		"	{																					\n"
		"		return 1.0 - (1.0 - a) * (1.0 - b) / 0.5;										\n"
		"	}																					\n"
		"}																						\n"

		"float softlight(float a, float b)														\n"
		"{																						\n"
		"	if (b <= 0.5)																		\n"
		"	{																					\n"
		"		return a * b / 0.5 + (a / 1.0) * (a / 1.0) * (1.0 - 2 * b);						\n"
		"	}																					\n"
		"	else																				\n"
		"	{																					\n"
		"		return a * (1.0 - b) / 0.5 + sqrt(a / 1.0) * (2.0 * b - 1.0);					\n"
		"	}																					\n"
		"}																						\n"

		"vec4 softlight(vec4 a, vec4 b)															\n"
		"{																						\n"
		"	//return vec4(softlight(a.x, b.x), softlight(a.y, b.y), softlight(a.z, b.z), 1.0);	\n"
		"	return 2.0 * a * b + a * a - 2.0 * a * a * b;										\n"
		"}																						\n"

		"vec4 gauss_filter(float kernal[49], int radius, sampler2D image, vec2 uv, vec2 texSize)\n"
		"{																						\n"
		"	vec4 c = vec4(0.0, 0.0, 0.0, 0.0);													\n"
		"   float dx = 1.0 / texSize.x;															\n"
		"   float dy = 1.0 / texSize.y;															\n"
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
		"	int radius = 3;																		\n"
		"	kernal[0] = float(0.019407); kernal[1] = float(0.019898); kernal[2] = float(0.020199); kernal[3] = float(0.020300); kernal[4] = float(0.020199); kernal[5] = float(0.019898); kernal[6] = float(0.019407); \n"
		"	kernal[7] = float(0.019898); kernal[8] = float(0.020402); kernal[9] = float(0.020710); kernal[10] = float(0.020814); kernal[11] = float(0.020710); kernal[12] = float(0.020402); kernal[13] = float(0.019898); \n"
		"	kernal[14] = float(0.020199); kernal[15] = float(0.020710); kernal[16] = float(0.021023); kernal[17] = float(0.021129); kernal[18] = float(0.021023); kernal[19] = float(0.020710); kernal[20] = float(0.020199); \n"
		"	kernal[21] = float(0.020300); kernal[22] = float(0.020814); kernal[23] = float(0.021129); kernal[24] = float(0.021235); kernal[25] = float(0.021129); kernal[26] = float(0.020814); kernal[27] = float(0.020300); \n"
		"	kernal[28] = float(0.020199); kernal[29] = float(0.020710); kernal[30] = float(0.021023); kernal[31] = float(0.021129); kernal[32] = float(0.021023); kernal[33] = float(0.020710); kernal[34] = float(0.020199); \n"
		"	kernal[35] = float(0.019898); kernal[36] = float(0.020402); kernal[37] = float(0.020710); kernal[38] = float(0.020814); kernal[39] = float(0.020710); kernal[40] = float(0.020402); kernal[41] = float(0.019898); \n"
		"	kernal[42] = float(0.019407); kernal[43] = float(0.019898); kernal[44] = float(0.020199); kernal[45] = float(0.020300); kernal[46] = float(0.020199); kernal[47] = float(0.019898); kernal[48] = float(0.019407); \n"
		"	vec2 uv = gl_TexCoord[0].st;														\n"
		"	vec4 a = texture2D(Texture0, uv);													\n"
		"	vec4 b = gauss_filter(kernal, radius, Texture0, uv, TexSize);						\n"
		"   vec4 c = softlight(b, a);															\n"
		"                                                                                       \n"
		"	if (enableMatte == 0)																\n"
		"	{																					\n"
		"		gl_FragColor = c;																\n"
		"   }																					\n"
		"   else																				\n"
		"	{																					\n"
		"       vec4 d = texture2D(Texture1, uv);												\n"
		"		gl_FragColor = alpha_blend(c, d);												\n"
		"   }																					\n"
		"}																						\n"
	};
}