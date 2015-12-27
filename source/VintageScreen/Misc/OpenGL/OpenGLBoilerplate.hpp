#pragma once

#include <functional>

#include <Misc\OpenGL\GLSLHelper.hpp>
#include <Misc\Tools\Callback.hpp>
#include <Misc\Debug\Assert.hpp>

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>

struct OpenGLInfo {
	unsigned windowWidth;
	unsigned windowHeight;
	std::string windowTitle;
	std::function<void(void)> drawCallback;
	std::function<void(void)> idleCallback;
	std::function<void(void)> initCallback;
};

class OpenGLBoilerplate {

	enum { 
		IDLE_ID = GEN_GUID(),
		DRAW_ID = GEN_GUID()
	};

public:
	typedef void (*CFuncType)(void);

private:
	OpenGLInfo glInfo;

public:
	OpenGLBoilerplate(){}

	explicit OpenGLBoilerplate(const OpenGLInfo &oi){
		glInfo = oi;

		RT_ASSERT(oi.drawCallback != false, "draw function is needed for initialization");
		RT_ASSERT(oi.idleCallback != false, "idle function is needed for initialization");
	}

	void loadGLInfo(const OpenGLInfo &oi) {
		glInfo = oi;
		
		RT_ASSERT(oi.drawCallback != false, "draw function is needed for initialization");
		RT_ASSERT(oi.idleCallback != false, "idle function is needed for initialization");
	}

	void display() {
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glInfo.drawCallback();

		glutSwapBuffers();
	}

	void idle() {
		glInfo.idleCallback();
	}

	void start() {
		// dummy args for glut
		int argc = 0;
		char *argv[] = {""};

		glutInit(&argc, argv);
		glutInitWindowSize(glInfo.windowWidth, glInfo.windowHeight);
		glutInitWindowPosition(100, 100);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

		glutCreateWindow(glInfo.windowTitle.c_str());

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		// after creating the glut context, we can init glew and opengl
		glInfo.initCallback();

		Callback<DRAW_ID, void(void)>::func = std::bind(&OpenGLBoilerplate::display, this);
		CFuncType rawDrawFn = static_cast<CFuncType>(Callback<DRAW_ID, void(void)>::callback);
		glutIdleFunc(rawDrawFn);

		Callback<IDLE_ID, void(void)>::func = std::bind(&OpenGLBoilerplate::idle, this);
		CFuncType rawIdleFn = static_cast<CFuncType>(Callback<IDLE_ID, void(void)>::callback);
		glutDisplayFunc(rawIdleFn);

		glutMainLoop();
	}

};