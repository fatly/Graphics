#ifndef __CORE_SHADER_H__
#define __CORE_SHADER_H__

namespace e
{
	extern const char* g_vexterShader;

	const char* CreateShaderString(int radius, float sigma, bool gray);
}

#endif

