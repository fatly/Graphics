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

		//����FBO��׼����Ļ��֡����
		glGenFramebuffersEXT(1, &frameBuffer);
		//����Ļ��֡���棬���ܿ��˴���ϵͳĬ�ϵ���ȾĿ��
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frameBuffer);

		//��ʼ������
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
		Initialize(dc, width, height);

		glBindTexture(GL_TEXTURE_2D, texInput);

		//������ں˳�����ͼ��߽�ʱʹ��ͼ���Ե������ֵ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		//������˵ķ�ʽ��Ӧ������Ϊ���Բ�ֵ
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

		//�ɰ�
		glBindTexture(GL_TEXTURE_2D, texMatte);
	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//�������������FBO
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
#if 1
		GenFile();

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
		if (!CreateShaderEx(vexterShader, GL_VERTEX_SHADER, vs))
		{
			return false;
		}

		if (!CreateShaderEx(fragmentShader, GL_FRAGMENT_SHADER, fs))
		{
			return false;
		}
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
		//���η���ռ�
		bitmap.Alloc(width, height, 32);

		glGenTextures(1, &texInput);
		glBindTexture(GL_TEXTURE_2D, texInput);

		//������ں˳�����ͼ��߽�ʱʹ��ͼ���Ե������ֵ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		//������˵ķ�ʽ��Ӧ������Ϊ���Բ�ֵ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap.bits);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glGenTextures(1, &texOutput);
		glBindTexture(GL_TEXTURE_2D, texOutput);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		//������˵ķ�ʽ��Ӧ������Ϊ���Բ�ֵ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//û�и�������������ݣ��ȴ�������и�ֵ
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glGenTextures(1, &texMatte);
		glBindTexture(GL_TEXTURE_2D, texMatte);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		//������˵ķ�ʽ��Ӧ������Ϊ���Բ�ֵ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//û�и�������������ݣ��ȴ�������и�ֵ
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
		static PIXELFORMATDESCRIPTOR pfd =	//�������ظ�ʽ
		{
			sizeof(PIXELFORMATDESCRIPTOR),	// ������ʽ�������Ĵ�С
			1,								// �汾��
			PFD_DRAW_TO_WINDOW |			// ��ʽ֧�ִ���
			PFD_SUPPORT_OPENGL |			// ��ʽ����֧��OpenGL
			PFD_DOUBLEBUFFER,				// ����֧��˫����
			PFD_TYPE_RGBA,					// ���� RGBA ��ʽ
			24,								// 24λɫ����ȣ���1.67ǧ������ɫ
			0, 0, 0, 0, 0, 0,				// ���Ե�ɫ��λ
			0,								// ��Alpha����
			0,								// ����Shift Bit
			0,								// ���ۼӻ���
			0, 0, 0, 0,						// ���Ծۼ�λ
			32,								// 32λ Z-���� (��Ȼ���)
			0,								// ���ɰ建��
			0,								// �޸�������
			PFD_MAIN_PLANE,					// ����ͼ��
			0,								// Reserved
			0, 0, 0							// ���Բ�����
		};

		int index = ChoosePixelFormat(dc, &pfd); //ѡ��ոն�������ظ�ʽ
		if (index == 0) return false;

		return SetPixelFormat(dc, index, &pfd) ? true : false;   //�������ظ�ʽ
	}

	bool OpenGL::SetMatte(uint8 * buffer, int width, int height, int bitCount)
	{
		if (buffer != NULL)
		{
			glBindTexture(GL_TEXTURE_2D, texMatte);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

			//������˵ķ�ʽ��Ӧ������Ϊ���Բ�ֵ
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			//û�и�������������ݣ��ȴ�������и�ֵ
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

	void OpenGL::SetParam(int radius, float sigma)
	{
		this->radius = radius;
		this->sigma = sigma;
	}

	inline double Calc(int x, int y, float sigma)
	{
		return exp(-(x*x + y*y) / (2 * sigma * sigma)) / (2 * 3.141592654 * sigma * sigma);
	}

	void OpenGL::MakeGauss(char** buffer, int size)
	{
		assert(buffer != 0);
		double values[30][30] = { 0 };
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

	void OpenGL::GenFile(void)
	{
		int size = 4096 * 5;
		char* buffer = new char[size];
		memset(buffer, 0, sizeof(char) * size);

		char* kernal = new char[size];
		memset(kernal, 0, sizeof(char)* size);

		int count = (radius * 2 + 1) * (radius * 2 + 1);

		MakeGauss(&kernal, size);

		sprintf_s(buffer
			, size
			, "uniform sampler2D Texture0;															\n"
			"uniform sampler2D Texture1;															\n"
			"uniform vec2 TexSize;																	\n"
			"uniform int enableMatte;																\n"
			"float kernal[%d];																		\n"

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

			"vec4 gauss_filter(float kernal[%d], int radius, sampler2D image, vec2 uv, vec2 texSize)\n"
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
			"	int radius = %d;																	\n"
			"%s\n"
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
			, count
			, count
			, radius
			, kernal);

		char path[MAX_PATH] = { 0 };
		GetExecuteDirectoryA(path, MAX_PATH);
		strcat_s(path, MAX_PATH, "\\filter.fs");

		FILE* fp = NULL;
		fopen_s(&fp, path, "w");
		fwrite(buffer, 1, strlen(buffer) + 1, fp);
		fclose(fp);

		delete[] kernal;
		delete[] buffer;
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
