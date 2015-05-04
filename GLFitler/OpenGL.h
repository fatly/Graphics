#ifndef __CORE_OPENGL_H__
#define __CORE_OPENGL_H__

#include <gl/glew.h>
#if defined (_WIN32)
#include <GL/wglew.h>
#endif
#include <gl/GL.h>

#include "Bitmap.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

namespace e
{
	class OpenGL
	{
	public:
		OpenGL(HDC dc);
		~OpenGL(void);
		bool Process(uint8 * buffer, int width, int height, int bitCount);
		//设置蒙版，当buffer为NULL时关闭蒙版功能，否则打开; 蒙版需要32bit的RGBA
		bool SetMatte(uint8 * buffer, int width, int height, int bitCount);
		void SetParam(int radius, float sigma);
	private:
		bool Initialize(HDC dc, int width, int height);
		bool CreateShader(const char* fileName, GLenum type, GLuint & shader);
		bool CreateShaderEx(const char* buffer, GLenum type, GLuint & shader);
		bool CreateProgram(void);
		char* GetErrorString(GLuint object);
		bool InitTexture(int width, int height);
		bool SetupPixelFormat(HDC dc);
		void MakeGauss(char** buffer, int size);
		void GenFile(void);
		void Cleanup(void);
	private:
		GLuint program;
		GLuint texInput;
		GLuint texOutput;
		GLuint texMatte;
		GLuint frameBuffer;
		Bitmap bitmap;
		HGLRC  rc;
		HDC    dc;
		int	   radius;
		float  sigma;
	};
}



#endif
