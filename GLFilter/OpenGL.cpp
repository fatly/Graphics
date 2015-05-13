#include "stdafx.h"
#include "OpenGL.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Utility.h"
#include "Shader.h"

namespace e
{
	void BRG2RGBA(uint8 * des, uint8 * src, int width, int height)
	{
		for (int y = 0; y < height; y++)
		{
			uint8 * p0 = des + y * WIDTHBYTES(32 * width);
			uint8 * p1 = src + y * WIDTHBYTES(24 * width);

			for (int x = 0; x < width; x++)
			{
				*(p0 + 0) = *(p1 + 2);
				*(p0 + 1) = *(p1 + 1);
				*(p0 + 2) = *(p1 + 0);
				*(p0 + 3) = 0xff;

				p0 += 4;
				p1 += 3;
			}
		}
	}

	void RGBA2BRG(uint8 * des, uint8 * src, int width, int height)
	{
		for (int y = 0; y < height; y++)
		{
			uint8 * p0 = des + y * WIDTHBYTES(24 * width);
			uint8 * p1 = src + y * WIDTHBYTES(32 * width);

			for (int x = 0; x < width; x++)
			{
				*(p0 + 0) = *(p1 + 2);
				*(p0 + 1) = *(p1 + 1);
				*(p0 + 2) = *(p1 + 0);

				p0 += 3;
				p1 += 4;
			}
		}
	}

	OpenGL::OpenGL(HDC dc)
	{
		//bool ret = Initialize(dc, VIDEO_WIDTH, VIDEO_HEIGHT);
		//assert(ret != false);
		this->dc = dc;
		this->radius = 3;
		this->sigma = 3.5f;
		this->gray = false;
	}

	OpenGL::~OpenGL(void)
	{
		Cleanup();
	}

	bool OpenGL::Initialize(HDC dc, int width, int height)
	{
		//int argc = 0;
		//glutInit(&argc, 0);
		//glutCreateWindow("filter");

		if (!SetupPixelFormat(dc))
		{
			return false;
		}

		rc = wglCreateContext(dc);
		wglMakeCurrent(dc, rc);

		char * version = (char*)glGetString(GL_VERSION);
		printf("OpenGL : version = %s\n", version);

		GLenum result = glewInit();
		if (result != GLEW_OK)
		{
			OutputDebugString(TEXT("OpenGL : init glew failed\n"));
			return false;
		}

		if (!glewIsSupported("GL_VERSION_2_0"))// GL_VERSION_1_5 GL_ARB_multitexture GL_ARB_vertex_buffer_object
		{
			MessageBox(NULL, TEXT("Required OpenGL extensions missing."), NULL, 0);
			return false;
		}

		glShadeModel(GL_SMOOTH);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glViewport(0, 0, width, height);
		glEnable(GL_TEXTURE_2D);

		if (!CreateProgram())
		{
			return false;
		}

		//创建FBO，准备屏幕外帧缓存
		glGenFramebuffersEXT(1, &frameBuffer);
		//绑定屏幕外帧缓存，即避开了窗口系统默认的渲染目标
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBuffer);

		//初始化纹理
		InitTexture(width, height);

		glUseProgram(program);
		GLint loc1 = glGetUniformLocation(program, "Texture0");
		GLint loc2 = glGetUniformLocation(program, "Texture1");
		GLint loc3 = glGetUniformLocation(program, "TexSize");
		GLint loc4 = glGetUniformLocation(program, "enableMatte");
		glUniform1i(loc1, 0);
		glUniform1i(loc2, 2);
		glUniform2f(loc3, (GLfloat)width, (GLfloat)height);
		glUniform1i(loc4, 0);

		return true;
	}

	bool OpenGL::Process(uint8 * buffer, int width, int height, int bitCount)
	{
		//init glsl
		if (!Initialize(dc, width, height))
		{
			return false;
		}

		glBindTexture(GL_TEXTURE_2D, texInput);

		//当卷积内核超过了图像边界时使用图像边缘的像素值
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		//纹理过滤的方式不应该设置为线性插值
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		if (bitCount == 24)
		{
			BRG2RGBA(bitmap.bits, buffer, width, height);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap.bits);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		}

		//蒙版
		glBindTexture(GL_TEXTURE_2D, texMatte);
	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//关联输出缓存至FBO
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
			GL_COLOR_ATTACHMENT0_EXT,
			GL_TEXTURE_2D,
			texOutput,
			0);

		//glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

		glBegin(GL_QUADS);

		glMultiTexCoord2f(GL_TEXTURE0, 0.0, 0.0);
		glVertex2f(-1.0, -1.0);

		glMultiTexCoord2f(GL_TEXTURE0, 0.0, 1.0);
		glVertex2f(-1.0, 1.0);

		glMultiTexCoord2f(GL_TEXTURE0, 1.0, 1.0);
		glVertex2f(1.0, 1.0);

		glMultiTexCoord2f(GL_TEXTURE0, 1.0, 0.0);
		glVertex2f(1.0, -1.0);

		glEnd();
		glFlush();

		glBindTexture(GL_TEXTURE_2D, texOutput);

		if (bitCount == 24)
		{
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap.bits);
			RGBA2BRG(buffer, bitmap.bits, width, height);
		}
		else
		{
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		}

		Cleanup();

		return true;
	}

	bool OpenGL::CreateShader(const char* fileName, GLenum type, GLuint & shader)
	{
		int fileSize = 0;
		char * buffer = ReadFile(fileName, fileSize);
		if (buffer == NULL)
		{
			OutputDebugString(TEXT("OpenGL : open file failed"));
			return false;
		}

		shader = glCreateShader(type);
		const char* sources[2] = {
#ifdef GL_ES_VERSION_2_0
			"version 100\n"
			"#define GLES2\n",
#else
			"#version 120\n",
#endif
			buffer
		};

		glShaderSource(shader, 2, sources, NULL);

		Release(buffer);

		glCompileShader(shader);

		GLint result = GL_FALSE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

		if (result == GL_FALSE)
		{
			char * buffer = GetErrorString(shader);
			if (buffer != NULL)
			{
				OutputDebugStringA(buffer);
				Release(buffer);
			}

			glDeleteShader(shader);
			return false;
		}

		return true;
	}

	bool OpenGL::CreateShaderEx(const char* buffer, GLenum type, GLuint & shader)
	{
		shader = glCreateShader(type);
		const char* sources[2] = {
#ifdef GL_ES_VERSION_2_0
			"version 100\n"
			"#define GLES2\n",
#else
			"#version 120\n",
#endif
			buffer
		};

		glShaderSource(shader, 2, sources, NULL);

		glCompileShader(shader);

		GLint result = GL_FALSE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

		if (result == GL_FALSE)
		{
			char * buffer = GetErrorString(shader);
			if (buffer != NULL)
			{
				OutputDebugStringA(buffer);
				Release(buffer);
			}

			glDeleteShader(shader);
			return false;
		}

		return true;
	}

	bool OpenGL::CreateProgram(void)
	{
		GLint result = GL_FALSE;
		GLuint vs, fs;
#if 0
		char basePath[MAX_PATH] = { 0 };
		char fileName[MAX_PATH] = { 0 };
		GetExeDir(basePath, MAX_PATH);

		strcpy_s(fileName, basePath);
		strcat_s(fileName, "filter.vs");
		if (!CreateShader(fileName, GL_VERTEX_SHADER, vs))
		{
			return false;
		}

		strcpy_s(fileName, basePath);
		strcat_s(fileName, "filter.fs");
		if (!CreateShader(fileName, GL_FRAGMENT_SHADER, fs))
		{
			return false;
		}
#else
		if (!CreateShaderEx(g_vexterShader, GL_VERTEX_SHADER, vs))
		{
			return false;
		}
		const char* fragmentShader = CreateShaderString(radius, sigma, gray);
		if (fragmentShader == 0)
		{
			return false;
		}

		if (!CreateShaderEx(fragmentShader, GL_FRAGMENT_SHADER, fs))
		{
			delete fragmentShader;
			return false;
		}

		delete fragmentShader;
#endif
		program = glCreateProgram();

		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &result);

		if (!result)
		{
			char * buffer = GetErrorString(program);
			if (buffer != NULL)
			{
				OutputDebugStringA(buffer);
				Release(buffer);
			}
			return false;
		}

		return true;
	}

	char * OpenGL::GetErrorString(GLuint object)
	{
		GLint length = 0;
		if (glIsShader(object))
		{
			glGetShaderiv(object, GL_INFO_LOG_LENGTH, &length);
		}
		else if (glIsProgram(object))
		{
			glGetProgramiv(object, GL_INFO_LOG_LENGTH, &length);
		}
		else
		{
			return NULL;
		}

		char * buffer = (char*)malloc(length + 1);
		if (buffer == NULL)
		{
			return NULL;
		}

		if (glIsShader(object))
		{
			glGetShaderInfoLog(object, length, NULL, buffer);
		}
		else if (glIsProgram(object))
		{
			glGetProgramInfoLog(object, length, NULL, buffer);
		}

		buffer[length] = '\0';
		return buffer;
	}

	bool OpenGL::InitTexture(int width, int height)
	{
		//初次分配空间
		bitmap.Alloc(width, height, 32);

		glGenTextures(1, &texInput);
		glBindTexture(GL_TEXTURE_2D, texInput);

		//当卷积内核超过了图像边界时使用图像边缘的像素值
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		//纹理过滤的方式不应该设置为线性插值
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap.bits);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glGenTextures(1, &texOutput);
		glBindTexture(GL_TEXTURE_2D, texOutput);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		//纹理过滤的方式不应该设置为线性插值
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//没有给输出的纹理数据，等待程序进行赋值
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glGenTextures(1, &texMatte);
		glBindTexture(GL_TEXTURE_2D, texMatte);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		//纹理过滤的方式不应该设置为线性插值
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//没有给输出的纹理数据，等待程序进行赋值
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texInput);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texOutput);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texMatte);

		return true;
	}

	bool OpenGL::SetupPixelFormat(HDC dc)
	{
		static PIXELFORMATDESCRIPTOR pfd =	//定义像素格式
		{
			sizeof(PIXELFORMATDESCRIPTOR),	// 上述格式描述符的大小
			1,								// 版本号
			PFD_DRAW_TO_WINDOW |			// 格式支持窗口
			PFD_SUPPORT_OPENGL |			// 格式必须支持OpenGL
			PFD_DOUBLEBUFFER,				// 必须支持双缓冲
			PFD_TYPE_RGBA,					// 申请 RGBA 格式
			24,								// 24位色彩深度，即1.67千万的真彩色
			0, 0, 0, 0, 0, 0,				// 忽略的色彩位
			0,								// 无Alpha缓存
			0,								// 忽略Shift Bit
			0,								// 无累加缓存
			0, 0, 0, 0,						// 忽略聚集位
			32,								// 32位 Z-缓存 (深度缓存)
			0,								// 无蒙板缓存
			0,								// 无辅助缓存
			PFD_MAIN_PLANE,					// 主绘图层
			0,								// Reserved
			0, 0, 0							// 忽略层遮罩
		};

		int index = ChoosePixelFormat(dc, &pfd); //选择刚刚定义的像素格式
		if (index == 0) return false;

		return SetPixelFormat(dc, index, &pfd) ? true : false;   //设置像素格式
	}

	bool OpenGL::SetMatte(uint8 * buffer, int width, int height, int bitCount)
	{
		if (buffer != NULL)
		{
			glBindTexture(GL_TEXTURE_2D, texMatte);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

			//纹理过滤的方式不应该设置为线性插值
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			//没有给输出的纹理数据，等待程序进行赋值
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

			GLint loc = glGetUniformLocation(program, "enableMatte");
			glUniform1i(loc, 1);
		}
		else
		{
			GLint loc = glGetUniformLocation(program, "enableMatte");
			glUniform1i(loc, 0);
		}

		return true;
	}

	void OpenGL::SetParam(int radius, float sigma, bool gray)
	{
		this->radius = radius;
		this->sigma = sigma;
		this->gray = gray;
	}

	void OpenGL::Cleanup(void)
	{
		if (program != 0)
		{
			glDeleteProgram(program);
			program = 0;
		}

		if (texInput != 0)
		{
			glDeleteTextures(1, &texInput);
			texInput = 0;
		}

		if (texOutput != 0)
		{
			glDeleteTextures(1, &texOutput);
			texOutput = 0;
		}

		if (texMatte != 0)
		{
			glDeleteTextures(1, &texMatte);
			texMatte = 0;
		}

		if (frameBuffer != 0)
		{
			glDeleteFramebuffers(1, &frameBuffer);
			frameBuffer = 0;
		}

		if (rc != 0)
		{
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(rc);
			rc = 0;
		}
	}
}
