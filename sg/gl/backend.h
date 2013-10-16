#ifndef BACKEND_H
#define BACKEND_H

#if defined(__APPLE__)
	#include <GL/glew.h>
	#ifdef GL_ES_VERSION_2_0
		#include <OpenGLES/ES2/gl.h>
	#else
		#include <OpenGL/gl.h>
	#endif
#elif defined(_WIN32) || defined(_WIN64)
	#include <GL/glew.h>
	#include <GL/wglew.h>
#else
	#include <GL/glew.h>
	#include <GL/gl.h>
#endif
#include <GLFW/glfw3.h>

#endif /* BACKEND_H */
